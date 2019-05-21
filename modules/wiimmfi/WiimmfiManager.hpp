#pragma once

#include <libpokey/debug.h>
#include <libpokey/hooks.h>

namespace Wiimmfi {

//! PatchDomain and PatchWiimmfi are based on domainpatcher and PrivateServerPatcher from https://sourceforge.net/p/usbloadergx/code/HEAD/tree/trunk/source/patches/gamepatches.c
//! Key differences:
//! - Deobfuscated and fully documented
//! - Syntax and naming updated to Comet standards
//! - Sanity checking and code optimization
class WiimmfiManager
{
public:
	//! @brief TODO
	static void PatchDomain(void* addr, u32 len);
	//! @brief TODO
	static void PatchWiimmfi(void* addr, u32 len);

	WiimmfiManager();

private:
	static WiimmfiManager sInstance;

public:
	static const WiimmfiManager& getInstance() { return sInstance; }
};

} // namespace Wiimmfi