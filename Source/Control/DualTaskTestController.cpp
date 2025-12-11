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
}

void DualTaskTestController::startTest() {
    setWords(currentWordList);
    setInputsEnabled(true);
}

void DualTaskTestController::stopTest() {
}

void DualTaskTestController::buttonClicked(const juce::String& id) {
}

const DualTaskTestResults DualTaskTestController::getResults() {
    return results;
}

void DualTaskTestController::timerCallback() {
}
