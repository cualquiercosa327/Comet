#include <lib/MemoryPatcher.hpp>
#include <revolution/dvd.h>
#include <libpokey/mkw/environment.h>


MemoryPatcher::MemoryPatcher()
{}
MemoryPatcher::~MemoryPatcher()
{}
void MemoryPatcher::processPatchFile(u32* block)
{
	int i;
	for (i = 0; block[i]; i += 2)
	{
		u32* dst = (u32*)block[i];
		u32  val = block[i + 1];

		mPatchRecord.push_back(PatchRecord(dst, *dst, val));

		DebugReport("Patching 32: %p = 0x%08x\n", dst, val);
		patch((Patch32*)&block[i]);
	}
	DebugReport("Made %u patches!\n", i / 2);
}
void MemoryPatcher::revertPatches()
{
	for (std::vector<PatchRecord>::iterator it = mPatchRecord.begin(); it != mPatchRecord.end(); it++)
	{
		DebugReport("Revert 32: %p was 0x%08x patched to 0x%08x\n", (*it).address, (*it).oldVal, (*it).newVal);
		patch((Patch32*)&(*it).address);
	}
	mPatchRecord.clear();
}
void MemoryPatcher::processDiscPatchFile()
{
	std::vector<u8> patch_block;
	DVDFileInfo fileInfo;
	void* buf = (void*)OSRoundUp32B((u32)&patch_block[0]);
	// Read patches from disc
	if (DVDOpen(PATH_PATCH_BIN, &fileInfo))
	{
		u32 fileLen = fileInfo.length;
		patch_block.resize((fileLen - 8) / 8);
		DebugReport("Expecting %u patches\n", patch_block.size());
		u32 fileLen32 = OSRoundUp32B(fileLen);

		u32 amountRead = DVDRead(&fileInfo, buf, fileLen32, 0);
		DebugReport("Read %u bytes\n", amountRead);
		DVDClose(&fileInfo);
		if (fileLen32 > amountRead)
		{
			DebugReport("Failed to load PATCH.bin!\n");
		}
		else
		{
			// NULL terminated
			processPatchFile((u32*)buf);
			DebugReport("Success!\n");
		}
	}
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
	for (std::vector<PatchRecord>::iterator it = mPatchRecord.begin(); it != mPatchRecord.end(); it++)
	{
		if (*(u32*)(*it).address != (*it).newVal)
		{
			if ((u32)(*it).address == (*it).oldVal)
				DebugReport("Watcher: Revert 32: %p was 0x%08x patched to 0x%08x\n", (*it).address, (*it).oldVal, (*it).newVal);
			else
				DebugReport("Watcher: %p was overwriten (was %p, patched to %p, now %p)\n", (*it).address, (*it).oldVal, (*it).newVal, *(u32*)(*it).address);
		}
	}
}