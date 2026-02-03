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

    std::function<void()> onBackClicked;

    void addSetting(const juce::String& settingName, std::function<void()> onSettingClicked);

    void renameSetting(int index, const juce::String& settingName);


private:

    // Model for listbox
    class SettingsListModel : public juce::ListBoxModel {
    public:
        SettingsListModel(SettingsScreen* owner, juce::ListBox& listBox) : owner(owner), listBox(listBox) {
        }

        int getNumRows() override {
            return static_cast<int>(owner->settings.size());
        }

        void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override {
            if (row < 0 || row >= static_cast<int>(owner->settings.size())) return;

            auto& lookAndFeel = listBox.getLookAndFeel();

            juce::Colour bg = lookAndFeel.findColour(juce::TextButton::buttonColourId);

            juce::Colour text = selected
                ? lookAndFeel.findColour(juce::TextButton::textColourOnId)
                : lookAndFeel.findColour(juce::TextButton::textColourOffId);

            g.setColour(juce::Colours::black);
            g.fillRect(0, 0, width, height - 1);

            g.setColour(bg);
            g.fillRect(1, 1, width - 2, height - 3);

            g.setColour(text);
            g.setFont(18.0f);

            g.drawText(owner->settings[row].first, 10, 0, width - 20, height, juce::Justification::centredLeft);
        }

        void selectedRowsChanged(int lastRowSelected) override {
            if (lastRowSelected < 0 || lastRowSelected >= static_cast<int>(owner->settings.size())) return;

            auto& onTestPressed = owner->settings[lastRowSelected].second;

            if (onTestPressed) onTestPressed();
        }

    private:
        SettingsScreen* owner;
        juce::ListBox& listBox;
    };

    juce::ListBox settingsListBox;
    std::unique_ptr<SettingsListModel> listModel;

    juce::TextButton backButton{ "Back" };

    std::vector<std::pair<juce::String, std::function<void()>>> settings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsScreen)
};