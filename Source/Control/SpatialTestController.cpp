/*
  ==============================================================================

    SpatialTestController.cpp
    Created: 7 Nov 2025 4:34:29pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpatialTestController.h"
#include "../MainComponent.h"
#include <BinaryData.h>

SpatialTestController::SpatialTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef)
    	: TestController(mainComponentRef, soundEngineRef), timer([this] {timerCallback();})
{
    currentState = TestState::END;

    firstAzimuth = 0.0f;
    secondAzimuth = 0.0f;
    moveLeft = false;
    responseLeft = false;

    // Using a fixed SNR for the test
    signalAmplitude = dbToAmplitude(signalAmplitudeDb);
    maskingAmplitude = dbToAmplitude(maskingAmplitudeDb);
}

void SpatialTestController::startTest() {
    currentState = TestState::START;
    currentTrial = 0;
    scheduleNextState(2000);
}

void SpatialTestController::stopTest() {
    timer.stopTimer();
    soundEngine.stop();
}

void SpatialTestController::buttonClicked(const juce::String& id) {
    if (id == "stopButton") {
        stopTest();
        return;
    }
    if (currentState == TestState::AWAIT_RESPONSE) {
        userResponded = true;
        responseLeft = (id == "leftButton");
        if (responseLeft == moveLeft) {
            DBG("Correct response");
            
        }
        else {
            DBG("Incorrect response");
        }

        SpatialTestResponse response;
        response.referenceAzimuth = firstAzimuth;
        response.targetAzimuth = secondAzimuth;
        response.spatialCorrect = (responseLeft == moveLeft);
        response.snr = signalAmplitudeDb - maskingAmplitudeDb;

        results.responses.push_back(response);

        currentState = TestState::NEXT_TRIAL;
        scheduleNextState(static_cast<int>(interTrialDelay * 1000));
    }
}

const SpatialTestResults SpatialTestController::getResults() {
    return results;
}

void SpatialTestController::timerCallback() {
    timer.stopTimer();

    switch (currentState) {
        case TestState::START:
            currentTrial = 1;
            currentState = TestState::TRIAL_START;
            playMaskingNoise();
            scheduleNextState(static_cast<int>(preSignalDelay * 1000));
            break;

        case TestState::TRIAL_START:
            playFirstSound();
            scheduleNextState(static_cast<int>(signalDuration * 1000));
            currentState = TestState::FIRST_SOUND;
            break;

        case TestState::FIRST_SOUND:
            currentState = TestState::WAIT_BETWEEN_SOUNDS;
            scheduleNextState(static_cast<int>(interSignalDelay * 1000));
            break;

        case TestState::WAIT_BETWEEN_SOUNDS:
            currentState = TestState::SECOND_SOUND;
            playSecondSound();
            scheduleNextState(static_cast<int>(signalDuration * 1000));
            break;

        case TestState::SECOND_SOUND:
            currentState = TestState::AWAIT_RESPONSE;
            userResponded = false;
            break;

        case TestState::AWAIT_RESPONSE:
            break;

        case TestState::NEXT_TRIAL:
            if (currentTrial < numTrials) {
                currentTrial++;
                currentState = TestState::TRIAL_START;
                playMaskingNoise();
                scheduleNextState(static_cast<int>(preSignalDelay * 1000));
            }
            else {
                currentState = TestState::END;
                scheduleNextState(1);
            }
            break;
        case TestState::END:
            mainComponent.showSpatialResultsScreen();
            stopTest();
            break;
    }
}

void SpatialTestController::scheduleNextState(int delayMs) {
    timer.stopTimer();
    timer.startTimer(delayMs);
}

void SpatialTestController::playFirstSound() {
    auto idx = static_cast<size_t>(random.nextInt((int)testAzimuths.size() ));
    firstAzimuth = testAzimuths[idx];
    soundEngine.playNoiseSpatial(signalAmplitude, signalDuration, 0.0f, firstAzimuth);

}

void SpatialTestController::playSecondSound() {
    // Choose direction to move
    if (firstAzimuth >= 90.0f) {
        moveLeft = false;
    }
    else if (firstAzimuth <= -90.0f) {
        moveLeft = true;
    }
    else {
        moveLeft = random.nextBool();
    }

    secondAzimuth = moveLeft ? firstAzimuth + 15.0f : firstAzimuth - 15.0f;

    soundEngine.playNoiseSpatial(signalAmplitude, signalDuration, 0.0f, secondAzimuth);

}

void SpatialTestController::playMaskingNoise() {
    float maskingDuration = preSignalDelay + 2.0f * signalDuration + interSignalDelay + postSignalMasking;
    for(float az : maskingAzimuths)
        soundEngine.playNoiseSpatial(maskingAmplitude / maskingAzimuths.size(), maskingDuration, 0.0f, az);
}