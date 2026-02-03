/*
  ==============================================================================

    DualTaskTestController.h
    Created: 4 Dec 2025 12:30:20pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TestController.h"
#include "../Results/DualTaskTestResults.h"
#include "../Audio/Speech/SpeechFileManager.h"
#include "TestControllerTimer.h"

class DualTaskTestController : public TestController {
public:
	DualTaskTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef);
	void startTest() override;
	void stopTest() override;

	void buttonClicked(const juce::String& id) override;

	const DualTaskTestResults getResults();

	std::function<void(bool)> setInputsEnabled;
	std::function<void(const std::vector<juce::String>&)> setWords;

private:
	TestControllerTimer timer;

	enum class TestState {
		START,
		TRIAL_START,
		FIRST_SOUND,
		WAIT_BETWEEN_SOUNDS,
		SECOND_SOUND,
		AWAIT_RESPONSE,
		NEXT_TRIAL,
		END
	};

	TestState currentState{ TestState::END };

	void timerCallback();

	void scheduleNextState(int delayMs);

	void checkUserResponse();

	void playReferenceWord();
	void playTargetWord();
	void playMaskingNoise();

	void playWordSpatial(juce::String word, float azimuth);

	void chooseRandomWordGroup();

	std::unique_ptr<SpeechFileManager> fm;
	juce::Random random;

	std::vector<juce::String> currentWordGroup;
	juce::String targetWord;
	juce::String referenceWord;
	
	std::vector<float> testAzimuths = { -90.0f, -75.0f, -60.0f, -45.0f, -30.0f, -15.0f, 0.0f, 15.0f, 30.0f, 45.0f, 60.0f, 75.0f, 90.0f };

	std::vector<float> maskingAzimuths = { -90.0f, -30.0f, 30.0f, 90.0f };

	int currentTrial = 0;
	int numTrials = 10;

	float firstAzimuth, secondAzimuth;
	bool moveLeft;

	float signalAmplitudeDb = -10.0f;
	float maskingAmplitudeDb = -25.0f;

	float signalAmplitude = 0.25f;
	float maskingAmplitude = 0.1f;

	float signalDuration = 1.0f;
	float interSignalDelay = 0.5f;
	float preSignalDelay = 1.0f;
	float postSignalMasking = 0.5f;

	float interTrialDelay = 1.0f;

	bool userRespondedSpatial = false;
	bool spatialResponseLeft;

	bool userRespondedSpeech = false;
	juce::String chosenWord;

	DualTaskTestResults results;
};