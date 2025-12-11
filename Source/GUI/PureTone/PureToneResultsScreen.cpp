/*
  ==============================================================================

    PureToneResultsScreen.cpp
    Created: 4 Nov 2025 7:24:41pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "PureToneResultsScreen.h"

PureToneResultsScreen::PureToneResultsScreen() {
    addAndMakeVisible(exportButton);
    exportButton.onClick = [this] {if (onExportClicked) onExportClicked(); };

    addAndMakeVisible(menuButton);
    menuButton.onClick = [this] {if (onMenuClicked) onMenuClicked(); };
}

void PureToneResultsScreen::setResults(const PureToneTestResults& newResults) {
    results = newResults;
}

void PureToneResultsScreen::resized() {
    auto area = getLocalBounds().reduced(20);
    auto buttonHeight = area.getHeight() / 4;
    area.removeFromTop(buttonHeight * 2);
    exportButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    menuButton.setBounds(area.reduced(10));
}

void PureToneResultsScreen::paint(juce::Graphics& g) {

    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::black);

    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromBottom(bounds.getHeight() / 2  + 30);

    bounds.removeFromLeft(30);


    auto h = bounds.getHeight();
    auto w = bounds.getWidth();
    auto x = bounds.getX();
    auto y = bounds.getY();

    g.drawLine(x, y + h, x + w, y + h); // x-axis
    g.drawLine(x, y, x, y + h); // y-axis

    float maxF = results.left.rbegin()->first;
    float minF = results.left.begin()->first * 0.5f;

    // X labels
    for (auto r : results.left) {
        float f = r.first;
        g.drawText(juce::String(f), x + (std::log10(f)-std::log10(minF)) / (std::log10(maxF)-std::log10(minF))*(w-40), y + h, 40, 10, juce::Justification::centred, true);
    }

    // Y labels
    for (int i = 0; i <= 5; ++i) {
        g.drawText(juce::String(-i * 10), x-40, y + i * (h / 5), 40, 10, juce::Justification::centred, true);
    }

    // Data points 
    float prevX, prevY;
    bool connectPrev = false;
    for (auto r : results.left) {
        float f = r.first;
        float t = r.second;
        float pX = x + (std::log10(f) - std::log10(minF)) / (std::log10(maxF) - std::log10(minF)) * (w);
        float pY = y + h * (1.0f + t / 50.0f);
        g.setColour(juce::Colours::red);
        g.fillEllipse(pX - 3, pY - 3, 6, 6);
        if (connectPrev) {
            juce::Point<float> p1(pX, pY);
            juce::Point<float> p2(prevX, prevY);
            g.drawLine(juce::Line<float>(p1, p2), 2.0f);
        }
        prevX = pX;
        prevY = pY;
        connectPrev = true;
    }

    connectPrev = false;
    for (auto r : results.right) {
        float f = r.first;
        float t = r.second;
        float pX = x + (std::log10(f) - std::log10(minF)) / (std::log10(maxF) - std::log10(minF)) * (w);
        float pY = y + h * (1.0f + t / 50.0f);
        g.setColour(juce::Colours::blue);
        g.fillEllipse(pX - 3, pY - 3, 6, 6);
        if (connectPrev) {
            juce::Point<float> p1(pX, pY);
            juce::Point<float> p2(prevX, prevY);
            g.drawLine(juce::Line<float>(p1, p2), 2.0f);
        }
        prevX = pX;
        prevY = pY;
        connectPrev = true;
    }
}
