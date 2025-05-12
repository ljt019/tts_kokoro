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
    // Constructor declaration (with a default parameter for convenience)
    UTTSBPLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * Speaks the provided text using the Text-To-Speech system.
     *
     * @param TextToSpeak The text to be spoken.
     */
    UFUNCTION(BlueprintCallable, Category = "TTS")
    static USoundWave* Speak(const FString& TextToSpeak);

private:
    // Helper function to convert generated audio to a USoundWave.
    static USoundWave* ConvertGeneratedAudioToSoundWave(const struct SherpaOnnxGeneratedAudio* GeneratedAudio);
};
