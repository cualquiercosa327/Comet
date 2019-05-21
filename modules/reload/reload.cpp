#include "reload.hpp"
#include <libpokey/hooks.h>

#include <libpokey/debug.h>
#include <modules/prologue/prologue.hpp>
#include <lib/GlobalsManager.hpp>

#include <revolution/pad.h>
#include <EGG/core/eggExpHeap.hpp>

#include <libpokey/mkw/environment.h>
#include <revolution/dvd.h>

#include <lib/CometSystem.hpp>

namespace Reload {

// We need this here, as the reload module cannot change for a reload to work
void _rprologue()
{
	Prologue::prologue();
}

#ifdef DEBUG


void reload()
{
POKEY_BEGIN_NO_MULTITASK

	DVDFileInfo fileInfo;
	bool success = false;
	
	DebugReport("---\nRELOADING\n---\n");

	GlobalsManager::destroyGlobals();

	if (DVDOpen(PATH_CODE_BIN, &fileInfo))
	{
		u32 fileLen = fileInfo.length;
		PokeyDebugReport("File len: %u\n", fileLen);
		u32 fileLen32 = OSRoundUp32B(fileLen);
		PokeyDebugReport("File len (rounded): %u\n", fileLen32);
		u32 amountRead = DVDRead(&fileInfo, (void*)0x809c4fa0, fileLen32, 0);
		DVDClose(&fileInfo);
		if (fileLen <= amountRead)
		{
			// PokeyDebugReport("Loaded file!\n");
			success = true;
		}
	}
	if (!success)
		PokeyDebugReport("Reload Failed!\n");

	// Call the prologue, again. this will load patches
	((u32(*)(void))0x809c4fa0)();

	PokeyDebugReport("Reload Success!\n");

POKEY_END_NO_MULTITASK
}

#if 1
void loopAdditions()
{
	PADStatus padStatusArr[PAD_MAX_CONTROLLERS];

	PADRead(padStatusArr);
	if (padStatusArr[0].button & PAD_BUTTON_X)
	{
		reload();
	}

}
PokeyBranch(0x80009824, loopAdditions);
#endif


#endif
} // namespace Reload