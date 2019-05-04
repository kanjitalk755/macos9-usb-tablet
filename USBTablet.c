#include <USB.h>
#include <HID.h>
#include <CursorDevices.h>

static HIDDeviceDispatchTablePtr sDev;
static HIDPreparsedDataRef sReport;
static HIDDeviceConnectionRef sCon;

static void InitToolbox() {
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	FlushEvents(everyEvent, 0);
	TEInit();
	InitDialogs(0L);
	InitCursor();
}

static void SetupMenu() {
	Handle menuBar = GetNewMBar(128);
	SetMenuBar(menuBar);
	DisposeHandle(menuBar);
	AppendResMenu(GetMenuHandle(128), 'DRVR');	
	DrawMenuBar();
}

static int doMenuCommand(long menuChoice) {
	int menu, item, r = 0;
	Str255 accName;
	if (menuChoice) {
		menu = HiWord(menuChoice);
		item = LoWord(menuChoice);
		switch (menu) {
			case 128:
				if (item == 1) {
					ParamText("\pUSB Tablet", nil, nil, nil);
					Alert(128, nil);
				}
				else {
					GetMenuItemText(GetMenuHandle(128), item, accName);
					OpenDeskAcc(accName);
				}
				break;
			case 129:
				if (item == 1) r = 1;
				break;
		}
		HiliteMenu(0);
	}
	return r;
}

static int doMouseDown(EventRecord *e) {
	WindowPtr win;
	return FindWindow(e->where, &win) == inMenuBar ? doMenuCommand(MenuSelect(e->where)) : 0;
}

static int doEvent(EventRecord *e) {
	switch (e->what) {
		case mouseDown:
			return doMouseDown(e);
		case keyDown: case autoKey:
			if (e->modifiers & cmdKey) return doMenuCommand(MenuKey(e->message & charCodeMask));
			break;
		case kHighLevelEvent:
			AEProcessAppleEvent(e);
			break;
	}
	return 0;
}

static void ReportHandler(void *r, UInt32 len, UInt32 rc) {
#pragma unused(rc)
	CursorDevice *c = nil;
	CursorDeviceNextDevice(&c);
	if (c && len == 6) {
		SInt32 x, y;
		if (!HIDGetUsageValue(kHIDInputReport, 1, 0, 48, &x, sReport, r, len) &&
			!HIDGetUsageValue(kHIDInputReport, 1, 0, 49, &y, sReport, r, len)) {
			Rect rect = (*GetGrayRgn())->rgnBBox;
			CursorDeviceMoveTo(c, rect.right * x >> 15, rect.bottom * y >> 15);
		}
		HIDUsageAndPage u;
		UInt32 uSize = 1;
		int b = !HIDGetButtons(kHIDInputReport, 0, &u, &uSize, sReport, r, len) &&
			uSize == 1 && u.usage == 1 && u.usagePage == 9;
		static int l;
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
		CFragConnectionID c;
		err = USBGetNextDeviceByClass(&dev, &c, kUSBHIDClass, kUSBAnySubClass, kUSBAnyProtocol);
		if (err) return err;
		THz currentZone = GetZone();
		SetZone(SystemZone());
		CFragSymbolClass s;
		err = FindSymbol(c, "\pTheHIDDeviceDispatchTable", (Ptr *)&sDev, &s);
		SetZone(currentZone);
	} while (err);
	return noErr;
}

static OSErr Prepare() {
	UInt32 len = 0;
	OSErr err = (*sDev->pHIDGetHIDDescriptor)(kUSBReportDesc, 0, nil, &len);
	if (err) return err;
	UInt8 *r = (UInt8 *)NewPtrClear(len);
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

static void Uninstall() {
	if (sDev && sCon) {
		(*sDev->pHIDRemoveReportHandler)(sCon);
		(*sDev->pHIDCloseDevice)(sCon);
	}
	if (sReport) HIDCloseReportDescriptor(sReport);
}

static pascal OSErr quit(AppleEvent *e, AppleEvent *r, long rc) {
#pragma unused(e, r, rc)
	Uninstall();
	ExitToShell();
	return noErr;
}

int main(void) {
	InitToolbox();
	SetupMenu();
	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP(quit), 0, 0);
	if (!FindDevice() && !Prepare() && !Install()) {
		if (sDev->pHIDGetReport) (*sDev->pHIDGetReport)(sCon, kHIDInputReport, 0, ReportHandler, 0);
		while (1) {
			EventRecord e = { 0 };
			if (WaitNextEvent(everyEvent, &e, -1, nil) && doEvent(&e)) break;
		}
	}
	Uninstall();
	return 0;
}
