/*
  ==============================================================================

    TestCalibrationTest.h
    Created: 25 Mar 2026 2:16:28pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "Test.h"
#include "../GUI/Calibration/TestCalibrationScreen.h"
#include "../MainComponent.h"
#include "../Audio/Calibration/HTL.h"

class TestCalibrationTest : public Test {
public:
    TestCalibrationTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef);
    void displayInfo() override;
    void startTest() override;
    void exitTest() override;
    void onTestFinish() override;
    static juce::String getName();
private:
    enum class ToneChannel { LEFT, RIGHT, BOTH };
    ToneChannel channel = ToneChannel::RIGHT;

    enum class Stimulus { PURETONE, NOISE };
    Stimulus stimulus = Stimulus::PURETONE;

    SoundEngine& soundEngine;

    bool playing = false;
    void updateTone();
    void stopTone();

    bool htl = false;
    
    float frequency = 1000.0f;
    float level = 0.0f;
};
