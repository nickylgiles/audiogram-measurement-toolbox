/*
  ==============================================================================

    SpeechInNoiseController.h
    Created: 29 Nov 2025 1:23:35am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "TestController.h"
#include "../Audio/Speech/SpeechFileManager.h"
#include "../Results/SpeechInNoiseTestResults.h"
#include "TestControllerTimer.h"

class DigitsInNoiseController : public TestController {
public:
    DigitsInNoiseController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef);
    void startTest() override;
    void stopTest() override;

    void buttonClicked(const juce::String& id) override;

    const float getSRT();

    const SpeechInNoiseTestResults getResults();

    std::function<void(bool)> setInputsEnabled;

private:
    TestControllerTimer timer;

    enum class TestState {
        START,
        TRIAL_START,
        READ_DIGITS,
        AWAIT_RESPONSE,
        NEXT_TRIAL,
        END
    };

    TestState currentState = TestState::END;

    void scheduleNextState(int delayMs);

    void setLevels(float snr);
    void makeRandomSequence();
    void playDigit(int digit);
    void playMaskingNoise();

    void digitInput(int digit);
    
    std::unique_ptr<SpeechFileManager> fm;

    void timerCallback();

    float digitAmplitude = 1.0f;

    float preDigitDelay = 0.5f;
    float interDigitDelay = 1.0f;
    float interDigitJitter = 0.1f;

    float interTrialDelay = 1.0f;

    float maskingAmplitude = 0.1f;
    float maskingDuration = 3.5f;


    // SNR Parameters
    float dbIncrementAscending = 2.0f;
    float dbIncrementDescending = 2.0f;
    float dbInitial = 0.0f;
    float dbMax = 20.0f;
    float dbMin = -20.0f;

    float currentSNR = 0.0f;

    std::vector<float> trialSNRs;
    
    int numTrials = 10;
    int currentTrial = 0;
    std::vector<int> currentSequence;
    int numDigits = 3;

    juce::Random random;

    int currentDigit = 0;

    std::vector<int> userInput;
    bool inputCorrect = false;

    SpeechInNoiseTestResults results;
};