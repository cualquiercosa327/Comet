#pragma once

#include <libpokey/debug.h>
#include <lib/IModule.hpp>

class ModuleAccessor
{
public:
	inline ModuleAccessor(IModule* pModule)
		: mpModule(pModule), bLoaded(false)
	{}
	inline ModuleAccessor()
	{}

	inline ~ModuleAccessor()
	{
		if (bLoaded)
			mpModule->unload();
	}

	// todo: might want to return result
	void load();
	void unload();

	inline bool isLoaded() { return bLoaded; }

public:
	IModule* mpModule;
	bool bLoaded;
};