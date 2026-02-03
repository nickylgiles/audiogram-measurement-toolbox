/*
  ==============================================================================

    PureToneTestController.cpp
    Created: 7 Nov 2025 4:33:08pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "PureToneTestController.h"
#include "../MainComponent.h"

PureToneTestController::PureToneTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef)
    : TestController(mainComponentRef, soundEngineRef), timer([this] {timerCallback();}) {
    currentState = TestState::END;
    for (auto tone : testTones) {
        toneThresholds[0][tone] = dbLevelMax;
        toneThresholds[1][tone] = dbLevelMax;
    }
}

void PureToneTestController::startTest() {
    currentTone = 0;
    currentEar = 0;
    currentThreshold = dbLevelMin;

    currentToneDetected = false;
    currentState = TestState::START;

    scheduleNextTone(2000);
}

void PureToneTestController::buttonClicked(const juce::String& id) {
    if (id == "hearButton") {
        toneHeard();
        return;
    }
    if (id == "stopButton") {
        stopTest();
        return;
    }
}

void PureToneTestController::toneHeard() {
    currentToneDetected = true;
}

void PureToneTestController::stopTest() {
    timer.stopTimer();
    soundEngine.stop();
}



void PureToneTestController::timerCallback() {
    timer.stopTimer();

    switch (currentState) {
        case TestState::START:
            currentState = TestState::DB_ASCENDING;
            break;

        case TestState::DB_ASCENDING:
            if (currentToneDetected) {
                if (currentThreshold > dbLevelMin && !floatsEqual(currentThreshold, dbLevelMin))
                    currentState = TestState::DB_DESCENDING;
                else
                    currentState = TestState::NEXT_TONE;
                toneThresholds[currentEar][testTones[currentTone]] = currentThreshold;
            }
            else {
                currentThreshold += dbIncrementAscending;
                if (currentThreshold > dbLevelMax && !floatsEqual(currentThreshold, dbLevelMax)) {
                    currentState = TestState::NEXT_TONE;
                }
            }
            break;

        case TestState::DB_DESCENDING:
            if (currentToneDetected) {
                toneThresholds[currentEar][testTones[currentTone]] = currentThreshold;
                if (currentThreshold > dbLevelMin && !floatsEqual(currentThreshold, dbLevelMin)) {
                    currentThreshold -= dbIncrementDescending;
                }
                else {
                    currentState = TestState::NEXT_TONE;
                }
            }
            else {
                currentState = TestState::NEXT_TONE;
            }
            break;

        case TestState::NEXT_TONE:
            if (currentTone < (testTones.size() - 1)) {
                currentTone++;
                currentState = TestState::DB_ASCENDING;
                currentThreshold = dbLevelMin;
            }
            else {
                if (currentEar < 1) {
                    currentEar = 1;
                    currentState = TestState::DB_ASCENDING;
                    currentThreshold = dbLevelMin;
                    currentTone = 0;
                }
                else {
                    currentState = TestState::END;
                }
            }
            break;

        case TestState::END:
            stopTest();
            if (onTestFinished)
                onTestFinished();
            return;
    }

    if (currentState == TestState::DB_ASCENDING || currentState == TestState::DB_DESCENDING) {
        playCurrentTone();
        scheduleNextTone(2000);
    }
    if (currentState == TestState::NEXT_TONE) {
        scheduleNextTone(0);
    }
    if (currentState == TestState::END) {
        scheduleNextTone(0);
    }

    
}

void PureToneTestController::playCurrentTone() {

    soundEngine.playToneMasked(testTones[currentTone], dbToAmplitude(currentThreshold), 1.0f, currentEar);
    currentToneDetected = false;
}

/*static*/ constexpr bool PureToneTestController::floatsEqual(float a, float b) {
    return (a - b < 0.01f && a - b > -0.01f);
}

PureToneTestResults const PureToneTestController::getResults() {
    return toneThresholds;
}

void PureToneTestController::scheduleNextTone(int delayMs) {
    timer.stopTimer();
    timer.startTimer(delayMs);
}
