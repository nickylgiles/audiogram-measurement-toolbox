/*
  ==============================================================================

    AppLookAndFeel.h
    Created: 2 Feb 2026 11:20:47pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*
Colours should conform to WCAG contrast guidelines:
https://webaim.org/resources/contrastchecker/
*/
class AppLookAndFeel : public juce::LookAndFeel_V4 {
public:
    AppLookAndFeel() {
        setColour(juce::ResizableWindow::backgroundColourId,
            juce::Colour(0xfff5f7fa)); // light grey-blue

        setColour(juce::TextButton::buttonColourId,
            juce::Colour(0xff005fcc)); // dark blue

        setColour(juce::TextButton::buttonOnColourId,
            juce::Colour(0xff20368c)); // darker blue

        setColour(juce::TextButton::textColourOffId,
            juce::Colour(0xffffffff)); // white

        setColour(juce::TextButton::textColourOnId,
            juce::Colour(0xffa0a0a0)); // medium grey

        setColour(juce::Label::textColourId,
            juce::Colour(0xff000000)); // black

        setColour(juce::Slider::textBoxTextColourId,
            juce::Colour(0xff000000));

        // setDefaultSansSerifTypefaceName("MingLiU-ExtB");
    }

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override {
        return juce::Font(
            juce::FontOptions().withHeight(buttonHeight * 0.4f)
        );
    }
    
    juce::Font getLabelFont(juce::Label&) override {
        return juce::Font(
            juce::FontOptions().withHeight(18.0f)
        );
    }

    juce::Font getComboBoxFont(juce::ComboBox&) override {
        return juce::Font(
            juce::FontOptions().withHeight(18.0f)
        );
    }
};