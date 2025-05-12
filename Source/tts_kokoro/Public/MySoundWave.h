#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWaveProcedural.h"
#include "MySoundWave.generated.h"

/**
 * A custom SoundWaveProcedural class that plays pre-buffered PCM data.
 * This avoids direct access to the internal bulk data and works with UE5.5+.
 */
UCLASS()
class UMySoundWave : public USoundWaveProcedural
{
    GENERATED_BODY()

public:
    // Constructor that takes an FObjectInitializer.
    UMySoundWave(const FObjectInitializer& ObjectInitializer);

    /**
     * Initializes the sound wave with PCM data.
     *
     * @param InPCMData     The PCM data (16-bit little-endian).
     * @param InSampleRate  The sample rate of the PCM data.
     * @param InNumChannels The number of channels (1 for mono, 2 for stereo, etc.).
     */
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void Init(const TArray<uint8>& InPCMData, int32 InSampleRate, int32 InNumChannels);

    /**
     * Resets the PCM playback position to the beginning.
     * Call this if you need to replay the sound from the start.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void ResetPCMData();

    // USoundWaveProcedural override

    /**
     * Provides PCM data for playback.
     *
     * @param OutBuffer  The buffer to write PCM data to.
     * @param BufferSize The maximum number of bytes to write.
     * @return The number of bytes actually written; returning 0 stops playback.
     */
    virtual int32 GeneratePCMData(uint8* OutBuffer, int32 BufferSize) override;

private:
    // The PCM data buffer.
    TArray<uint8> PCMData;

    // The current read position in the PCMData array.
    int32 PCMDataPosition;
};
