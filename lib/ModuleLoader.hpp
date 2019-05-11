#pragma once

#include <lib/ModuleAccessor.hpp>
#include <vector.h>
#include <libpokey/debug.h>


class ModuleLoader
{
public:
	ModuleLoader();
	~ModuleLoader();

	void loadModules();
	void unloadModules();
	void reloadModules();

	// Dispatch module frame hooks
	void tick();

	template<class M>
	void appendNewModule()
	{
		mModules.push_back(new M());
	}

private:
	std::vector<ModuleAccessor> mModules;
};