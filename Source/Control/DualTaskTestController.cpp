/*
  ==============================================================================

    DualTaskTestController.cpp
    Created: 4 Dec 2025 12:30:20pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "DualTaskTestController.h"
#include "../MainComponent.h"

DualTaskTestController::DualTaskTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& configFile)
    : TestController(mainComponentRef, soundEngineRef), timer([this] {timerCallback();})
{
    config = Config::loadFromFile(configFile);

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
    signalAmplitude = dbToAmplitude(config.signalAmplitudeDb);
    maskingAmplitude = dbToAmplitude(config.maskingAmplitudeDb);
}

void DualTaskTestController::startTest() {
    // First check if word groups exist
    if (fm->getWordGroupIds().empty()) {
        currentState = TestState::END;
        DBG("Unable to start test as no word groups were found.");
        return;
    }
    currentState = TestState::START;
    currentTrial = 0;
    scheduleNextState(2000);

#if JUCE_DEBUG

    soundEngine.startRecording("dualtask");

#endif
}

void DualTaskTestController::stopTest() {
    soundEngine.stop();
    timer.stopTimer();

#if JUCE_DEBUG

    soundEngine.stopRecording();

#endif
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
            scheduleNextState(config.interTrialDelayMs);
        }
    }
}

void DualTaskTestController::checkUserResponse() {
    DualTaskTestResponse response;
    // Check if spatial correct
    response.spatialTestResponse.referenceAzimuth = firstAzimuth;
    response.spatialTestResponse.targetAzimuth = secondAzimuth;
    response.spatialTestResponse.spatialCorrect = (spatialResponseLeft == moveLeft);
    response.spatialTestResponse.snr = config.signalAmplitudeDb - config.maskingAmplitudeDb;

    // Check if word correct
    response.wordTestResponse.targetWord = targetWord;
    response.wordTestResponse.reportedWord = chosenWord;
    response.wordTestResponse.wordCorrect = (targetWord == chosenWord);
    response.wordTestResponse.snr = config.signalAmplitudeDb - config.maskingAmplitudeDb;
    
    DBG("Spatial response " << (response.spatialTestResponse.spatialCorrect ? "correct" : "incorrect"));
    DBG("Word response " << (response.wordTestResponse.wordCorrect ? "correct" : "incorrect"));

    results.responses.push_back(response);
}

const DualTaskTestResults DualTaskTestController::getResults() {
    return results;
}

void DualTaskTestController::timerCallback() {
    timer.stopTimer();

    switch (currentState) {
        case TestState::START:
            currentTrial = 1;
            currentState = TestState::TRIAL_START;
            playMaskingNoise();
            scheduleNextState(config.preSignalDelayMs);
            break;

        case TestState::TRIAL_START:
            chooseRandomWordGroup();
            playReferenceWord();
            scheduleNextState(config.signalDurationMs);
            currentState = TestState::FIRST_SOUND;
            break;

        case TestState::FIRST_SOUND:
            currentState = TestState::WAIT_BETWEEN_SOUNDS;
            scheduleNextState(config.interSignalDelayMs);
            break;

        case TestState::WAIT_BETWEEN_SOUNDS:
            currentState = TestState::SECOND_SOUND;
            playTargetWord();
            scheduleNextState(config.signalDurationMs);
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
            if (currentTrial < config.numTrials) {
                currentTrial++;
                currentState = TestState::TRIAL_START;
                playMaskingNoise();
                scheduleNextState(config.preSignalDelayMs);
            }
            else {
                currentState = TestState::END;
                scheduleNextState(1);
            }
            break;

        case TestState::END:
            if (onTestFinished)
                onTestFinished();
            stopTest();
            break;
    }
}

void DualTaskTestController::scheduleNextState(int delayMs) {
    timer.stopTimer();
    timer.startTimer(delayMs);
}

void DualTaskTestController::playReferenceWord() {
    auto azIdx = static_cast<size_t>(
        random.nextInt(static_cast<int>( config.testAzimuths.size() ))
    );

    firstAzimuth = config.testAzimuths[azIdx];
    if (currentWordGroup.size() > 0) {
        size_t wordIdx = abs(random.nextInt()) % currentWordGroup.size();
        referenceWord = currentWordGroup[wordIdx];

        playWordSpatial(referenceWord, firstAzimuth);
    }
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
    if (currentWordGroup.size() > 0) {
        size_t wordIdx = abs(random.nextInt()) % currentWordGroup.size();
        targetWord = currentWordGroup[wordIdx];

        playWordSpatial(targetWord, secondAzimuth);
    }

}

void DualTaskTestController::playMaskingNoise() {
    int maskingDurationMs = config.preSignalDelayMs
                           + (2 * config.signalDurationMs)
                           + config.interSignalDelayMs
                           + config.postSignalMaskingMs;

    float maskingDuration = static_cast<float>(maskingDurationMs) * 0.001f;

    for (float az : config.maskingAzimuths)
        soundEngine.playNoiseSpatial(maskingAmplitude / config.maskingAzimuths.size(), maskingDuration, 0.0f, az);
}

void DualTaskTestController::playWordSpatial(juce::String word, float azimuth) {
    soundEngine.playSampleSpatial(fm->getWordFile(word), 0.0f, azimuth, signalAmplitude, true);
}

void DualTaskTestController::chooseRandomWordGroup() {
    std::vector<juce::String> wordGroups = fm->getWordGroupIds();

    if (wordGroups.size() > 0) {
        size_t idx = static_cast<size_t>(abs(random.nextInt()) % wordGroups.size());
        juce::String currentGroupId = wordGroups[idx];
        currentWordGroup = fm->getWordsInGroup(currentGroupId);
        setWords(currentWordGroup);
    }
}

DualTaskTestController::Config
DualTaskTestController::Config::loadFromFile(const juce::File& file) {
    Config config;

    if (!file.existsAsFile()) {
        DBG("Config file not found.  Using defaults.");
        return config;
    }

    juce::var json = juce::JSON::parse(file);

    if (!json.isObject()) {
        DBG("Invalid JSON format in config. Using defaults.");
        return config;
    }

    auto* root = json.getDynamicObject();
    if (!root) {
        DBG("Invalid JSON format in config. Using defaults.");
        return config;
    }

    if (root->hasProperty("configName")) {
        config.name = root->getProperty("configName").toString();
    }

    if (root->hasProperty("numTrials")) {
        config.numTrials = static_cast<int>(
            root->getProperty("numTrials"));
    }

    if (root->hasProperty("testAzimuths")) {
        auto tonesVar = root->getProperty("testAzimuths");
        if (tonesVar.isArray()) {
            config.testAzimuths.clear();
            for (auto& t : *tonesVar.getArray())
                config.testAzimuths.push_back((float)t);
        }
    }

    if (root->hasProperty("maskingAzimuths")) {
        auto tonesVar = root->getProperty("maskingAzimuths");
        if (tonesVar.isArray()) {
            config.maskingAzimuths.clear();
            for (auto& t : *tonesVar.getArray())
                config.maskingAzimuths.push_back((float)t);
        }
    }


    if (root->hasProperty("signalAmplitudeDb")) {
        config.signalAmplitudeDb = static_cast<float>(
            root->getProperty("signalAmplitudeDb"));
    }

    if (root->hasProperty("maskingAmplitudeDb")) {
        config.maskingAmplitudeDb = static_cast<float>(
            root->getProperty("maskingAmplitudeDb"));
    }

    if (root->hasProperty("signalDurationMs")) {
        config.signalDurationMs = static_cast<int>(
            root->getProperty("signalDurationMs"));
    }

    if (root->hasProperty("interSignalDelayMs")) {
        config.interSignalDelayMs = static_cast<int>(
            root->getProperty("interSignalDelayMs"));
    }

    if (root->hasProperty("preSignalDelayMs")) {
        config.preSignalDelayMs = static_cast<int>(
            root->getProperty("preSignalDelayMs"));
    }

    if (root->hasProperty("postSignalMaskingMs")) {
        config.postSignalMaskingMs = static_cast<int>(
            root->getProperty("postSignalMaskingMs"));
    }

    if (root->hasProperty("interTrialDelayMs")) {
        config.interTrialDelayMs = static_cast<int>(
            root->getProperty("interTrialDelayMs"));
    }

    return config;
}