/*
  ==============================================================================

    SpatialTestController.h
    Created: 7 Nov 2025 4:34:29pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "TestController.h"
#include "../Results/SpatialTestResults.h"
#include "TestControllerTimer.h"

class SpatialTestController : public TestController {
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

		float signalAmplitudeDb = -20.0f;
		float maskingAmplitudeDb = -25.0f;

		int signalDurationMs = 1000;
		int interSignalDelayMs = 500;
		int preSignalDelayMs = 1000;
		int postSignalMaskingMs = 500;

		int interTrialDelayMs = 1000;

		static Config loadFromFile(const juce::File& file);
	};

	SpatialTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& configFile);

	void startTest() override;
	void stopTest() override;

	void buttonClicked(const juce::String& id) override;

	 const SpatialTestResults getResults();
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

	void playFirstSound();
	void playSecondSound();
	void playMaskingNoise();

	int currentTrial = 0;

	float firstAzimuth, secondAzimuth;
	bool moveLeft;

	juce::Random random;

	float signalAmplitude = 0.25f;
	float maskingAmplitude = 0.1f;

	bool userResponded = false;
	bool responseLeft;

	SpatialTestResults results;

};