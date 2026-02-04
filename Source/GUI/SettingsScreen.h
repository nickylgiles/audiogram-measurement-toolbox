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

    // Return the index of the added setting
    size_t addButtonSetting(const juce::String& settingName, std::function<void()> onSettingClicked);
    size_t addTextSetting(const juce::String& settingName, std::function<void(const juce::String&)> onTextChanged, const juce::String& defaultValue = "");


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

            auto& setting = owner->settings[row];

            auto& lookAndFeel = listBox.getLookAndFeel();

            juce::Colour bg = lookAndFeel.findColour(juce::TextButton::buttonColourId);

            juce::Colour text = lookAndFeel.findColour(juce::TextButton::textColourOffId);

            g.setColour(juce::Colours::black);
            g.fillRect(0, 0, width, height - 1);

            g.setColour(bg);
            g.fillRect(1, 1, width - 2, height - 3);

            g.setColour(text);
            g.setFont(18.0f);

            g.drawText(owner->settings[row].name, 10, 0, 150, height, juce::Justification::centredLeft);
        }

        void selectedRowsChanged(int lastRowSelected) override {
            if (lastRowSelected < 0 || lastRowSelected >= static_cast<int>(owner->settings.size())) return;

            if (owner->settings[lastRowSelected].type == Setting::Type::Button) {
                auto& onSettingPressed = owner->settings[lastRowSelected].onClick;
                if (onSettingPressed) onSettingPressed();
            }

            // Resize all text editor bars to the correct size
            for (auto& s : owner->settings) {
                if (s.type == Setting::Type::TextInput && s.editorComponent != nullptr) {
                    s.editorComponent->setBounds(160, 5, owner->settingsListBox.getWidth() - 170, 40);
                }
            }
            
        }

        juce::Component* refreshComponentForRow(int row, bool, juce::Component* existing) override {
            if (row < 0 || row >= static_cast<int>(owner->settings.size()))
                return nullptr;

            auto& setting = owner->settings[row];
            
            if (setting.type == Setting::Type::Button) {
                return existing;
            }

            if (setting.type == Setting::Type::TextInput) {
                juce::TextEditor* editor = nullptr;

                if (setting.editorComponent != nullptr) {
                    editor = setting.editorComponent;
                }
                else {
                    editor = new juce::TextEditor();
                    setting.editorComponent = editor;
                }

                editor->setText(setting.textValue);
                
                editor->onTextChange = [&setting, editor]() {
                    setting.textValue = editor->getText();
                    if (setting.onTextChanged)
                        setting.onTextChanged(setting.textValue);
                    };

                return editor;
            }
        }

    private:
        SettingsScreen* owner;
        juce::ListBox& listBox;
    };

    struct Setting {
        enum class Type { Button, TextInput } type;
        juce::String name;
        std::function<void()> onClick;
        juce::String textValue;
        std::function<void(const juce::String&)> onTextChanged;
        juce::TextEditor* editorComponent = nullptr;
    };

    juce::ListBox settingsListBox;
    std::unique_ptr<SettingsListModel> listModel;

    juce::TextButton backButton{ "Back" };

    std::vector<Setting> settings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsScreen)
};