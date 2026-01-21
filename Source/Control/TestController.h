/*
  ==============================================================================

    TestController.h
    Created: 30 Oct 2025 2:34:24am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Audio/SoundEngine.h"

class MainComponent;

class TestController {
public:
    TestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef)
        : mainComponent(mainComponentRef), soundEngine(soundEngineRef) {}

    virtual ~TestController() = default;

    virtual void startTest() = 0;
    virtual void stopTest() = 0;

    virtual void buttonClicked(const juce::String& id) { return; }

    static float dbToAmplitude(float db) {
        return static_cast<float>(
            std::pow( 10.0, db / 20.0));
    }

protected:
    MainComponent& mainComponent;
    SoundEngine& soundEngine;
};