// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "TTSAsyncAction.h"

class Ftts_kokoroModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the test DLL (ExampleLibrary) */
	void *ExampleLibraryHandle = nullptr;

	/** Handle to the DLL needed for Blueprint functions (sherpa-onnx-c-api) */
	void *SherpaDllHandle = nullptr;
};