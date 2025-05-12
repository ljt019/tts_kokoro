#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "MySoundWave.h"
#include "TTSAsyncAction.generated.h"

// Delegate fired when the SoundWave is ready.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnTTSCompleted, USoundWave*, SoundWave);

// Delegate fired on failure.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTTSFailed);

/**
 * Generates a SoundWave from the provided text using the KokoroTTs Model asynchronously.
 *
 * @param TextToSpeak The text to be used for generating the soundwave.
 */
UCLASS(meta = (DisplayName = "Generate TTS SoundWave Async"))
class TTS_KOKORO_API UTTSAsyncAction : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Generate TTS SoundWave Async"), Category = "TTS")
    static UTTSAsyncAction* GenerateTTSSoundWaveAsync(
        UObject* WorldContextObject,
        const FString& TextToSpeak);

    UPROPERTY(BlueprintAssignable)
    FOnTTSCompleted OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnTTSFailed OnFailure;

    virtual void Activate() override;

private:
    UObject* WorldContextObject;
    FString TextToSpeak;

    // Progress callback for TTS generation.
    static int32 ProgressCallback(const float* /*samples*/, int32 /*num_samples*/, float progress);

    // Internal worker: generates PCM data from text input
    bool GeneratePCMData(TArray<uint8>& OutPCM,
        int32& OutSampleRate,
        int32& OutNumChannels);
};