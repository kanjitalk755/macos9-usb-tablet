#include <lowmem.h>
#include <USB.h>
#include <HID.h>
#include <CursorDevices.h>

static HIDDeviceDispatchTablePtr sDev;
static HIDPreparsedDataRef sReport;
static HIDDeviceConnectionRef sCon;

static void ReportHandler(void *r, UInt32 len, UInt32 rc) {
#pragma unused(rc)
	CursorDevice *c = nil;
	CursorDeviceNextDevice(&c);
	if (c && len == 6) {
		static UInt32 l;
		HIDUsageAndPage u;
		UInt32 b, uSize = 1;
		SInt32 x, y;
		if (!HIDGetUsageValue(kHIDInputReport, 1, 0, 48, &x, sReport, r, len) &&
			!HIDGetUsageValue(kHIDInputReport, 1, 0, 49, &y, sReport, r, len)) {
			Rect rect = (*GetGrayRgn())->rgnBBox;
			CursorDeviceMoveTo(c, rect.right * x >> 15, rect.bottom * y >> 15);
		}
		b = !HIDGetButtons(kHIDInputReport, 0, &u, &uSize, sReport, r, len) &&
			uSize == 1 && u.usage == 1 && u.usagePage == 9;
		if (!l && b) CursorDeviceButtonDown(c);
		else if (l && !b) CursorDeviceButtonUp(c);
		l = b;
	}
	if (sDev->pHIDCallPreviousReportHandler && sCon) 
		(*sDev->pHIDCallPreviousReportHandler)(sCon, r, len);
}

static OSErr FindDevice() {
	OSErr err;
	USBDeviceRef dev = kNoDeviceRef;
	do {
		CFragSymbolClass s;
		THz currentZone;
		CFragConnectionID c;
		err = USBGetNextDeviceByClass(&dev, &c, kUSBHIDClass, kUSBAnySubClass, kUSBAnyProtocol);
		if (err) return err;
		currentZone = GetZone();
		SetZone(SystemZone());
		err = FindSymbol(c, "\pTheHIDDeviceDispatchTable", (Ptr *)&sDev, &s);
		SetZone(currentZone);
	} while (err);
	return noErr;
}

static OSErr Prepare() {
	UInt32 len = 0;
	UInt8 *r;
	OSErr err = (*sDev->pHIDGetHIDDescriptor)(kUSBReportDesc, 0, nil, &len);
	if (err) return err;
	r = (UInt8 *)NewPtrClear(len);
	if (!r) return MemError();
	err = (*sDev->pHIDGetHIDDescriptor)(kUSBReportDesc, 0, r, &len);
	if (!err) err = HIDOpenReportDescriptor(r, len, &sReport, kHIDFlag_StrictErrorChecking);
	DisposePtr((Ptr)r);
	return err;
}

static OSErr Install() {
	HIDDeviceConnectionRef tmp;
	OSErr err = (*sDev->pHIDOpenDevice)(&tmp, kHIDPerm_ReadWriteShared, 0);
	if (err) return err;
	err = (*sDev->pHIDInstallReportHandler)(tmp, 0, ReportHandler, 0);
	if (err) {
		(*sDev->pHIDCloseDevice)(tmp);
		return err;
	}
	sCon = tmp;
	return noErr;
}

void main(void) {
	if (!FindDevice() && !Prepare() && !Install() && sDev->pHIDGetReport)
		(*sDev->pHIDGetReport)(sCon, kHIDInputReport, 0, ReportHandler, 0);
}
