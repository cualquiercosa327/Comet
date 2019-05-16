#pragma once

struct chain
{
	chain* last;
	void* dtor;
	void* obj;
};

__declspec(weak) void** __ctor_loc;
__declspec(weak) void** __ctor_end;


namespace GlobalsManager {

	void destroyGlobals();
	void resetGlobalDestructorChain();
	void createGlobals();


}; // namespace GlobalsManager

extern "C" void __register_global_object(void* obj, void* dtor, chain* newchain);