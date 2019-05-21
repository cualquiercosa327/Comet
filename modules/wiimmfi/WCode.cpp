#define WCODE_CPP_INCLUDED_FROM_SOURCE
#include "WCode.hpp"

#include <libpokey/debug.h>
#include <libpokey/hooks.h>



namespace Wiimmfi {

// Types


//! @brief The payload called by the loader code.
//!
//! @param[in] pBlock  Pointer to the start of the payload. (unaligned)
//! @param[in] p4Block [Unused] Pointer to the start of the payload block 4B aligned (+2 pBlock)
//! @param[in] pProl   Pointer to the prologue (this function)
//!
typedef void(*WC_Prologue)(void* pBlock, void* p4Block, void* pProl);

// Standalone patches

// Nop "Host" header
PokeyWrite32(0x800ed868, 0x60000000);

// 's://' -- todo: don't think needed still
PokeyWrite32(0x802a146cl, 0x733a2f2f);


// Force dev server when DWC initializes
PokeyWrite32(0x800ECAAC, 0x3BC00000);


// Complex patches

#ifdef COMET_WCODE_REIMPLEMENTATION

void WCode_LoadPayload(int a, void* payload)
{
	DebugReport("WCode_LoadPayload was called(%i, %p)!\n", a, payload);

	if (a <= 0)
	{
		DebugReport("> Early exit\n");
		return;
	}
	
	if (DWC::sAuthServer != DWC::AuthSvr::PROHIBITED)
	{
		DebugReport("> Getting things ready for next time..\n");
		// Store payload
		SSL::l_initialized = (int)payload;
		// Mark as ready next time
		DWC::sAuthServer = DWC::AuthSvr::PROHIBITED;
	}
	else
	{	
		// Round up our payload (+2)
		void* payloadRounded = (void*)RoundUpTo(payload, 4); // r4
			
		// Derive our prologue pointer
		WC_Prologue prologue = (WC_Prologue)((u8*)payloadRounded + *(u8*)payloadRounded); // r5
			
		// Clear data cache
		dcbf(prologue);

		int alignTrans; // Debug only

		DebugReport("Launching WC Payload.\n\tUnaligned block pointer: %p\n\tAligned (4B) block pointer: %p (%c%u)\n\tPrologue offset: %u\n\tPrologue pointer: %p\n",
			payload,
			( alignTrans = (int)payloadRounded-(int)payload ) >= 0 ? '+' : '-' , // Explicit sign
			abs(alignTrans),
			*(u8*)payloadRounded,
			prologue
		);

		// Call our prologue
		prologue(payload, payloadRounded, prologue);

		DebugReport("..Wiimmfi code changes injected!\n");
	}
	

}

PokeyCallDefAsm(0x800EE3A0)
{
	lwz r4, 12(r1);
	bl WCode_LoadPayload;
	// setup cr0
	li r3, -1;
	cmpwi r3, 0;
	// blr;
}
#else

// Reverse engineered version for reference
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
	// Clear prologue cache block
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
#endif


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