#pragma once

#include <Common/rk_types.h>


namespace Prologue {

//! @brief apply the patches
void applyPatches(u32* block);

} // namespace Prologue

struct chain
{
	chain* last;
	void* dtor;
	void* obj;
};

__declspec(weak) void** __ctor_loc;
__declspec(weak) void** __ctor_end;
void destroyGlobals();