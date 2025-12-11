/*
  ==============================================================================

    MenuScreen.h
    Created: 4 Nov 2025 7:25:01pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MenuScreen : public juce::Component {
public:
    MenuScreen();
    ~MenuScreen() override = default;


    // Callbacks set in MainComponent
    std::function<void()> onPureToneClicked;
    std::function<void()> onSpatialClicked;
    std::function<void()> onSpeechInNoiseClicked;
    std::function<void()> onDualTaskClicked;

    std::function<void()> onSettingsClicked;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:

    juce::TextButton pureToneButton{ "Pure Tone Test" };
    juce::TextButton spatialButton{ "Spatialization Test" };
    juce::TextButton speechInNoiseButton{ "Speech-in-noise Test" };
    juce::TextButton dualTaskButton{ "Dual-task (spatial + speech) Test" };

    juce::TextButton settingsButton{ "Settings" };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuScreen)
};
