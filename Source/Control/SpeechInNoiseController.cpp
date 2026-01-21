/*
  ==============================================================================

    SpeechInNoiseController.cpp
    Created: 29 Nov 2025 1:23:35am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpeechInNoiseController.h"
#include "../MainComponent.h"

class MainComponent;

SpeechInNoiseController::SpeechInNoiseController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef) 
    : TestController(mainComponentRef, soundEngineRef)
{
    fm = std::make_unique<SpeechFileManager>();
    currentSequence.resize(numDigits);
}

void SpeechInNoiseController::startTest() {
    currentState = TestState::START;
    currentTrial = 0;
    scheduleNextState(1000);
}

void SpeechInNoiseController::stopTest() {
    soundEngine.stop();
    stopTimer();
}

void SpeechInNoiseController::buttonClicked(const juce::String& id) {
    if (id == "stopButton") {
        stopTest();
        return;
    }

    if (currentState == TestState::AWAIT_RESPONSE) {
        if ((id.length() == 1) && juce::CharacterFunctions::isDigit(id[0])) {
            digitInput(id.getIntValue());
        }
    }
}

const float SpeechInNoiseController::getSRT() {
    // Average of presented triplets (4th onward)
    float total = 0.0f;
    for (int i = 3; i < trialSNRs.size(); ++i) {
        total += trialSNRs[i];
    }

    return total / (trialSNRs.size() - 3);
}

const SpeechInNoiseTestResults SpeechInNoiseController::getResults() {
    return results;
}

void SpeechInNoiseController::scheduleNextState(int delayMs) {
    stopTimer();
    startTimer(delayMs);
}

void SpeechInNoiseController::setLevels(float snr) {
    const float noiseRefDb = -20.0f;
    const float speechRefDb = -20.0f;
    if (snr < 0.0f) {
        digitAmplitude = dbToAmplitude(noiseRefDb + snr);
        maskingAmplitude = dbToAmplitude(noiseRefDb);
    }
    else {
        digitAmplitude = dbToAmplitude(speechRefDb);
        maskingAmplitude = dbToAmplitude(speechRefDb - snr);
    }
    DBG("Levels set: SNR = " << snr);
    DBG("Digits amplitude = " << digitAmplitude);
    DBG("Masking amplitude = " << maskingAmplitude);
}

void SpeechInNoiseController::makeRandomSequence() {
    for (int i = 0; i < numDigits; ++i) {
        currentSequence[i] = abs(random.nextInt() % 10);
    }
}

void SpeechInNoiseController::playDigit(int digit) {
    if (digit < 0 || digit > 9) {
        DBG("Digit out of range.");
        return;
    }
    const auto& res = fm->getDigit(digit);
    soundEngine.playSample(res.data, res.dataSize, digitAmplitude);
}

void SpeechInNoiseController::playMaskingNoise() {
   soundEngine.playNoise(maskingAmplitude, maskingDuration, 0);
   soundEngine.playNoise(maskingAmplitude, maskingDuration, 1);
}

void SpeechInNoiseController::digitInput(int digit) {
    userInput.push_back(digit);

    if (userInput.size() >= numDigits) {
        inputCorrect = true;
        for (int i = 0; i < numDigits; ++i) {
            if (userInput[i] != currentSequence[i])
                inputCorrect = false;
        }

        juce::String userStr, correctStr;
        for (int i = 0; i < numDigits; ++i) {
            userStr += juce::String(userInput[i]) + " ";
            correctStr += juce::String(currentSequence[i]) + " ";
        }
        DBG("Correct sequence: " << correctStr);
        DBG("Your sequence: " << userStr << (inputCorrect ? " (correct)" : " (incorrect)"));

        SpeechInNoiseTestResponse response;
        response.targetWord = correctStr;
        response.reportedWord = userStr;
        response.wordCorrect = inputCorrect;

        response.snr = currentSNR;

        results.responses.push_back(response);
          

        currentState = TestState::NEXT_TRIAL;
        timerCallback();
    }
}

void SpeechInNoiseController::timerCallback() {
    stopTimer();
    switch (currentState) {
    case SpeechInNoiseController::TestState::START:
        currentSNR = dbInitial;
        setLevels(currentSNR);
        trialSNRs.push_back(currentSNR);
        currentTrial = 1;
        currentState = TestState::TRIAL_START;
        timerCallback();
        break;

    case SpeechInNoiseController::TestState::TRIAL_START:
        playMaskingNoise();
        currentDigit = 0;
        makeRandomSequence();
        currentState = TestState::READ_DIGITS;
        scheduleNextState(static_cast<int>(1000 * preDigitDelay));
        break;

    case SpeechInNoiseController::TestState::READ_DIGITS:
        if (currentDigit < numDigits) {
            playDigit(currentSequence[currentDigit]);
            currentDigit++;
            float jitter = random.nextFloat() * interDigitJitter;
            scheduleNextState(static_cast<int>(1000 * (interDigitDelay + jitter)));
        }
        else {
            currentState = TestState::AWAIT_RESPONSE;

            setInputsEnabled(true);
            userInput.clear();

            timerCallback();
        }
        break;

    case SpeechInNoiseController::TestState::AWAIT_RESPONSE:
        break;

    case SpeechInNoiseController::TestState::NEXT_TRIAL:
        setInputsEnabled(false);
        trialSNRs.push_back(currentSNR);
        if (inputCorrect) {
            currentSNR -= dbIncrementDescending;
        }
        else {
            currentSNR += dbIncrementAscending;
        }
        
        setLevels(std::clamp(currentSNR, dbMin, dbMax));

        if (currentTrial < numTrials) {
            ++currentTrial;
            currentState = TestState::TRIAL_START;
            scheduleNextState(static_cast<int>(1000 * interTrialDelay));
        }
        else {
            currentState = TestState::END;
            timerCallback();
        }
        break;

    case SpeechInNoiseController::TestState::END:
        DBG("Test Complete.  SRT = " << getSRT());
        results.srt = getSRT();
        stopTest();
        mainComponent.showSpeechInNoiseResultsScreen();
        break;

    default:
        break;
    }
}
