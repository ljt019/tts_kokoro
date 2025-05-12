// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class tts_kokoro : ModuleRules
{
    public tts_kokoro(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
			}
        );

        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Engine",
                "tts_kokoroLibrary",
                "Projects",
                "AudioMixer",
                "AudioMixerCore",
                "AudioExtensions"
				// ... add other public dependencies that you statically link with here ...
			}
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
        );

        // Set up paths for the third party DLLs. Adjust the path if necessary.
        string PluginBaseDir = ModuleDirectory;
        // Assuming your third party binaries are located in Plugins/tts_kokoro/Binaries/ThirdParty/tts_kokoroLibrary/Win64
        string ThirdPartyPath = Path.Combine(PluginBaseDir, ".", "Binaries", "ThirdParty", "tts_kokoroLibrary", "Win64");

        // List of all DLLs you need to deploy.
        string[] DllNames = new string[]
        {
            "ucd.dll",
            "cargs.dll",
            "espeak-ng.dll",
            "ExampleLibrary.dll",
            "onnxruntime.dll",
            "onnxruntime_providers_shared.dll",
            "piper_phonemize.dll",
            "sherpa-onnx-c-api.dll",
            "sherpa-onnx-core.dll"
        };

        foreach (string dll in DllNames)
        {
            // Directly add the DLL as a runtime dependency.
            string DllPath = Path.Combine(ThirdPartyPath, dll);
            RuntimeDependencies.Add(DllPath);
        }

    }
}
