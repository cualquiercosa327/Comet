#include "reload.hpp"
#include <libpokey/hooks.h>

#include <libpokey/debug.h>
#include <modules/prologue/prologue.hpp>

#include <revolution/pad.h>
#include <EGG/core/eggExpHeap.hpp>

#include <lib/IModule.hpp>
#include <libpokey/mkw/environment.h>
#include <revolution/dvd.h>

#include <lib/CometSystem.hpp>

namespace Reload {
#ifdef DEBUG


void reload()
{
	BOOL iState = OSDisableInterrupts();

	DVDFileInfo fileInfo;
	bool success = false;
	u8* block = (u8*)0x809c4fa0;//getBlock();

	PokeyVerboseReport("---\nRELOADING\n---\n");

	//CometSystem::getSystem()->mModuleLoader.unloadModules();

	if (DVDOpen(PATH_CODE_BIN, &fileInfo))
	{
		u32 fileLen = fileInfo.length;
		PokeyDebugReport("File len: %u\n", fileLen);
		u32 fileLen32 = OSRoundUp32B(fileLen);
		PokeyDebugReport("File len (rounded): %u\n", fileLen32);
		u32 amountRead = DVDRead(&fileInfo, block, fileLen32, 0);
		DVDClose(&fileInfo);
		if (fileLen <= amountRead)
		{
			// PokeyDebugReport("Loaded file!\n");
			success = true;
		}
	}
	if (!success)
		PokeyDebugReport("Reload Failed!\n");
	
	
	//PokeyDebugReport("Calling prologue: %p\n", block);


	// Call the prologue, again. this will load patches
	((u32(*)(void))0x809c4fa0)();

	PokeyDebugReport("Reload Success!\n");
	OSRestoreInterrupts(iState);
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