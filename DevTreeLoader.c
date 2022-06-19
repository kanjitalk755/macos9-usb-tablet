// Replace the ROM resource USBHIDMouseModule with a PEF appended to this one
// Participates in the early boot process

#include <CodeFragments.h>
#include <Memory.h>
#include <Resources.h>
#include <Types.h>

#include "nkprintf.h"

#ifndef BUG
#define BUG 0
#endif

// Patch to "re-patch" the ROM resource handle
// Apply to any trap taking a handle in a0
// Come-from patch is protected when the PowerPC Resource Manager is installed
short patchTemplate[] = {
	0x6006,                 //  +0          bra.s   mycode    (come-from patch sig)
	0x4ef9, 0x0000, 0x0000, //  +2  orig    jmp     $00000000 (come-from patch sig)
	0xb1fc, 0x0000, 0x0000, //  +8  mycode  cmp.l   #$00000000,a0
	0x6606,                 //  +e          bne.s   escape
	0x20bc, 0x0000, 0x0000, // +10          move.l  #$00000000,(a0)
	0x60ea,                 // +16  escape  bra.s   orig
};

#define patchHandOffset 0xa
#define patchPtrOffset 0x12
#define patchJmpOffset 0x4

// HSetState
#define trapTabAddr (0x400 + 4 * 0x6a)

OSErr init(CFragInitBlockPtr initBlock);

OSErr init(CFragInitBlockPtr initBlock) {
	void **romDriverHand;
	long *driverPEF;
	char *patch;
	long i;

	// Get the handle whose master pointer we must change
	*(short *)0xb9e = 0xffff; // TempInsertROMMapTrue
	romDriverHand = (void **)GetResource('usbd', -20779);
	if (BUG) nkprintf("%s:%d ROM usbd pre-patch: hand=%#x ptr=%#x\n", __FILE__, __LINE__, romDriverHand, *romDriverHand);

	// If handle is already in RAM then do not re-patch
	if (*romDriverHand < *(void **)0x2ae) {
		if (BUG) nkprintf("%s:%d already patched\n", __FILE__, __LINE__);
		return paramErr;
	}

	patch = NewPtr(sizeof patchTemplate);
	for (i = 0; i < sizeof patchTemplate; i++) {
		((char *)patch)[i] = ((char *)patchTemplate)[i];
	}

	// Get address of this code fragment
	driverPEF = initBlock->fragLocator.u.inMem.address;
	do {
		driverPEF++;
	} while (*driverPEF != 'Joy!');
	if (BUG) nkprintf("%s:%d new driver ptr=%#x\n", __FILE__, __LINE__, driverPEF);

	// Change the master ptr persistently, every time this trap is called
	*(void **)(patch + patchHandOffset) = romDriverHand;
	*(void **)(patch + patchPtrOffset) = driverPEF;
	*(void **)(patch + patchJmpOffset) = *(void **)trapTabAddr;
	*(void **)trapTabAddr = patch;

	// Test the change
	if (BUG) {
		*(short *)0xb9e = 0xffff; // TempInsertROMMapTrue
		romDriverHand = (void **)GetResource('usbd', -20779);
		nkprintf("%s:%d ROM usbd post-patch hand=%#x ptr=%#x\n", __FILE__, __LINE__, romDriverHand, *romDriverHand);
	}

	// Ensure that this fragment is unloaded
	return paramErr;
}
