#pragma once

struct DestructorChainElement
{
	DestructorChainElement* last;
	void* dtor;
	void* obj;
};

__declspec(weak) void** __ctor_loc;
__declspec(weak) void** __ctor_end;


namespace GlobalsManager {

	//! @brief Traverse the destructor chain, destroying all global objects.
	//!
	void destroyGlobals();
	
	//! @brief Reset the global destructor chain. Only the pointer to the head is modified!
	//!
	void resetGlobalDestructorChain();
	
	//! @brief Iterate through the global constructors array, creating all global objects.
	//!
	void createGlobals();


}; // namespace GlobalsManager

//! @brief Custom implementation of the CW runtime function for Comet. (Necessary for comet components to be fully contained.)
//!
//! @param[in] obj		Pointer to the global object.
//! @param[in] dtor		Pointer to the object's destructor.
//! @param[in] newchain Pointer to a new tweleve byte chain link to become the new head.
//!
extern "C" void __register_global_object(void* obj, void* dtor, DestructorChainElement* newchain);