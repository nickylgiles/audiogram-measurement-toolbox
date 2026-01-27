/*
  ==============================================================================

    SettingsScreen.h
    Created: 11 Dec 2025 4:43:28am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SettingsScreen : public juce::Component {
public:
    SettingsScreen();
    ~SettingsScreen() override = default;

    void resized() override;

    std::function<void()> onExportClicked;
    std::function<void()> onBackClicked;
    std::function<void()> onChooseWordGroupsJsonClicked;
    
    void setWordGroupsJsonPath(juce::String path);
private:

    juce::TextButton backButton{ "Back" };
    juce::TextButton exportButton{ "Export Results Database" };

    juce::TextButton chooseWordGroupsJsonButton{ "Select word groups JSON file" };
    juce::Label wordGroupsFileLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsScreen)
};