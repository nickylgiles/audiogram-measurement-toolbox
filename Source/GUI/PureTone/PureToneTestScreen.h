/*
  ==============================================================================

    PureToneTestScreen.h
    Created: 4 Nov 2025 7:24:17pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PureToneTestScreen : public juce::Component {
public:
    PureToneTestScreen();
    ~PureToneTestScreen() override = default;


    // Callbacks set in MainComponent
    std::function<void()> onHearClicked;
    std::function<void()> onStopClicked;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::TextButton hearButton{ "I hear this" };
    juce::TextButton stopButton{ "Stop Test" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PureToneTestScreen)
};