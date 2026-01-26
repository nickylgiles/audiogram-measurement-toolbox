/*
  ==============================================================================

    DualTaskTestController.cpp
    Created: 4 Dec 2025 12:30:20pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "DualTaskTestController.h"

DualTaskTestController::DualTaskTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef) 
    : TestController(mainComponentRef, soundEngineRef) {

    fm = std::make_unique<SpeechFileManager>();
}

void DualTaskTestController::startTest() {
    std::vector<juce::String> wordGroups = fm->getWordGroupIds();
    DBG(wordGroups.size() << " word groups present.");
    size_t idx = static_cast<size_t>(abs(random.nextInt()) % wordGroups.size());
    juce::String currentGroup = wordGroups[idx];
    std::vector<juce::String> words = fm->getWordsInGroup(currentGroup);
    setWords(words);
    setInputsEnabled(true);

    size_t correctIndex = abs(random.nextInt()) % words.size();
    juce::String correctWord = words[correctIndex];
    DBG("Correct word is " << correctWord);

    soundEngine.playSampleSpatial(fm->getWordFile(correctWord), 0.0f, -90.0f, 1.0f, true);
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
}

const DualTaskTestResults DualTaskTestController::getResults() {
    return results;
}

void DualTaskTestController::timerCallback() {
}
