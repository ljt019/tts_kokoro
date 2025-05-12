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
 * Blueprint node "Generate TTS SoundWave".
 * Does *not* block. When done, OnSuccess or OnFailure fires.
 */
UCLASS(meta = (DisplayName = "Generate TTS SoundWave Async"))
class TTS_KOKORO_API UTTSAsyncAction : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    // The factory (blueprint node).
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Generate TTS SoundWave Async"), Category = "TTS")
    static UTTSAsyncAction* GenerateTTSSoundWaveAsync(
        UObject* WorldContextObject,
        const FString& TextToSpeak);

    // Fired on success. Passes the new SoundWave.
    UPROPERTY(BlueprintAssignable)
    FOnTTSCompleted OnSuccess;

    // Fired on failure.
    UPROPERTY(BlueprintAssignable)
    FOnTTSFailed OnFailure;

    // UObject interface
    virtual void Activate() override;

private:
    // Stash these from the factory call.
    UObject* WorldContextObject;
    FString TextToSpeak;

    // Progress callback for TTS generation.
    static int32 ProgressCallback(const float* /*samples*/, int32 /*num_samples*/, float progress);

    // Internal worker: generates PCM data from text input
    bool GeneratePCMData(TArray<uint8>& OutPCM,
        int32& OutSampleRate,
        int32& OutNumChannels);
};