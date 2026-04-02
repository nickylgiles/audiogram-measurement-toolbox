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

SpatialTestController::SpatialTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& configFile)
    	: TestController(mainComponentRef, soundEngineRef), timer([this] {timerCallback();})
{
    // Load config from file provided
    config = Config::loadFromFile(configFile);

    currentState = TestState::END;

    firstAzimuth = 0.0f;
    secondAzimuth = 0.0f;
    moveLeft = false;
    responseLeft = false;

    // Using a fixed SNR for the test
    signalAmplitude = dbToAmplitude(config.signalAmplitudeDb);
    maskingAmplitude = dbToAmplitude(config.maskingAmplitudeDb);
}

void SpatialTestController::startTest() {
    currentState = TestState::START;
    currentTrial = 0;
    scheduleNextState(2000);

#if JUCE_DEBUG

    soundEngine.startRecording("spatial");

#endif
}

void SpatialTestController::stopTest() {
    timer.stopTimer();
    soundEngine.stop();

#if JUCE_DEBUG

    soundEngine.stopRecording();

#endif
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
        response.snr = config.signalAmplitudeDb - config.maskingAmplitudeDb;

        results.responses.push_back(response);

        currentState = TestState::NEXT_TRIAL;
        scheduleNextState(config.interTrialDelayMs);
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
            scheduleNextState(config.preSignalDelayMs);
            break;

        case TestState::TRIAL_START:
            playFirstSound();
            scheduleNextState(config.signalDurationMs);
            currentState = TestState::FIRST_SOUND;
            break;

        case TestState::FIRST_SOUND:
            currentState = TestState::WAIT_BETWEEN_SOUNDS;
            scheduleNextState(config.interSignalDelayMs);
            break;

        case TestState::WAIT_BETWEEN_SOUNDS:
            currentState = TestState::SECOND_SOUND;
            playSecondSound();
            scheduleNextState(config.signalDurationMs);
            break;

        case TestState::SECOND_SOUND:
            currentState = TestState::AWAIT_RESPONSE;
            userResponded = false;
            break;

        case TestState::AWAIT_RESPONSE:
            break;

        case TestState::NEXT_TRIAL:
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

void SpatialTestController::scheduleNextState(int delayMs) {
    timer.stopTimer();
    timer.startTimer(delayMs);
}

void SpatialTestController::playFirstSound() {
    auto idx = static_cast<size_t>(random.nextInt((int)config.testAzimuths.size() ));
    firstAzimuth = config.testAzimuths[idx];
    soundEngine.playNoiseSpatial(signalAmplitude, static_cast<float>(config.signalDurationMs) * 0.001f, 0.0f, firstAzimuth);

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

    soundEngine.playNoiseSpatial(signalAmplitude, static_cast<float>(config.signalDurationMs) * 0.001f, 0.0f, secondAzimuth);

}

void SpatialTestController::playMaskingNoise() {
    int maskingDurationMs = config.preSignalDelayMs + 2 * config.signalDurationMs + config.interSignalDelayMs + config.postSignalMaskingMs;

    float maskingDuration = static_cast<float>(maskingDurationMs) * 0.001f;
    for(float az : config.maskingAzimuths)
        soundEngine.playNoiseSpatial(maskingAmplitude / config.maskingAzimuths.size(), maskingDuration, 0.0f, az);
}


SpatialTestController::Config
SpatialTestController::Config::loadFromFile(const juce::File& file) {
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

    if (root->hasProperty("numTrials")) {
        config.numTrials = static_cast<int>(
            root->getProperty("numTrials"));
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