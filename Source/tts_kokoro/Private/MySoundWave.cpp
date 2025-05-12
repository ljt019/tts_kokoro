#include "MySoundWave.h"

UMySoundWave::UMySoundWave(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , PCMDataPosition(0)
{
    // Mark as procedural so that GeneratePCMData is called.
    bProcedural = true;

    // Do not loop unless desired.
    bLooping = false;
}

void UMySoundWave::Init(const TArray<uint8>& InPCMData, int32 InSampleRate, int32 InNumChannels)
{
    // Set the audio properties.
    SampleRate = InSampleRate;
    NumChannels = InNumChannels;

    // Assuming 16-bit (2 bytes per sample) PCM data:
    int32 NumSamples = InPCMData.Num() / (sizeof(int16) * InNumChannels);
    Duration = static_cast<float>(NumSamples) / static_cast<float>(InSampleRate);

    // Store a copy of the PCM data.
    PCMData = InPCMData;
    PCMDataPosition = 0;

    // Invalidate any previously compressed data.
    InvalidateCompressedData();
}

void UMySoundWave::ResetPCMData()
{
    PCMDataPosition = 0;
}

int32 UMySoundWave::GeneratePCMData(uint8* OutBuffer, int32 BufferSize)
{
    // Calculate how many bytes remain.
    int32 BytesRemaining = PCMData.Num() - PCMDataPosition;
    if (BytesRemaining <= 0)
    {
        // No more data available; playback will stop.
        return 0;
    }

    // Determine how many bytes to copy in this call.
    int32 BytesToCopy = FMath::Min(BufferSize, BytesRemaining);
    FMemory::Memcpy(OutBuffer, PCMData.GetData() + PCMDataPosition, BytesToCopy);
    PCMDataPosition += BytesToCopy;

    return BytesToCopy;
}
