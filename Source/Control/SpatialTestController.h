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

class SpatialTestController : public TestController, private juce::Timer {
public:
	SpatialTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef);
	void startTest() override;
	void stopTest() override;

	void buttonClicked(const juce::String& id) override;

	 const SpatialTestResults getResults();
private:

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

	void timerCallback() override;

	void scheduleNextState(int delayMs);

	void playFirstSound();
	void playSecondSound();
	void playMaskingNoise();

	std::vector<float> testAzimuths = { -90.0f, -75.0f, -60.0f, -45.0f, -30.0f, -15.0f, 0.0f, 15.0f, 30.0f, 45.0f, 60.0f, 75.0f, 90.0f };

	std::vector<float> maskingAzimuths = { -90.0f, -30.0f, 30.0f, 90.0f };

	int currentTrial = 0;
	int numTrials = 10;

	float firstAzimuth, secondAzimuth;
	bool moveLeft;

	juce::Random random;

	float signalAmplitude = 1.0f;
	float maskingAmplitude = 0.1f;

	float signalDuration = 1.0f;
	float interSignalDelay = 0.5f;
	float preSignalDelay = 1.0f;
	float postSignalMasking = 0.5f;

	float interTrialDelay = 1.0f;

	bool userResponded = false;
	bool responseLeft;

	SpatialTestResults results;

};