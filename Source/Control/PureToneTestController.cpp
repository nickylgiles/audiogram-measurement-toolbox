/*
  ==============================================================================

    PureToneTestController.cpp
    Created: 7 Nov 2025 4:33:08pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "PureToneTestController.h"
#include "../MainComponent.h"

PureToneTestController::PureToneTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& configFile)
    : TestController(mainComponentRef, soundEngineRef),
      timer([this] {timerCallback();})
{
    // Load config from file provided
    config = Config::loadFromFile(configFile);

    currentState = TestState::END;
    for (auto tone : config.testTones) {
        toneThresholds[0][tone] = config.dbLevelMax;
        toneThresholds[1][tone] = config.dbLevelMax;
    }
}

void PureToneTestController::startTest() {
    currentTone = 0;
    currentEar = 0;
    currentThreshold = config.dbLevelMin;

    currentToneDetected = false;
    currentState = TestState::START;

    scheduleNextTone(config.toneDelayMs);
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
                if (currentThreshold > config.dbLevelMin && !floatsEqual(currentThreshold, config.dbLevelMin))
                    currentState = TestState::DB_DESCENDING;
                else
                    currentState = TestState::NEXT_TONE;
                toneThresholds[currentEar][config.testTones[currentTone]] = currentThreshold;
            }
            else {
                currentThreshold += config.dbIncrementAscending;
                if (currentThreshold > config.dbLevelMax && !floatsEqual(currentThreshold, config.dbLevelMax)) {
                    currentState = TestState::NEXT_TONE;
                }
            }
            break;

        case TestState::DB_DESCENDING:
            if (currentToneDetected) {
                toneThresholds[currentEar][config.testTones[currentTone]] = currentThreshold;
                if (currentThreshold > config.dbLevelMin && !floatsEqual(currentThreshold, config.dbLevelMin)) {
                    currentThreshold -= config.dbIncrementDescending;
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
            if (currentTone < (config.testTones.size() - 1)) {
                currentTone++;
                currentState = TestState::DB_ASCENDING;
                currentThreshold = config.dbLevelMin;
            }
            else {
                if (currentEar < 1) {
                    currentEar = 1;
                    currentState = TestState::DB_ASCENDING;
                    currentThreshold = config.dbLevelMin;
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
        scheduleNextTone(config.toneDelayMs);
    }
    if (currentState == TestState::NEXT_TONE) {
        scheduleNextTone(0);
    }
    if (currentState == TestState::END) {
        scheduleNextTone(0);
    }

    
}

void PureToneTestController::playCurrentTone() {

    soundEngine.playToneMasked(config.testTones[currentTone], dbToAmplitude(currentThreshold), 1.0f, currentEar);
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

PureToneTestController::Config
PureToneTestController::Config::loadFromFile(const juce::File& file) {
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
    
    if (root->hasProperty("testTones")) {
        auto tonesVar = root->getProperty("testTones");
        if (tonesVar.isArray()) {
            config.testTones.clear();
            for (auto& t : *tonesVar.getArray())
                config.testTones.push_back((float)t);
        }
    }

    if (root->hasProperty("dbLevelMin")) {
        config.dbLevelMin = static_cast<float>(
            root->getProperty("dbLevelMin"));
    }

    if (root->hasProperty("dbLevelMax")) {
        config.dbLevelMax = static_cast<float>(
            root->getProperty("dbLevelMax"));
    }

    if (root->hasProperty("dbIncrementAscending")) {
        config.dbIncrementAscending = static_cast<float>(
            root->getProperty("dbIncrementAscending"));
    }

    if (root->hasProperty("dbIncrementDescending")) {
        config.dbIncrementDescending = static_cast<float>(
            root->getProperty("dbIncrementDescending"));
    }

    if (root->hasProperty("toneDelayMs")) {
        config.toneDelayMs = static_cast<int>(
            root->getProperty("toneDelayMs"));
    }

    return config;
}
