#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundWave.h"
#include "tts_kokoroBPLibrary.generated.h"

UCLASS()
class TTS_KOKORO_API Utts_kokoroBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Generate TTS audio from the provided text.
	 * @param InText - The input text to synthesize.
	 * @return A USoundWave pointer containing the generated audio (or nullptr on failure).
	 */
	UFUNCTION(BlueprintCallable, Category = "TTS", meta = (DisplayName = "Generate TTS Audio", Keywords = "TTS Kokoro"))
	static USoundWave* GenerateTtsAudio(const FString& InText);
};
