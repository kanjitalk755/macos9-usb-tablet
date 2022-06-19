#include <Events.h>
#include <USB.h>
#include <Types.h>

#include "nkprintf.h"

enum {
	stateInit,
	stateConfigureInterface,
	stateFindNextPipe,
	stateIntRead,
};

OSStatus returnNoErr(void);
OSStatus interfaceInitialize(UInt32 interfaceNum, USBInterfaceDescriptorPtr pInterface, USBDeviceDescriptorPtr pDevice, USBInterfaceRef interfaceRef);
void stateMachine(USBPB *pb);

UInt8 ifnum;

USBPB pb;

USBPB pbClean = {
	NULL,           // qLink
	0,              // qType
	sizeof(USBPB),  // pbLength ?ensures we are wired in memory
	0x0100,         // pbVersion
	0, 0,           // reserved
	0,              // usbStatus
	stateMachine,   // usbCompletion
	stateInit,      // usbRefcon
};

unsigned char report[16];
unsigned char prevButton;

USBDriverDescription TheUSBDriverDescription = {
	// Signature info
	kTheUSBDriverDescriptionSignature,
	kInitialUSBDriverDescriptor,

	// Device Info
	0x0627,                                 // vendor = QEMU
	0x0001,                                 // product = QEMU mouse/tablet
	0,                                      // version of product = not device specific
	0,                                      // protocol = not device specific

	// Interface Info
	0,                                      // Configuration Value
	0,                                      // Interface Number
	3,                                      // Interface Class: HID
	kUSBNoInterfaceSubClass,                // Interface SubClass: 0 or 1 is fine
	kUSBNoInterfaceProtocol,                // Interface Protocol: neither kbd nor mouse

	// Driver Info
	"\pQEMUTab",                            // Driver name for Name Registry
	0,                                      // Device Class  (from USBDeviceDefines.h)
	0,                                      // Device Subclass
	0x01, 0x00, finalStage, 0x00,           // version of driver

	// Driver Loading Info
	kUSBInterfaceMatchOnly,                 // Flags
};

USBClassDriverPluginDispatchTable TheClassDriverPluginDispatchTable = {
	kClassDriverPluginVersion,              // Version of this structure
	(void *)returnNoErr,                    // Hardware Validation Procedure
	NULL,                                   // Initialization Procedure
	interfaceInitialize,                    // Interface Initialization Procedure
	(void *)returnNoErr,                    // Finalization Procedure
	NULL,                                   // Driver Notification Procedure
};

OSStatus returnNoErr(void) {
	return noErr;
}

OSStatus interfaceInitialize(UInt32 interfaceNum, USBInterfaceDescriptorPtr pInterface, USBDeviceDescriptorPtr pDevice, USBInterfaceRef interfaceRef) {
	(void)interfaceNum;
	(void)pDevice;

	pbClean.usbReference = interfaceRef;
	ifnum = pInterface->interfaceNumber;
	stateMachine(&pb);
	return noErr;
}

void stateMachine(USBPB *_pb) {
	(void)_pb; // use global pb instead

	//nkprintf("stateMachine usbRefcon=%d usbStatus=%d\n", pb.usbRefcon, pb.usbStatus);

	switch (pb.usbRefcon) {
	case stateInit:
		pb = pbClean;
		pb.usbRefcon = stateConfigureInterface;
		USBConfigureInterface(&pb);
		break;

	case stateConfigureInterface:
		pb = pbClean;
		pb.usbRefcon = stateFindNextPipe;

		pb.usbFlags = kUSBIn;
		pb.usbClassType = kUSBInterrupt;

		USBFindNextPipe(&pb);

	case stateFindNextPipe:
		// Future calls will ask for the pipe reference
		pbClean.usbReference = pb.usbReference;
		// fall through

	case stateIntRead:
		//nkprintf("%08x %08x\n", *(long *)report, *(long *)(report + 4));

		if (pb.usbStatus == noErr) {
			unsigned long point;

			// byte 5 is a signed scroll amount

			point = ((((unsigned long)report[4] << 8) | report[3]) * *(unsigned short *)0xc22 >> 15 << 16) |
				((((unsigned long)report[2] << 8) | report[1]) * *(unsigned short *)0xc20 >> 15);

			*(unsigned long *)0x828 = point; // MTemp
			*(unsigned long *)0x82c = point; // RawMouse

			*(char *)0x8ce = *(char *)0x8cf; // CrsrNew = CrsrCouple

			if (!report[0] != !prevButton) {
				EvQEl *event;

				*(unsigned char *)0x172 = report[0] ? 0 : 0x80;
				PPostEvent(report[0] ? mouseDown : mouseUp, 0, &event);

				// Right-click becomes control-click
				if (report[0] & 2) {
					event->evtQModifiers |= 0x1000;
				}

				prevButton = report[0];
			}
		}

		pb = pbClean;
		pb.usbRefcon = stateIntRead;
		pb.usbBuffer = report;
		pb.usbReqCount = sizeof report;
		pb.usb.cntl.WIndex = ifnum;

		USBIntRead(&pb);
		break;
	}
}
