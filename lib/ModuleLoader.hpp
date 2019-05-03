#pragma once

#include <lib/IModule.hpp>
#include <vector>
#include <libpokey/debug.h>
};

class ModuleLoader
{
public:
	ModuleLoader();
	~ModuleLoader();

	void loadModules();
	void unloadModules();
	void reloadModules();

	template<class M>
	void appendNewModule()
	{
		mModules.push_back(new M());
	}

private:
	std::vector<ModuleAccessor> mModules;
};