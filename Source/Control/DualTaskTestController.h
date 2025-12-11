/*
  ==============================================================================

    DualTaskTestController.h
    Created: 4 Dec 2025 12:30:20pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TestController.h"
#include "../Results/DualTaskTestResults.h"

class DualTaskTestController : public TestController, private juce::Timer {
public:
	DualTaskTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef);
	void startTest() override;
	void stopTest() override;

	void buttonClicked(const juce::String& id) override;

	const DualTaskTestResults getResults();

	std::function<void(bool)> setInputsEnabled;
	std::function<void(const std::vector<juce::String>&)> setWords;
private:
	void timerCallback() override;

	std::vector<juce::String> currentWordList = { "ant", "bat", "cart", "door" };
	DualTaskTestResults results;
};