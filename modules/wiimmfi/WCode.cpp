#include "WCode.hpp"

#include <libpokey/debug.h>
#include <libpokey/hooks.h>

#include <string.h>
#include <revolution/os.h>


static const char patcherInfo[] = "Comet 1.0                                 \n";


namespace Wiimmfi {

static const char*   WiimmfiDomainString	= "wiimmfi.de";
static const int	 WiimmfiDomainLength	= 10;

//! PatchDomain and PatchWiimmfi are based on domainpatcher and PrivateServerPatcher from https://sourceforge.net/p/usbloadergx/code/HEAD/tree/trunk/source/patches/gamepatches.c
//! Key differences:
//! - Deobfuscated and fully documented
//! - Syntax and naming updated to Comet standards
//! - Sanity checking and code optimization
static struct WiimmfiManager
{
	
	//! @brief TODO
	static void PatchDomain(void* addr, u32 len)
	{
		char* cur = (char*)addr;
		const char* end = cur + len - 16;

		do if (!memcmp(cur, "nintendowifi.net", 16))
		{
			DebugReport("\tPatching \"nintendowifi.net\" instance @ %p\n", cur);
			int len = strlen(cur);
			u8 i;
			memcpy(cur, WiimmfiDomainString, WiimmfiDomainLength);
			memmove(cur + WiimmfiDomainLength, cur + 16, len - 16);
			for (i = 16 - WiimmfiDomainLength; i > 0; i--)
				cur[len - i] = 0;
			cur += len;
		}
		while (++cur < end);
	}
	//! @brief TODO
	static void PatchWiimmfi(void* addr, u32 len)
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

	WiimmfiManager()
	{
		// Write patcher info
		strcpy((char*)0x80276054, patcherInfo);
		// Based on https://sourceforge.net/p/usbloadergx/code/1271/
		strcpy((char*)0x8027A400, "http://ca.nas.wiimmfi.de/ca");
		strcpy((char*)0x8027A400 + 0x28, "http://naswii.wiimmfi.de/ac");
		strcpy((char*)0x8027A400 + 0x4C, "https://main.nas.wiimmfi.de/pp");

		DebugReport("Searching for strings to patch..\n");
		u32 clock = OSGetTick();
		// Do the plain old patching with the string search
		PatchWiimmfi((void*)0x80004000, 0x385200);
		clock = OSGetTick() - clock;
		DebugReport("..Done. (%ums)\n", OSTicksToMilliseconds(clock));
	}

} sWiimmfiManager;


// Nop "Host" header
PokeyWrite32(0x800ed868, 0x60000000);

// 's://' -- todo: understand
PokeyWrite32(0x802a146cl, 0x733a2f2f);


// Force dev server when DWC initializes
PokeyWrite32(0x800ECAAC, 0x3BC00000);

PokeyInsertDefAsm(0x800EE3A0)
{
	cmpwi r3, 0;
	ble- exit;

	// Check authsvr
	lwz r3, 12(r1);
	lwz r0, -26824(r13);
	cmpwi r0, 2;
	beq- call_payload;

	// If not ready, store payload addr to SSL l_initialized BOOL (4B)
	stw r3, -24244(r13);

	// mark as ready for next time and exit
	li r0, 2;
	stw r0, -26824(r13);
	b exit;

call_payload:
	// Round to 4B
	addi r4, r3, 3;
	rlwinm r4, r4, 0, 0, 29;
	// Load prologue offset
	lbz r5, 0(r4);
	add r5, r4, r5;
	// Clear prologue cache block (32B)
	dcbf r0, r5;
	// Call prologue
	mtlr r5;
	blrl;

exit:
	// setup cr0
	li r3, -1;
	cmpwi r3, 0;
	blr; // patched
}
static u32 precomp_hash[] = {
0, // pad (branch instruction, necessary for lwzu r5, 4(r3) to load first value
0x0FFF1F07, 0x00E638C9, 0x49FBEFFA, 0x79022D3A, 0x84AB134F
};


PokeyInsertDefAsm(0x801D4EFC)
{
	// Load l_initialized. Check if it's simply TRUE (1) or set to payload pointer.
	lwz r4, -24244(r13);
	cmplwi r4, 1;
	ble- return_null;

	// Setup stack
	stwu r1, -128(r1);
	
	// Initialize SHA-1 hash
	addi r3, r1, 32;
	lis r0, -32739;
	ori r0, r0, 9460;
	mtlr r0;
	blrl;

	// Hash the first 1364 bytes of the payload
	addi r3, r1, 32;
	lwz r4, -24244(r13);
	li r5, 1364;
	stw r5, 196(r28);
	lis r0, -32739;
	ori r0, r0, 9540;
	mtlr r0;
	blrl;

	// Get the digest
	addi r3, r1, 32;
	addi r4, r1, 16;
	lis r0, -32739;
	ori r0, r0, 9720;
	mtlr r0;
	blrl;
// Unfortunately CW's assembler does not support raw data directives!
#if 0
	// Get a pointer to the precomputed hash
	b hash_end;
		.word 0x0FFF1F07
		.word 0x00E638C9
		.word 0x49FBEFFA
		.word 0x79022D3A
		.word 0x84AB134F
	hash_end:
	mflr r3;
#else
	lis r3, precomp_hash@h;
	ori r3, r3, precomp_hash@l;
#endif
	// Set r4 to point to the context digest
	addi r4, r1, 12;
	// Verify the hash
	li r5, 5; // (160 bits = 5 * 32)
	mtctr r5;
hash_compare:
	lwzu r5, 4(r3);
	lwzu r6, 4(r4);
	cmpw r6, r5;
	bne- hash_compare_exit; // immediately exit if different
	bdnz+ hash_compare;

hash_compare_exit:
	cmpw r6, r5; // Were the last two equal (good)?
	addi r1, r1, 128; // Restore stack
	lwz r4, -24244(r13); // Load pointer to payload
	beq- return; // If payload hash matches, return it.
return_null:
	li r4, 0; // Otherwise, return NULL.
return:
	blr; // patched
}
} // namespace Wiimmfi