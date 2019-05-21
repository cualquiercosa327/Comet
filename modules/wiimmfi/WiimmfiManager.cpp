#include "WiimmfiManager.hpp"

#include <string.h>

// Configuration
#define COMET_WIMMFI_ANNOUNCE
#define COMET_WSTRT_ANNOUNCE

namespace Wiimmfi {

// Static instance
WiimmfiManager WiimmfiManager::sInstance;

// Patch constants
static const char*   WiimmfiDomainString = "wiimmfi.de";
static const int	 WiimmfiDomainLength = 10;

#ifdef COMET_WIMMFI_ANNOUNCE
	#if defined(COMET_WSTRT_ANNOUNCE) // for debug ram dump comparison
		static const char patcherInfo[] = "wstrt v1.62d r7634                        \n";
	#else
		static const char patcherInfo[] = "Comet 1.0                                 \n";
	#endif
#endif

void WiimmfiManager::PatchDomain(void* addr, u32 len)
{
	char* cur = (char*)addr;
	const char* end = cur + len - 16;

	do if (!memcmp(cur, "nintendowifi.net", 16))
	{
		DebugReport("\tPatching \"nintendowifi.net\" instance @ %p\n", cur);
		int len = strlen(cur);

		memcpy(cur, WiimmfiDomainString, WiimmfiDomainLength);
		memmove(cur + WiimmfiDomainLength, cur + 16, len - 16);

		for (int i = 16 - WiimmfiDomainLength; i; i--)
			cur[len - i] = 0;

		cur += len;
	}
	while (++cur < end);
}

void WiimmfiManager::PatchWiimmfi(void* addr, u32 len)
{
	// Patch protocol https -> http
	char *cur = (char *)addr;
	const char *end = cur + len - 8;

	do if (!memcmp(cur, "https://", 8) && cur[8])
	{
		int len = strlen(cur);
		memmove(cur + 4, cur + 5, len - 5);
		cur[len - 1] = 0;
		cur += len;
	} while (++cur < end);

	// Patch nintendowifi.net -> private server domain
	PatchDomain(addr, len);
}

WiimmfiManager::WiimmfiManager()
{
	// Write patcher info
#ifdef COMET_WIMMFI_ANNOUNCE
	strcpy((char*)0x80276054, patcherInfo);
#endif

	// String search
	DebugReport("Searching for strings to patch..\n");
	u32 clock = OSGetTick();
	PatchWiimmfi((void*)0x80004000, 0x385200);
	clock = OSGetTick() - clock;
	DebugReport("..Done. (%ums)\n", OSTicksToMilliseconds(clock));

	// SSL
	strcpy((char*)0x8027A400, "http://ca.nas.wiimmfi.de/ca");
	strcpy((char*)0x8027A400 + 0x28, "http://naswii.wiimmfi.de/ac");
	strcpy((char*)0x8027A400 + 0x4C, "https://main.nas.wiimmfi.de/pp");
}

} // namespace Wiimmfi