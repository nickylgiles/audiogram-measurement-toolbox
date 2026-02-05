/*
  ==============================================================================

    PureToneTest.h
    Created: 3 Feb 2026 9:31:18pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "Test.h"
#include "../Control/PureToneTestController.h"
#include "../GUI/PureTone/PureToneTestScreen.h"
#include "../GUI/PureTone/PureToneResultsScreen.h"
#include "../GUI/TestInfoScreen.h""
#include "../MainComponent.h"

class PureToneTest : public Test {
public:
    PureToneTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef);
    void displayInfo() override;
    void startTest() override;
    void exitTest() override;
    void onTestFinish() override;
    static juce::String getName();
private:
    PureToneTestController controller;
};