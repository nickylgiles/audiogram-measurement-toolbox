/*
  ==============================================================================

    DualTaskTest.h
    Created: 3 Feb 2026 9:31:50pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "Test.h"
#include "../Control/DualTaskTestController.h"
#include "../GUI/DualTask/DualTaskTestScreen.h"
#include "../GUI/DualTask/DualTaskResultsScreen.h"
#include "../GUI/TestInfoScreen.h""
#include "../MainComponent.h"

class DualTaskTest : public Test {
public:
    DualTaskTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& testConfigFile);
    void displayInfo() override;
    void startTest() override;
    void exitTest() override;
    void onTestFinish() override;
    static juce::String getName();
private:
    DualTaskTestController controller;
};