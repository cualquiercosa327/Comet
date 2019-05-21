#include "globalsManager.hpp"
#include <libpokey/debug.h>

namespace GlobalsManager {

DestructorChainElement* __global_destructor_chain;


void destroyGlobals()
{
	for (DestructorChainElement* it = __global_destructor_chain; it && OSIsMEM1Region(it); it = it->last)
	{
		PokeyDebugReport("Destroying global %p\n", it->obj);
		PokeyDebugReport("DT: %p\n", it->dtor);
		((void(*)(void*, int))it->dtor)(it->obj, -1);
	}
}


void resetGlobalDestructorChain()
{
	__global_destructor_chain = 0;
}

void createGlobals()
{
	int q = 0;
	for (void*** f = &__ctor_loc; f < &__ctor_end; f++)
	{
		PokeyDebugReport("Loading __sinit #%u; ctor: %p\n", q++, *f);

		((void(*)(void))*f)();
	}
}

}; // namespace GlobalsManager

extern "C" void __register_global_object(void* obj, void* dtor, DestructorChainElement* newchain)
{
	PokeyDebugReport("Registering global object: obj=%p, dtor=%p, dtor chain=%p\n", obj, dtor, newchain);

	newchain->last = GlobalsManager::__global_destructor_chain;
	newchain->dtor = dtor;
	newchain->obj = obj;
	GlobalsManager::__global_destructor_chain = newchain;
}
