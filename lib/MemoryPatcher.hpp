#pragma once

#include <libpokey/debug.h>
#include <vector>

class MemoryPatcher
{
public:
	template <typename T>
	struct Patch
	{
		void* addr;
		T     val;

		inline Patch()
		{}
		inline Patch(void* dst, T src)
			: addr(dst), val(src)
		{}
	};
	typedef Patch<u32> Patch32;

	MemoryPatcher();
	~MemoryPatcher();

	void processPatchFile(u32* block);
	void revertPatches();
	void processDiscPatchFile();
	static void patch(Patch32* patch);

	void scanForChanges();


private:

	struct PatchRecord : public Patch32
	{
		u32 newVal;

		inline PatchRecord() : Patch() {}
		inline PatchRecord(void* addr, u32 o, u32 n)
			: Patch32(addr, o), newVal(n)
		{}
	};

	std::vector<PatchRecord> mPatchRecord;
};