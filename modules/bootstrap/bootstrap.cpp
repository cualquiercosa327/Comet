#include "bootstrap.hpp"
#include <libpokey/hooks.h>
#include <EGG/core/eggHeap.hpp>
#include <revolution/dvd.h>
#include <Static/System/SystemManager.hpp>

namespace Bootstrap {
// filepaths from all regions combined
static const char* BootStrapFilePaths[] = {
	"/Boot/Strap/jp/jp.szs", // 0
	"/Boot/Strap/eu/English.szs", // 1
	"/Boot/Strap/eu/German.szs", // 2
	"/Boot/Strap/eu/French.szs", // 3
	// "/Boot/Strap/eu/Spanish_US.szs"
	"/Boot/Strap/eu/Spanish_EU.szs", // 4
	"/Boot/Strap/eu/Italian.szs", // 5
	"/Boot/Strap/eu/Dutch.szs" // 6
};

const char** getBootStrapFilePaths()
{
	return BootStrapFilePaths;
}

u8* bootStrapLoad(void* pManager, char* path, EGG::Heap* heap, bool allocTop, u32* fsizeOutput)
{
	DVDFileInfo fInfo;


	if (DVDOpen(path, &fInfo))
		goto out;

	if (!strcmp(path, BootStrapFilePaths[4]))
	{
		path = "/Boot/Strap/eu/Spanish_US.szs";
		if (DVDOpen(path, &fInfo))
			goto out;
	}

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
	
out:
	DVDClose(&fInfo);
	return pManager->ripFromDisc(path, heap, allocTop, fsizeOutput);
}
// Overwrite load to game table
PokeyCall(0x80007500, getBootStrapFilePaths);

// Overwrite call to rip file
PokeyCall(0x80007528, bootStrapLoad);
} // namespace Bootstrap