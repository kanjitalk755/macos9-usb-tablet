#include <A4Stuff.h>

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
	SetA4(saveA4);
}
