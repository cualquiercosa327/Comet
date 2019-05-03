#include "bootstrap.hpp"
#include <libpokey/hooks.h>
#include <EGG/core/eggHeap.hpp>
#include <revolution/dvd.h>
#include <Static/System/SystemManager.hpp>

namespace Bootstrap {
// filepaths from all regions combined
static const char* BootStrapFilePaths[] = {
	"/Boot/Strap/jp/jp.szs",
	"/Boot/Strap/eu/English.szs",
	"/Boot/Strap/eu/German.szs",
	"/Boot/Strap/eu/French.szs",
	// TODO: support Spanish_US
	"/Boot/Strap/eu/Spanish_EU.szs",
	"/Boot/Strap/eu/Italian.szs",
	"/Boot/Strap/eu/Dutch.szs"
};

const char** getBootStrapFilePaths()
{
	return BootStrapFilePaths;
}

u8* bootStrapLoad(SystemManager* pManager, char* path, EGG::Heap* heap, bool allocTop, u32* fsizeOutput)
{
	DVDFileInfo fInfo;
	if (!DVDOpen(path, &fInfo))
	{
		path = BootStrapFilePaths[1]; // Default to English
		if (!DVDOpen(path, &fInfo))
		{
			path = BootStrapFilePaths[0]; // Japan
			if (!DVDOpen(path, &fInfo))
			{
				// Fatal: file doesn't exist
				GXColor bg = { 0x00, 0x00, 0x00, 0xff };
				GXColor fg = { 0xff, 0xff, 0xff, 0xff };
				OSFatal(fg, bg, "Failed to load bootstrap file!\n");
				return 0;
			}
		}
	}
	DVDClose(&fInfo);
	return pManager->ripFromDisc(path, heap, allocTop, fsizeOutput);
}
// Overwrite load to game table
PokeyCall(0x80007500, getBootStrapFilePaths);

// Overwrite call to rip file
PokeyCall(0x80007528, bootStrapLoad);
} // namespace Bootstrap