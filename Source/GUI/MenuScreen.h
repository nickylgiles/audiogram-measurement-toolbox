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

    void addTest(const juce::String& name, std::function<void()> onTestPressed);

    std::function<void()> onSettingsClicked;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:

    // Model for listbox
    class MenuListModel : public juce::ListBoxModel {
    public:
        MenuListModel(MenuScreen* owner, juce::ListBox& listBox) : owner(owner), listBox(listBox) {
        }

        int getNumRows() override {
            return static_cast<int>(owner->tests.size());
        }

        void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override {
            if (row < 0 || row >= static_cast<int>(owner->tests.size())) return;
            
            auto& lookAndFeel = listBox.getLookAndFeel();

            juce::Colour bg = selected
                ? lookAndFeel.findColour(juce::TextButton::buttonOnColourId)
                : lookAndFeel.findColour(juce::TextButton::buttonColourId);

            juce::Colour text = selected
                ? lookAndFeel.findColour(juce::TextButton::textColourOnId)
                : lookAndFeel.findColour(juce::TextButton::textColourOffId);

            g.setColour(juce::Colours::black);
            g.fillRect(0, 0, width, height - 1);

            g.setColour(bg);
            g.fillRect(1, 1, width - 2, height - 3);

            g.setColour(text);
            g.setFont(18.0f);

            g.drawText(owner->tests[row].first, 10, 0, width - 20, height, juce::Justification::centredLeft);
        }

        void listBoxItemClicked(int row, const juce::MouseEvent& event) override {
            if (!event.mouseWasClicked()) return;
            if (row < 0 || row >= static_cast<int>(owner->tests.size())) return;

            auto& onTestPressed = owner->tests[row].second;

            if (onTestPressed) onTestPressed();
        }

    private:
        MenuScreen* owner;
        juce::ListBox& listBox;
    };

    juce::ListBox testListBox;
    std::unique_ptr<MenuListModel> listModel;

    std::vector<std::pair<juce::String, std::function<void()>>> tests;

    juce::TextButton settingsButton{ "Settings" };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuScreen)
};
