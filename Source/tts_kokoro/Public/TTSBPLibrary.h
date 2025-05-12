#pragma once

#include "tts_kokoro.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TTSBPLibrary.generated.h"

UCLASS()
class UTTSBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UTTSBPLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * Generates a SoundWave from the provided text using the KokoroTTs Model.
     * 
     * NOTE: This is a SYNCHRONOUS function that will block until processing completes.
     * Consider using GenerateTTSSoundWaveAsync for non-blocking operation, for most cases.
     *
     * @param TextToSpeak The text to be used for generating the soundwave.
     */
    UFUNCTION(BlueprintCallable, Category = "TTS", DisplayName = "Generate TTS SoundWave")
    static USoundWave* GenerateTTSSoundWave(const FString& TextToSpeak);

private:
    // Helper function to convert generated audio to a USoundWave.
    static USoundWave* ConvertGeneratedAudioToSoundWave(const struct SherpaOnnxGeneratedAudio* GeneratedAudio);
};