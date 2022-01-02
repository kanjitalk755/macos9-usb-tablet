#include <A4Stuff.h>

#define	LMVCheckSum			(*(UInt16 *)0x928)
#define	LMVCoord			(*(UInt16 *)0x92A)
#define	LMHCoord			(*(UInt16 *)0x92C)
#define	LMHCheckSum			(*(UInt16 *)0x92E)
#define	CheckSum(x)			(((x) << 1 | (x) >> 15) ^ 0x1021)

static void ShowInitIcon(void) {
	UInt32 saveA5;
	QDGlobals qd;
	UInt32 qdGlobalsPtr;
	CGrafPort colorPort;
	Point pt;
	
	saveA5 = SetA5((UInt32)&qdGlobalsPtr);
	InitGraf(&qd.thePort);
	
	if (CheckSum(LMHCoord) != LMHCheckSum) LMHCoord = 8;
	if (CheckSum(LMVCoord) != LMVCheckSum) LMVCoord = qd.screenBits.bounds.bottom - 40;
	
	pt = LMHCoord + 34 <= qd.screenBits.bounds.right ? (Point){ LMVCoord, LMHCoord } : (Point){ LMVCoord - 40, 8 };

	OpenCPort(&colorPort);
	PlotIconID(&(Rect){ pt.v, pt.h, pt.v + 32, pt.h + 32 }, atNone, ttNone, -16455);
	CloseCPort(&colorPort);

	LMHCoord = pt.h + 40;
	LMVCoord = pt.v;
	LMHCheckSum = CheckSum(LMHCoord);
	LMVCheckSum = CheckSum(LMVCoord);
		
	SetA5(saveA5);
}

void main(void) {
	Handle hd;
	UInt32 saveA4 = SetCurrentA4();
	SetZone(SystemZone());
	hd = Get1Resource('PPC ', 128);
	if (hd) {
		UInt8 buf[256];
		RoutineDescriptor rd = BUILD_ROUTINE_DESCRIPTOR(kCStackBased, 0);
		ProcPtr procPtr = 0;
		CFragConnectionID id;
		DetachResource(hd);
		HLockHi(hd);
	  	if (!GetMemFragment(*hd, GetHandleSize(hd), "\p", kPrivateCFragCopy, &id, (Ptr *)&procPtr, buf)) {
			rd.routineRecords[0].ISA = kPowerPCISA | kPowerPCRTA;
			rd.routineRecords[0].procDescriptor = procPtr;
			CALL_ZERO_PARAMETER_UPP((UniversalProcPtr)&rd, uppINITProcInfo);
		}
	}
	SetZone(ApplicationZone());
	ShowInitIcon();
	SetA4(saveA4);
}
