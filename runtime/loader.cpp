#define DEBUG

#include <runtime/bootstrap.hpp>
#include <libpokey/hooks.h>
#include <libpokey/mkw/environment.h>



inline
EGG::Heap* getMem1Heap()
{
	__asm {
		lwz r3, -0x6C38(r13);
		lwz r3, 0x18(r3);
	}
}
kmCallDefCpp(0x80007B5C, bool, System::ModuleLinker* linker, int moduleID)
{
	u8* block;
	// Before we call the module, let's add our code
	EGG::Heap* pHeap = getMem1Heap();

	if (pHeap)
	{
		DebugReport("Loaded heap!\n");
	}
	else
	{
		DebugReport("Could not acquire MEM1 heap!");
		goto out;
	}
	block = EGG::DvdRipper::loadToMainRAM(PATH_CODE_BIN, NULL, pHeap, EGG::DvdRipper::ALLOC_DIR_TOP, 0, NULL, NULL);
	if (block)
	{
		DebugReport("..ripped file!\n");
	}
	else
	{
		DebugReport("Couldn't rip file!");
		goto out;
	}
		
	if ((u32)block != 0x809c4fa0)
	{
		DebugReport("Block allocated at unexpected location (%p)!\n", block);
		pHeap->free(block);
	}
	else
	{
		DebugReport("Block successfully allocated at expected location!\n");
		((u32(*)(void))block)();
	}

out:
	// Call down to the original module
	return linker->callModule(moduleID);
}