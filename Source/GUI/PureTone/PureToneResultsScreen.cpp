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
    auto buttonHeight = area.getHeight() / 6;
    area.removeFromTop(buttonHeight * 4);
    exportButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    menuButton.setBounds(area.reduced(10));
}

void PureToneResultsScreen::paint(juce::Graphics& g) {

    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::black);

    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromBottom(bounds.getHeight() / 3  + 30);

    drawAudiogram(g, bounds);

}

void PureToneResultsScreen::drawO(juce::Graphics& g, juce::Point<float> p, float diameter) {
    float r = diameter * 0.5f;

    g.drawEllipse(p.x - r, p.y - r, diameter, diameter, 2.0f);
}

void PureToneResultsScreen::drawX(juce::Graphics& g, juce::Point<float> p, float length) {
    float r = length * 0.5f;

    g.drawLine(p.x - r, p.y - r, p.x + r, p.y + r, 2.0f);
    g.drawLine(p.x - r, p.y + r, p.x + r, p.y - r, 2.0f);
}

void PureToneResultsScreen::drawAudiogram(juce::Graphics& g, juce::Rectangle<int> bounds) {

    bounds.removeFromLeft(80); // for axis labels


    auto h = bounds.getHeight();
    auto w = bounds.getWidth();
    auto x = bounds.getX();
    auto y = bounds.getY();


    float maxF = results.left.rbegin()->first;
    float minF = results.left.begin()->first * 0.5f;

    float minFdB = std::log10(minF);
    float maxFdB = std::log10(maxF);

    // X labels
    g.drawText(juce::translate("Frequency") + juce::String(" (Hz)"),
        x,
        y + h + 10,
        w,
        20,
        juce::Justification::centred
    );

    for (auto r : results.left) {
        float f = r.first;
        float fDb = std::log10(f);

        g.drawText(juce::translate(juce::String(f)),
            x + static_cast<int>( (fDb - minFdB) / (maxFdB - minFdB) * (w - 40) ),
            y + h + 5,
            40,
            10,
            juce::Justification::centred,
            true
        );
    }

    // Y labels
    g.drawText(juce::translate("Level") + juce::String(" (dB)"),
        5,
        y + static_cast<int>(h / 2),
        80,
        20,
        juce::Justification::centred
    );

    for (int i = 0; i <= 5; ++i) {
        g.drawText(juce::String(-i * 10),
            x - 40,
            y + i * static_cast<int>(h / 5),
            40, 
            10,
            juce::Justification::centred,
            true
        );
    }

    g.setColour(juce::Colours::lightgrey);

    for (int i = 0; i <= 5; ++i) {
        float yy = static_cast<float>(y + i * (h / 5.0f));
        g.drawLine(static_cast<float>(x), yy, static_cast<float>(x + w), yy, 1.0f);
    }

    for (auto r : results.left) {
        float f = r.first;
        float xx = static_cast<float>( x + (std::log10(f) - minFdB)
            / (maxFdB - minFdB) * w);

        g.drawLine(xx, static_cast<float>(y), xx, static_cast<float>(y + h), 1.0f);
    }

    g.setColour(juce::Colours::black);
    g.drawLine(
        static_cast<float>(x), 
        static_cast<float>(y + h), 
        static_cast<float>(x + w), 
        static_cast<float>(y + h)
    ); // x-axis

    g.drawLine(
        static_cast<float>(x),
        static_cast<float>(y), 
        static_cast<float>(x), 
        static_cast<float>(y + h)
    ); // y-axis

    // Data points 
    float prevX = 0.0f;
    float prevY = 0.0f;

    bool connectPrev = false;
    // Left: Blue X
    for (auto r : results.left) {
        float f = r.first;
        float t = r.second;
        float pX = x + (std::log10(f) - minFdB) / (maxFdB - minFdB) * w;
        float pY = y + h * (1.0f + t / 50.0f);
        g.setColour(juce::Colours::blue);
        drawX(g, juce::Point<float>(pX, pY), 15.0f);

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
    // Right: Red O
    for (auto r : results.right) {
        float f = r.first;
        float t = r.second;
        float pX = x + (std::log10(f) - std::log10(minF)) / (std::log10(maxF) - std::log10(minF)) * (w);
        float pY = y + h * (1.0f + t / 50.0f);
        g.setColour(juce::Colours::red);
        drawO(g, juce::Point<float>(pX, pY), 15.0f);

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