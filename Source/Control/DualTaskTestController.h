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
	// Configuration parameters
	struct Config {
		juce::String name = "";

		std::vector<float> testAzimuths = { 
			-90.0f,
			-75.0f,
			-60.0f,
			-45.0f,
			-30.0f,
			-15.0f,
			 0.0f, 
			 15.0f,
			 30.0f, 
			 45.0f, 
			 60.0f, 
			 75.0f, 
			 90.0f 
		};

		std::vector<float> maskingAzimuths = { 
			-90.0f,
			-30.0f,
			 30.0f,
			 90.0f 
		};

		int numTrials = 10;

		float signalAmplitudeDb = -10.0f;
		float maskingAmplitudeDb = -25.0f;

		// Timing Parameters
		int signalDurationMs = 1000;
		int interSignalDelayMs = 500;
		int preSignalDelayMs = 1000;
		int postSignalMaskingMs = 500;
		int interTrialDelayMs = 1000;

		static Config loadFromFile(const juce::File& file);
	};

	DualTaskTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& configFile);

	void startTest() override;
	void stopTest() override;

	void buttonClicked(const juce::String& id) override;

	const DualTaskTestResults getResults();

	std::function<void(bool)> setInputsEnabled;
	std::function<void(const std::vector<juce::String>&)> setWords;

private:
	Config config;

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

	int currentTrial = 0;
	float firstAzimuth, secondAzimuth;
	bool moveLeft;

	float signalAmplitude = 0.25f;
	float maskingAmplitude = 0.1f;

	bool userRespondedSpatial = false;
	bool spatialResponseLeft;

	bool userRespondedSpeech = false;
	juce::String chosenWord;

	DualTaskTestResults results;
};