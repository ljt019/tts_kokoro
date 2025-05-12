#include "TTSAsyncAction.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/PlatformProcess.h"

// Include the TTS library's header
#include "tts_kokoroLibrary/c-api.h"

UTTSAsyncAction* UTTSAsyncAction::GenerateTTSSoundWaveAsync(
    UObject* WorldContextObject,
    const FString& TextToSpeak)
{
  UTTSAsyncAction* Node = NewObject<UTTSAsyncAction>();
  Node->WorldContextObject = WorldContextObject;
  Node->TextToSpeak = TextToSpeak;
  return Node;
}

int32 UTTSAsyncAction::ProgressCallback(const float* /*samples*/, int32 /*num_samples*/, float progress)
{
    UE_LOG(LogTemp, Log, TEXT("TTS Async Progress: %.3f%%"), progress * 100);
    return 1; // Continue processing
}

void UTTSAsyncAction::Activate()
{
  // Kick off the heavy work on a background thread:
  Async(EAsyncExecution::ThreadPool, [this]()
  {
    TArray<uint8> PCM;
    int32 SR = 0, CH = 0;
    bool bOk = GeneratePCMData(PCM, SR, CH);

    // Back to game thread to build the SoundWave and fire delegates:
    AsyncTask(ENamedThreads::GameThread, [this, PCM = MoveTemp(PCM),
                                         SR, CH, bOk]()
    {
      if (!bOk)
      {
        OnFailure.Broadcast();
      }
      else
      {
        UMySoundWave* SW = NewObject<UMySoundWave>(WorldContextObject);
        SW->Init(PCM, SR, CH);
        OnSuccess.Broadcast(SW);
      }
      // Let Blueprint garbage‐collect this node:
      this->SetReadyToDestroy();
    });
  });
}

bool UTTSAsyncAction::GeneratePCMData(TArray<uint8>& OutPCM,
                                      int32& OutSampleRate,
                                      int32& OutNumChannels)
{
    // Get the plugin base directory
    TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("tts_kokoro"));
    if (!Plugin.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find tts_kokoro plugin."));
        return false;
    }
    FString PluginDir = Plugin->GetBaseDir();

    // Convert PluginDir to an absolute path
    PluginDir = FPaths::ConvertRelativePathToFull(PluginDir);
    UE_LOG(LogTemp, Log, TEXT("Plugin Base Directory (absolute): %s"), *PluginDir);

    // Build the asset directory path (assets located in ./TTSAssets/kokoro-en-v0_19 relative to the plugin)
    FString AssetsDir = FPaths::Combine(PluginDir, TEXT("TTSAssets/kokoro-en-v0_19"));

    // Build individual asset paths
    FString ModelPath = FPaths::Combine(AssetsDir, TEXT("model.onnx"));
    FString VoicesPath = FPaths::Combine(AssetsDir, TEXT("voices.bin"));
    FString TokensPath = FPaths::Combine(AssetsDir, TEXT("tokens.txt"));
    FString DataDir = FPaths::Combine(AssetsDir, TEXT("espeak-ng-data"));

    UE_LOG(LogTemp, Log, TEXT("Model Path: %s"), *ModelPath);
    UE_LOG(LogTemp, Log, TEXT("Voices Path: %s"), *VoicesPath);
    UE_LOG(LogTemp, Log, TEXT("Tokens Path: %s"), *TokensPath);
    UE_LOG(LogTemp, Log, TEXT("Data Directory: %s"), *DataDir);

    // Create persistent UTF-8 strings from the Unreal strings.
    std::string ModelPathUtf8 = TCHAR_TO_UTF8(*ModelPath);
    std::string VoicesPathUtf8 = TCHAR_TO_UTF8(*VoicesPath);
    std::string TokensPathUtf8 = TCHAR_TO_UTF8(*TokensPath);
    std::string DataDirUtf8 = TCHAR_TO_UTF8(*DataDir);

    // Configure the sherpa-onnx TTS engine.
    SherpaOnnxOfflineTtsConfig config;
    memset(&config, 0, sizeof(config));

    // Use the persistent C-string pointers from the std::string objects.
    config.model.kokoro.model = ModelPathUtf8.c_str();
    config.model.kokoro.voices = VoicesPathUtf8.c_str();
    config.model.kokoro.tokens = TokensPathUtf8.c_str();
    config.model.kokoro.data_dir = DataDirUtf8.c_str();
    config.model.kokoro.length_scale = 1.0f;
    config.model.num_threads = 2;
    config.model.debug = 1;

    // Prepare the input text (create a persistent string for the text)
    std::string InputTextUtf8 = TCHAR_TO_UTF8(*TextToSpeak);
    const char* inputText = InputTextUtf8.c_str();
    UE_LOG(LogTemp, Log, TEXT("Input text (UTF8): %s"), UTF8_TO_TCHAR(inputText));
    int32_t speaker_id = 0;
    float speed = 1.0f;

    // Create the TTS instance.
    UE_LOG(LogTemp, Log, TEXT("Creating TTS instance..."));
    const SherpaOnnxOfflineTts* tts = SherpaOnnxCreateOfflineTts(&config);
    if (!tts)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create sherpa-onnx TTS instance."));
        return false;
    }
    UE_LOG(LogTemp, Log, TEXT("TTS instance created successfully."));

    // Generate audio with a progress callback.
    UE_LOG(LogTemp, Log, TEXT("Generating audio..."));
    const SherpaOnnxGeneratedAudio* audio = SherpaOnnxOfflineTtsGenerateWithProgressCallback(tts, inputText, speaker_id, speed, ProgressCallback);
    if (!audio)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to generate audio from text."));
        SherpaOnnxDestroyOfflineTts(tts);
        return false;
    }
    UE_LOG(LogTemp, Log, TEXT("Audio generation completed."));

    if (!audio || !audio->samples)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid generated audio."));
        SherpaOnnxDestroyOfflineTts(tts);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Converting audio: NumFloatSamples=%d, SampleRate=%d"), audio->n, audio->sample_rate);
    const int32 NumFloatSamples = audio->n;
    const int32 SampleRate = audio->sample_rate;
    const int32 NumChannels = 1; // Mono audio

    OutPCM.AddUninitialized(NumFloatSamples * sizeof(int16));  // 2 bytes per sample
    OutSampleRate = SampleRate;
    OutNumChannels = NumChannels;

    for (int32 i = 0; i < NumFloatSamples; i++)
    {
        float SampleFloat = FMath::Clamp(audio->samples[i], -1.0f, 1.0f);
        int16 SampleInt16 = static_cast<int16>(SampleFloat * 32767.0f);
        ((int16*)OutPCM.GetData())[i] = SampleInt16;
    }
    UE_LOG(LogTemp, Log, TEXT("PCM data conversion complete."));

    // Clean up the TTS instance.
    SherpaOnnxDestroyOfflineTts(tts);
    UE_LOG(LogTemp, Log, TEXT("Cleaned up TTS instance."));

    return true;
}