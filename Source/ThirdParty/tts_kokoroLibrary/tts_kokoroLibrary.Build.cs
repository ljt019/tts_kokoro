// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class tts_kokoroLibrary : ModuleRules
{
	public tts_kokoroLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		PublicSystemIncludePaths.Add("$(ModuleDir)/Public");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// import static files
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "cargs.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "sherpa-onnx-c-api.lib"));

			// Delay-load the DLL
			PublicDelayLoadDLLs.Add("cargs.dll");
			PublicDelayLoadDLLs.Add("espeak-ng.dll");
			PublicDelayLoadDLLs.Add("onnxruntime.dll");
			PublicDelayLoadDLLs.Add("onnxruntime_providers_shared.dll");
			PublicDelayLoadDLLs.Add("piper_phonemize.dll");
			PublicDelayLoadDLLs.Add("sherpa-onnx-c-api.dll");
			PublicDelayLoadDLLs.Add("sherpa-onnx-core.dll");
			PublicDelayLoadDLLs.Add("ucd.dll");

			// Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/tts_kokoroLibrary/Win64/cargs.dll");
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/tts_kokoroLibrary/Win64/espeak-ng.dll");
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/tts_kokoroLibrary/Win64/onnxruntime.dll");
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/tts_kokoroLibrary/Win64/onnxruntime_providers_shared.dll");
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/tts_kokoroLibrary/Win64/piper_phonemize.dll");
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/tts_kokoroLibrary/Win64/sherpa-onnx-c-api.dll");
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/tts_kokoroLibrary/Win64/sherpa-onnx-core.dll");
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/tts_kokoroLibrary/Win64/ucd.dll");
			
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
	
		}
	}
}
