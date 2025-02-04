// tts_kokoroBPLibrary.cpp

#include "tts_kokoroBPLibrary.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Sound/SoundWaveProcedural.h"
#include "CoreMinimal.h"
#include "Logging/LogMacros.h"
#include "Math/UnrealMathUtility.h"
#include "tts_kokoroLibrary/sherpa-onnx/c-api/c-api.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "CoreMinimal.h"
#include "Logging/LogMacros.h"
#include "Math/UnrealMathUtility.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"

/**
 * A simple progress callback for sherpa-onnx that logs progress.
 */
static int32_t ProgressCallback(const float* /*samples*/, int32_t /*num_samples*/, float progress)
{
	UE_LOG(LogTemp, Log, TEXT("TTS Progress: %.3f%%"), progress * 100);
	// Returning 1 indicates to continue processing.
	return 1;
}

USoundWave* Utts_kokoroBPLibrary::GenerateTtsAudio(const FString& InText)
{
	// *** Configure the sherpa-onnx TTS engine ***
	SherpaOnnxOfflineTtsConfig config;
	FMemory::Memzero(&config, sizeof(config));

	// NOTE: You will need to adjust these paths to point to your model files.
	// In this example we assume the files are placed in a known folder (e.g., inside your project or plugin).
	FString BaseDir = FPaths::ProjectDir();  // Or get your plugin's base dir if appropriate
	FString ModelPath = FPaths::Combine(*BaseDir, TEXT("TTSAssets/kokoro-en-v0_19/model.onnx"));
	FString VoicesPath = FPaths::Combine(*BaseDir, TEXT("TTSAssets/kokoro-en-v0_19/voices.bin"));
	FString TokensPath = FPaths::Combine(*BaseDir, TEXT("TTSAssets/kokoro-en-v0_19/tokens.txt"));
	FString DataDir = FPaths::Combine(*BaseDir, TEXT("TTSAssets/kokoro-en-v0_19/espeak-ng-data"));

	// Convert Unreal strings to UTF-8 C strings
	config.model.kokoro.model = TCHAR_TO_UTF8(*ModelPath);
	config.model.kokoro.voices = TCHAR_TO_UTF8(*VoicesPath);
	config.model.kokoro.tokens = TCHAR_TO_UTF8(*TokensPath);
	config.model.kokoro.data_dir = TCHAR_TO_UTF8(*DataDir);

	config.model.num_threads = 2;
	config.model.debug = 1;  // set to 0 to disable debug messages

	// *** Run TTS Generation ***
	const char* inputText = TCHAR_TO_UTF8(*InText);
	int32_t sid = 0;       // Speaker ID (adjust as needed)
	float speed = 1.0f;    // Speech speed (adjust as needed)

	// Create the TTS instance
	const SherpaOnnxOfflineTts* tts = SherpaOnnxCreateOfflineTts(&config);
	if (!tts)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create sherpa-onnx TTS instance."));
		return nullptr;
	}

	// Generate audio with progress callback
	const SherpaOnnxGeneratedAudio* audio = SherpaOnnxOfflineTtsGenerateWithProgressCallback(tts, inputText, sid, speed, ProgressCallback);
	if (!audio)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate audio from text."));
		SherpaOnnxDestroyOfflineTts(tts);
		return nullptr;
	}

	// *** Convert the Generated Audio ***
	// Here we assume that audio->samples is an array of float samples in the range [-1.0, 1.0].
	// We convert these to 16-bit PCM data.
	int32 SampleCount = audio->n;
	TArray<uint8> RawPCMData;
	RawPCMData.AddUninitialized(SampleCount * sizeof(int16));
	int16* PCMData = reinterpret_cast<int16*>(RawPCMData.GetData());

	for (int32 i = 0; i < SampleCount; i++)
	{
		// Convert float sample to int16 sample
		float Sample = audio->samples[i];
		int16 PCM = FMath::Clamp<int32>(FMath::RoundToInt(Sample * 32767.f), -32768, 32767);
		PCMData[i] = PCM;
	}

	// *** Create a USoundWave Asset ***
	// There are several ways to create a playable sound asset from raw PCM data in Unreal.
	// One common method is to create a USoundWaveProcedural that streams the PCM data.
	// For this example we create one and “prime” it with our generated PCM buffer.
	USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
	if (!SoundWave)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create USoundWaveProcedural."));
		SherpaOnnxDestroyOfflineTtsGeneratedAudio(audio);
		SherpaOnnxDestroyOfflineTts(tts);
		return nullptr;
	}

	// Set sound wave properties.
	SoundWave->SetSampleRate(audio->sample_rate);
	SoundWave->NumChannels = 1;  // Assuming mono audio (adjust if stereo)
	SoundWave->Duration = (float)SampleCount / audio->sample_rate;
	SoundWave->bLooping = false;

	// IMPORTANT: USoundWaveProcedural does not automatically “store” PCM data.
	// You must override its GeneratePCMData (or queue the data) so that when played it
	// returns your generated samples. For a simple one-shot audio asset, you might consider
	// writing a custom USoundWave subclass that holds an internal buffer.
	//
	// For the purpose of this example, we assume the generated data is short and you will
	// extend this code to feed the data on demand (for example, via a custom sound wave class).
	//
	// As a workaround for testing, you might write the RawPCMData to a temporary file and then
	// import it using existing Unreal functionality. However, that is beyond this example’s scope.

	// Clean up sherpa-onnx resources.
	SherpaOnnxDestroyOfflineTtsGeneratedAudio(audio);
	SherpaOnnxDestroyOfflineTts(tts);

	// Return the (incomplete) SoundWave asset.
	// You will need to implement a mechanism to stream RawPCMData when the sound plays.
	return SoundWave;
}
