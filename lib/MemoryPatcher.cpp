#include <lib/MemoryPatcher.hpp>
#include <revolution/dvd.h>
#include <libpokey/mkw/environment.h>


MemoryPatcher::MemoryPatcher()
{
	numPatches = 0;
}
MemoryPatcher::~MemoryPatcher()
{}
void MemoryPatcher::processPatchFile(u32* block)
{
	int i = 0;
	for (Patch32* pPatch = (Patch32*)block;
		pPatch->addr;
		++pPatch, ++i)
	{
		DebugAssert(numPatches < NUM_PATCH_HISTORY);

		mPatchRecord[numPatches].addr = pPatch->addr;
		mPatchRecord[numPatches].val = *(u32*)pPatch->addr;
		mPatchRecord[numPatches].newVal = pPatch->val;
		patch(pPatch);
		numPatches++;

	}
	DebugReport("Made %u patches!\n", i);
}
void MemoryPatcher::revertPatches()
{
	for (PatchRecord* it = &mPatchRecord[0]; numPatches, it != &mPatchRecord[numPatches]; it++)
	{
		DebugReport("REVERTPATCHES: Revert 32: %p was 0x%08x patched to 0x%08x\n", it->addr, it->val, it->newVal);
		patch((Patch32*)&it->addr);
	}
	// Setting num to 0 effectively clearing
	numPatches = 0;
}

void funcReqFail(const char* exp, const char* msg)
{
	DebugReport("Function fail: %s (%s)\n", msg, exp);
}

#define FUNC_REQUIRES(exp, msg) do { \
	if( ! (exp) ) { funcReqFail(#exp, msg); return; } \
} while (0);
void MemoryPatcher::processDiscPatchFile()
{
	std::vector<u8> patch_block;
	DVDFileInfo fileInfo;
	
	FUNC_REQUIRES(DVDOpen(PATH_PATCH_BIN, &fileInfo), "Failed to open patch file!\n");
	
	
	u32 fileLen = fileInfo.length;
	u32 fileLen32 = OSRoundUp32B(fileLen);

	DebugReport("Expecting %u patches\n", fileLen32);

	u32 amountRead = DVDRead(&fileInfo, (void*)OSRoundUp32B(&patch_block[0]), fileLen32, 0);
	DebugReport("Read %u bytes\n", amountRead);
	DVDClose(&fileInfo);
	
	FUNC_REQUIRES(fileLen32 <= amountRead, "Did not read enough bytes")
			
	DebugReport("Applying patch file..\n");

	// NULL terminated
	processPatchFile((u32*)OSRoundUp32B((u32)&patch_block[0]));
	DebugReport("Success!\n");
}
void MemoryPatcher::patch(MemoryPatcher::Patch32* block)
{ __asm {
	lwz r4, 4(r3); // value
	lwz r3, 0(r3); // dest address
	stw r4, 0(r3);

	// cache
	dcbf 0, r3;
	sync;
	icbi 0, r3;
	isync;
} }

void MemoryPatcher::scanForChanges()
{
	DebugReport("Watcher: Periodic scan!\n");

	for (PatchRecord* it = &mPatchRecord[0]; numPatches, it != &mPatchRecord[numPatches]; it++)
	{
		if (*(u32*)it->addr != it->newVal)
		{
			if ((u32)it->addr == it->val)
				DebugReport("Watcher: Revert 32: %p was 0x%08x patched to 0x%08x\n", it->addr, it->val, it->newVal);
			else
				DebugReport("Watcher: %p was overwriten (was %p, patched to %p, now %p)\n", it->addr, it->val, it->newVal, *(u32*)it->addr);
		}
	}
}