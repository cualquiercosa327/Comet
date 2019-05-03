#pragma once

class IModule
{
public:
	enum eResult
	{
		RESULT_OK
	};
	virtual ~IModule() = 0;

	virtual eResult load() = 0;
	virtual eResult unload() = 0;

	virtual const char* getModuleName() = 0;
	virtual const char* getModuleVersion() = 0;

	// Whether or not to halt if this module fails to load.
	virtual bool getIsCriticalModule() = 0;
};
