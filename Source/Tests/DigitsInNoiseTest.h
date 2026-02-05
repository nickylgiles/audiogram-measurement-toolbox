/*
  ==============================================================================

    DigitsInNoiseTest.h
    Created: 3 Feb 2026 9:31:42pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "Test.h"
#include "../Control/DigitsInNoiseController.h"
#include "../GUI/SpeechInNoise/SpeechInNoiseTestScreen.h"
#include "../GUI/SpeechInNoise/SpeechInNoiseResultsScreen.h"
#include "../GUI/TestInfoScreen.h""
#include "../MainComponent.h"

class DigitsInNoiseTest : public Test {
public:
    DigitsInNoiseTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef);
    void displayInfo() override;
    void startTest() override;
    void exitTest() override;
    void onTestFinish() override;
    static juce::String getName();
private:
    DigitsInNoiseController controller;
};