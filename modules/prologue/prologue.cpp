#include "prologue.hpp"

#include <EGG/core/eggDvdRipper.hpp>
#include <EGG/core/eggExpHeap.hpp>

#include <Common/rk_types.h>

#include <libpokey/debug.h>
#include <libpokey/mkw/heaps.hpp>
#include <libpokey/mkw/environment.h>

#include <lib/ModuleLoader.hpp>

// for ctors
#include <modules/reload/reload.hpp>
#include <modules/bootstrap/bootstrap.hpp>

#include <vector.h>

namespace Prologue {


void applyPatches(u32* block);
void doPatch32(void* patch);


// The first function will be the entry point.
void prologue()
{
	std::vector<u8> patch_block;
	DVDFileInfo fileInfo;
	BOOL iState = OSDisableInterrupts();
	u32 amountRead;
	void* buf = (void*)OSRoundUp32B((u32)&patch_block[0]);

	// Read patches from disc
	if (DVDOpen(PATH_PATCH_BIN, &fileInfo))
	{
		u32 fileLen = fileInfo.length;
		patch_block.resize((fileLen - 8) / 4);
		DebugReport("Expecting %u patches\n", patch_block.size());
		u32 fileLen32 = OSRoundUp32B(fileLen);

		amountRead = DVDRead(&fileInfo, buf, fileLen32, 0);
		DebugReport("Read %u bytes\n", amountRead);
		DVDClose(&fileInfo);
		if (fileLen32 > amountRead)
		{
			DebugReport("Failed to load PATCH.bin!\n");
		}
		else
		{
			// NULL terminated
			applyPatches((u32*)buf);
			DebugReport("Success!\n");
		}
	}

	// call prologue functions
	
	// TODO: global system
	ModuleLoader moduleLoader;

	moduleLoader.appendNewModule<Reload::Reloader>();
	moduleLoader.appendNewModule<Bootstrap::Bootstrap>();

	moduleLoader.loadModules();

	DebugReport("DONE\n");



	OSRestoreInterrupts(iState);
}


#if 0
// ensure enough space for all patches

patchEntry patchRecord[NUM_PATCH_HISTORY];
int patchCursor = 0;

patchEntry* getPatchEntries()
{
	return &patchRecord[0];
}
int getPatchCursor()
{
	return patchCursor;
}

#endif
inline void applyPatches(u32* block)
{
	int i;
	for (i = 0; block[i]; i += 2)
	{
		u32* dst = (u32*)block[i];
		u32  val = block[i + 1];
#if 0
		patchRecord[patchCursor].addr = dst;
		patchRecord[patchCursor].oldVal = *dst;
		patchCursor++;
#endif
		DebugReport("Patching 32: %p = 0x%08x\n", dst, val);
		doPatch32(&block[i]);
	}
	DebugReport("Made %u patches!\n", i / 2);
}
inline void doPatch32(void* patch)
{

	__asm
	{
		lwz r4, 4(r3) // value
		lwz r3, 0(r3) // address to write to
		stw r4, 0(r3)

		// cache
		dcbf 0, r3
		sync
		icbi 0, r3
		isync
	}
}

} // namespace Prologue