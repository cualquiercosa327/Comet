#include "universalizer.hpp"
#include <libpokey/hooks.h>
#include <libpokey/debug.h>

#include <revolution/dvd.h>

// This alone will allow JPN to work.
// Additional code is needed for NTSC-U/PAL region overlap (e.g., _E and _U)
//	const char* loaderRegionSuffixes[] = {
//		"_J.szs",	// Japanese
//		"_E.szs",	// English (NTSC-U: _U)
//		"_G.szs",	// German
//		"_F.szs",	// French (NTSC-U: _Q)
//		"_S.szs",	// Spanish (NTSC-U: _M)
//		"_I.szs",	// Italian
//		"_N.szs"	// Dutch
//	};
const char* jpn_suffix = "_J.szs";

PokeyWritePointer(0x808B3188, &jpn_suffix);

const char* eng_us_suffix = "_U.szs";
const char* french_qu_suffix = "_Q.szs";
const char* spanish_mx_suffix = "_M.szs";

// Really nasty hack to switch to US mode if one file is detected.
// This isn't actually universal


inline void Set_NTSCU_Region_Suffixes()
{
	DebugReport("Setting NTSC-U region suffixeds..\n");
	*(u32*)0x808B318C = *(u32*)&eng_us_suffix;
	*(u32*)0x808B3194 = *(u32*)&french_qu_suffix;
	*(u32*)0x808B3198 = *(u32*)&spanish_mx_suffix;
}

// We can't rely on the disc ID
inline bool isNTSCU()
{
	return DVDConvertPathToEntrynum("/Scene/UI/MenuSingle_U.szs") != -1;
}

void NTSCU_Region_Suffix_Process()
{
	if (isNTSCU())
		Set_NTSCU_Region_Suffixes();
}


// Credit: zak
// Allows concurrent save data
PokeyWrite32(0x80544928, 0x7C601B78);
