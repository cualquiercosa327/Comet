#pragma once

#include <libpokey/debug.h>
#include <vector.h>

class MemoryPatcher
{
public:
	struct Patch32
	{
		void* addr;
		u32 val;
	};

	MemoryPatcher();
	~MemoryPatcher();

	void processPatchFile(u32* block);
	void revertPatches();
	void processDiscPatchFile();
	static void patch(Patch32* patch);



private:

	struct PatchRecord
	{
		// important that it starts same as Patch32
		void* address;
		u32 oldVal;
		u32 newVal;

		inline PatchRecord() {}
		inline PatchRecord(void* addr, u32 o, u32 n)
			: address(addr), oldVal(o), newVal(n)
		{}
	};

	std::vector<PatchRecord> mPatchRecord;
};