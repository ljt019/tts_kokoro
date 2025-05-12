// Copyright Epic Games, Inc. All Rights Reserved.

#include "tts_kokoro.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

// Include the header for the ExampleLibrary functions
#include "tts_kokoroLibrary/ExampleLibrary.h"

#define LOCTEXT_NAMESPACE "Ftts_kokoroModule"

void Ftts_kokoroModule::StartupModule()
{
	// This code executes when your module is loaded into memory.
	// Get the base directory of this plugin.
	FString BaseDir = IPluginManager::Get().FindPlugin("tts_kokoro")->GetBaseDir();

	// Load the ExampleLibrary.dll (used for testing)
	FString ExampleLibraryPath;
#if PLATFORM_WINDOWS
	ExampleLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/tts_kokoroLibrary/Win64/ExampleLibrary.dll"));
#elif PLATFORM_MAC
	ExampleLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/tts_kokoroLibrary/Mac/Release/libExampleLibrary.dylib"));
#elif PLATFORM_LINUX
	ExampleLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/tts_kokoroLibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
#endif

	ExampleLibraryHandle = !ExampleLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*ExampleLibraryPath) : nullptr;
	if (ExampleLibraryHandle)
	{
		// Call a test function from ExampleLibrary.
		ExampleLibraryFunction();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load ExampleLibrary.dll"));
	}

	// Now load the additional DLL needed for your Blueprint functions.
	// Here we assume sherpa-onnx-c-api.dll is used by your Blueprint function library.
	FString SherpaDllPath;
#if PLATFORM_WINDOWS
	SherpaDllPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/tts_kokoroLibrary/Win64/sherpa-onnx-c-api.dll"));
#elif PLATFORM_MAC
	SherpaDllPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/tts_kokoroLibrary/Mac/Release/libsherpa-onnx-c-api.dylib"));
#elif PLATFORM_LINUX
	SherpaDllPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/tts_kokoroLibrary/Linux/x86_64-unknown-linux-gnu/libsherpa-onnx-c-api.so"));
#endif

	SherpaDllHandle = !SherpaDllPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*SherpaDllPath) : nullptr;
	if (!SherpaDllHandle)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SherpaDllError", "Failed to load sherpa-onnx-c-api DLL"));
	}
}

void Ftts_kokoroModule::ShutdownModule()
{
	// Free the DLL handles during shutdown.
	if (ExampleLibraryHandle)
	{
		FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
		ExampleLibraryHandle = nullptr;
	}

	if (SherpaDllHandle)
	{
		FPlatformProcess::FreeDllHandle(SherpaDllHandle);
		SherpaDllHandle = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(Ftts_kokoroModule, tts_kokoro)
