/*
  ==============================================================================

    DualTaskTestController.cpp
    Created: 4 Dec 2025 12:30:20pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "DualTaskTestController.h"
#include "../MainComponent.h"

DualTaskTestController::DualTaskTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef) 
    : TestController(mainComponentRef, soundEngineRef) 
{
    fm = std::make_unique<SpeechFileManager>();

    currentState = TestState::END;

    firstAzimuth = 0.0f;
    secondAzimuth = 0.0f;
    moveLeft = false;
    spatialResponseLeft = false;

    referenceWord = "";
    targetWord = "";
    chosenWord = "";

    // Using a fixed SNR for the test
    signalAmplitude = dbToAmplitude(signalAmplitudeDb);
    maskingAmplitude = dbToAmplitude(maskingAmplitudeDb);
}

void DualTaskTestController::startTest() {
    //chooserandomwordgroup();
    //setinputsenabled(true);

    //playreferenceword();

    //playtargetword();
    currentState = TestState::START;
    currentTrial = 0;
    scheduleNextState(2000);
}

void DualTaskTestController::stopTest() {
    soundEngine.stop();
    stopTimer();
}

void DualTaskTestController::buttonClicked(const juce::String& id) {
    if (id == "stopButton") {
        stopTest();
        return;
    }

    if (currentState == TestState::AWAIT_RESPONSE) {
        if (id.startsWith("word")) {
            int wordIdx = id.substring(4).getIntValue();
            DBG("word index: " << wordIdx << " (" << currentWordGroup[wordIdx] << ")");
            if (wordIdx >= 0 && wordIdx < currentWordGroup.size()) {
                chosenWord = currentWordGroup[wordIdx];
                userRespondedSpeech = true;
            }
        }
        else if (id == "leftButton") {
            userRespondedSpatial = true;
        }
        else if (id == "rightButton") {
            userRespondedSpatial = true;
        }

        if (userRespondedSpatial && userRespondedSpeech) {
            checkUserResponse();
            currentState = TestState::NEXT_TRIAL;
            scheduleNextState(static_cast<int>(interTrialDelay * 1000));
        }
    }
}

void DualTaskTestController::checkUserResponse() {
    DualTaskTestResponse response;
    // Check if spatial correct
    response.spatialTestResponse.referenceAzimuth = firstAzimuth;
    response.spatialTestResponse.targetAzimuth = secondAzimuth;
    response.spatialTestResponse.spatialCorrect = (spatialResponseLeft == moveLeft);
    response.spatialTestResponse.snr = signalAmplitudeDb - maskingAmplitudeDb;

    // Check if word correct
    response.wordTestResponse.targetWord = targetWord;
    response.wordTestResponse.reportedWord = chosenWord;
    response.wordTestResponse.wordCorrect = (targetWord == chosenWord);
    response.wordTestResponse.snr = signalAmplitudeDb - maskingAmplitudeDb;
    
    DBG("Spatial response " << (response.spatialTestResponse.spatialCorrect ? "correct" : "incorrect"));
    DBG("Word response " << (response.wordTestResponse.wordCorrect ? "correct" : "incorrect"));

    results.responses.push_back(response);
}

const DualTaskTestResults DualTaskTestController::getResults() {
    return results;
}

void DualTaskTestController::timerCallback() {
    stopTimer();

    switch (currentState) {
        case TestState::START:
            currentTrial = 1;
            currentState = TestState::TRIAL_START;
            playMaskingNoise();
            scheduleNextState(static_cast<int>(preSignalDelay * 1000));
            break;
        case TestState::TRIAL_START:
            chooseRandomWordGroup();
            playReferenceWord();
            scheduleNextState(static_cast<int>(signalDuration * 1000));
            currentState = TestState::FIRST_SOUND;
            break;

        case TestState::FIRST_SOUND:
            currentState = TestState::WAIT_BETWEEN_SOUNDS;
            scheduleNextState(static_cast<int>(interSignalDelay * 1000));
            break;

        case TestState::WAIT_BETWEEN_SOUNDS:
            currentState = TestState::SECOND_SOUND;
            playTargetWord();
            scheduleNextState(static_cast<int>(signalDuration * 1000));
            break;

        case TestState::SECOND_SOUND:
            currentState = TestState::AWAIT_RESPONSE;

            userRespondedSpatial = false;
            userRespondedSpeech = false;

            setInputsEnabled(true);
            break;

        case TestState::AWAIT_RESPONSE:
            break;

        case TestState::NEXT_TRIAL:
            setInputsEnabled(false);
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
            mainComponent.showDualTaskResultsScreen();
            stopTest();
            break;
    }
}

void DualTaskTestController::scheduleNextState(int delayMs) {
    stopTimer();
    startTimer(delayMs);
}

void DualTaskTestController::playReferenceWord() {
    auto azIdx = static_cast<size_t>(random.nextInt((int)testAzimuths.size()));
    firstAzimuth = testAzimuths[azIdx];

    size_t wordIdx = abs(random.nextInt()) % currentWordGroup.size();
    referenceWord = currentWordGroup[wordIdx];

    playWordSpatial(referenceWord, firstAzimuth);
}

void DualTaskTestController::playTargetWord() {
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

    size_t wordIdx = abs(random.nextInt()) % currentWordGroup.size();
    targetWord = currentWordGroup[wordIdx];

    playWordSpatial(targetWord, secondAzimuth);

}

void DualTaskTestController::playMaskingNoise() {
    float maskingDuration = preSignalDelay + 2.0f * signalDuration + interSignalDelay + postSignalMasking;
    for (float az : maskingAzimuths)
        soundEngine.playNoiseSpatial(maskingAmplitude / maskingAzimuths.size(), maskingDuration, 0.0f, az);
}

void DualTaskTestController::playWordSpatial(juce::String word, float azimuth) {
    soundEngine.playSampleSpatial(fm->getWordFile(word), 0.0f, azimuth, signalAmplitude, true);
}

void DualTaskTestController::chooseRandomWordGroup() {
    std::vector<juce::String> wordGroups = fm->getWordGroupIds();

    size_t idx = static_cast<size_t>(abs(random.nextInt()) % wordGroups.size());
    juce::String currentGroupId = wordGroups[idx];
    currentWordGroup = fm->getWordsInGroup(currentGroupId);
    setWords(currentWordGroup);
}
