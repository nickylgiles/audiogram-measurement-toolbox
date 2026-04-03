/*
  ==============================================================================

    SpeechInNoiseController.cpp
    Created: 29 Nov 2025 1:23:35am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "DigitsInNoiseController.h"
#include "../MainComponent.h"

class MainComponent;

DigitsInNoiseController::DigitsInNoiseController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& configFile)
    : TestController(mainComponentRef, soundEngineRef), timer([this] {timerCallback();})
{
    config = Config::loadFromFile(configFile);

    fm = std::make_unique<SpeechFileManager>();
    currentSequence.resize(config.numDigits);
}

void DigitsInNoiseController::startTest() {
    currentState = TestState::START;
    currentTrial = 0;
    scheduleNextState(1000);

#if JUCE_DEBUG && !BENCHMARKING

    soundEngine.startRecording("din");

#endif
}

void DigitsInNoiseController::stopTest() {
    soundEngine.stop();
    timer.stopTimer();

#if JUCE_DEBUG && !BENCHMARKING

    soundEngine.stopRecording();

#endif
}

void DigitsInNoiseController::buttonClicked(const juce::String& id) {
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

const float DigitsInNoiseController::getSRT() {
    // Average of presented triplets (4th onward)
    float total = 0.0f;
    for (int i = 3; i < trialSNRs.size(); ++i) {
        total += trialSNRs[i];
    }

    return total / (trialSNRs.size() - 3);
}

const SpeechInNoiseTestResults DigitsInNoiseController::getResults() {
    return results;
}

void DigitsInNoiseController::scheduleNextState(int delayMs) {
    timer.stopTimer();
    timer.startTimer(delayMs);
}

void DigitsInNoiseController::setLevels(float snr) {
    const float noiseRefDb = -20.0f;
    const float speechRefDb = -20.0f;
    if (snr < 0.0f) {
        digitAmplitude = dbToAmplitude(noiseRefDb + snr + fm->getDigitNormalisationDb());
        maskingAmplitude = dbToAmplitude(noiseRefDb);
    }
    else {
        digitAmplitude = dbToAmplitude(speechRefDb + fm->getDigitNormalisationDb());
        maskingAmplitude = dbToAmplitude(speechRefDb - snr);
    }
    DBG("Levels set: SNR = " << snr);
    DBG("Digits amplitude = " << digitAmplitude);
    DBG("Masking amplitude = " << maskingAmplitude);
}

void DigitsInNoiseController::makeRandomSequence() {
    for (int i = 0; i < config.numDigits; ++i) {
        currentSequence[i] = abs(random.nextInt() % 10);
    }
}

void DigitsInNoiseController::playDigit(int digit) {
    if (digit < 0 || digit > 9) {
        DBG("Digit out of range.");
        return;
    }
    const auto& res = fm->getDigit(digit);
    soundEngine.playSample(res.data, res.dataSize, digitAmplitude);
}

void DigitsInNoiseController::playMaskingNoise() {
    int maskingDurationMs = config.preDigitDelayMs
                        + (config.numDigits - 1) * (config.interDigitDelayMs + config.interDigitJitterMs)
                        + config.postDigitMaskingMs;

    float maskingDuration = static_cast<float>(maskingDurationMs) * 0.001f;

    soundEngine.playNoise(maskingAmplitude, maskingDuration, 0);
    soundEngine.playNoise(maskingAmplitude, maskingDuration, 1);
}

void DigitsInNoiseController::digitInput(int digit) {
    userInput.push_back(digit);

    if (userInput.size() >= config.numDigits) {
        inputCorrect = true;
        for (int i = 0; i < config.numDigits; ++i) {
            if (userInput[i] != currentSequence[i])
                inputCorrect = false;
        }

        juce::String userStr, correctStr;
        for (int i = 0; i < config.numDigits; ++i) {
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

void DigitsInNoiseController::timerCallback() {
    timer.stopTimer();
    switch (currentState) {
    case DigitsInNoiseController::TestState::START:
        currentSNR = config.dbInitial;
        setLevels(currentSNR);
        trialSNRs.push_back(currentSNR);
        currentTrial = 1;
        currentState = TestState::TRIAL_START;
        timerCallback();
        break;

    case DigitsInNoiseController::TestState::TRIAL_START:
        playMaskingNoise();
        currentDigit = 0;
        makeRandomSequence();
        currentState = TestState::READ_DIGITS;
        scheduleNextState(config.preDigitDelayMs);
        break;

    case DigitsInNoiseController::TestState::READ_DIGITS:
        if (currentDigit < config.numDigits) {
            playDigit(currentSequence[currentDigit]);
            currentDigit++;
            int jitter = static_cast<int>(random.nextFloat() * config.interDigitJitterMs);
            scheduleNextState(config.interDigitDelayMs + jitter);
        }
        else {
            currentState = TestState::AWAIT_RESPONSE;

            setInputsEnabled(true);
            userInput.clear();

            timerCallback();
        }
        break;

    case DigitsInNoiseController::TestState::AWAIT_RESPONSE:
        break;

    case DigitsInNoiseController::TestState::NEXT_TRIAL:
        setInputsEnabled(false);
        trialSNRs.push_back(currentSNR);
        if (inputCorrect) {
            currentSNR -= config.dbIncrementDescending;
        }
        else {
            currentSNR += config.dbIncrementAscending;
        }
        
        setLevels(std::clamp(currentSNR, config.dbMin, config.dbMax));

        if (currentTrial < config.numTrials) {
            ++currentTrial;
            currentState = TestState::TRIAL_START;
            scheduleNextState(config.interTrialDelayMs);
        }
        else {
            currentState = TestState::END;
            timerCallback();
        }
        break;

    case DigitsInNoiseController::TestState::END:
        DBG("Test Complete.  SRT = " << getSRT());
        results.srt = getSRT();
        stopTest();
        if (onTestFinished)
            onTestFinished();
        break;

    default:
        break;
    }
}

DigitsInNoiseController::Config
DigitsInNoiseController::Config::loadFromFile(const juce::File& file) {
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
        config.numTrials = std::max(static_cast<int>(
            root->getProperty("numTrials")), 4); // Must have at least 4 trials to find SRT
    }

    if (root->hasProperty("numDigits")) {
        config.numDigits = static_cast<int>(
            root->getProperty("numDigits"));
    }

    if (root->hasProperty("dbIncrementAscending")) {
        config.dbIncrementAscending = static_cast<float>(
            root->getProperty("dbIncrementAscending"));
    }

    if (root->hasProperty("dbIncrementDescending")) {
        config.dbIncrementDescending = static_cast<float>(
            root->getProperty("dbIncrementDescending"));
    }

    if (root->hasProperty("dbInitial")) {
        config.dbInitial = static_cast<float>(
            root->getProperty("dbInitial"));
    }

    if (root->hasProperty("dbMax")) {
        config.dbMax = static_cast<float>(
            root->getProperty("dbMax"));
    }

    if (root->hasProperty("dbMin")) {
        config.dbMin = static_cast<float>(
            root->getProperty("dbMin"));
    }
    

    if (root->hasProperty("preDigitDelayMs")) {
        config.preDigitDelayMs = static_cast<int>(
            root->getProperty("preDigitDelayMs"));
    }

    if (root->hasProperty("interDigitDelayMs")) {
        config.interDigitDelayMs = static_cast<int>(
            root->getProperty("interDigitDelayMs"));
    }

    if (root->hasProperty("interDigitJitterMs")) {
        config.interDigitJitterMs = static_cast<int>(
            root->getProperty("interDigitJitterMs"));
    }

    if (root->hasProperty("interTrialDelayMs")) {
        config.interTrialDelayMs = static_cast<int>(
            root->getProperty("interTrialDelayMs"));
    }

    if (root->hasProperty("postDigitMaskingMs")) {
        config.postDigitMaskingMs = static_cast<int>(
            root->getProperty("postDigitMaskingMs"));
    }

    return config;
}