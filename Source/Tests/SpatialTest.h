/*
  ==============================================================================

    SpatialTest.h
    Created: 3 Feb 2026 9:31:29pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "Test.h"
#include "../Control/SpatialTestController.h"
#include "../GUI/Spatial/SpatialTestScreen.h"
#include "../GUI/Spatial/SpatialResultsScreen.h"
#include "../GUI/TestInfoScreen.h"
#include "../MainComponent.h"

class SpatialTest : public Test {
public:
    SpatialTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& testConfigFile);
    void displayInfo() override;
    void startTest() override;
    void exitTest() override;
    void onTestFinish() override;
    static juce::String getName();
private:
    SpatialTestController controller;
};