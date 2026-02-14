/*
  ==============================================================================

    AudiogramViewer.cpp
    Created: 14 Feb 2026 11:30:49am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "AudiogramViewer.h"

void AudiogramViewer::setResults(const PureToneTestResults& newResults) {
    results = newResults;
}

void AudiogramViewer::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::black);

    auto bounds = getLocalBounds().reduced(15);

    drawAudiogram(g, bounds);
}

void AudiogramViewer::drawAudiogram(juce::Graphics& g, juce::Rectangle<int> bounds) {

    bounds.removeFromLeft(80); // for axis labels
    bounds.removeFromRight(80); // for legend
    bounds.removeFromBottom(30);

    auto h = bounds.getHeight();
    auto w = bounds.getWidth();
    auto x = bounds.getX();
    auto y = bounds.getY();

    drawLegend(g, juce::Rectangle<int>(
        x + w + 5, y + 40, 80, 60
    ));

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
            x + static_cast<int>((fDb - minFdB) / (maxFdB - minFdB) * (w - 40)),
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
        float xx = static_cast<float>(x + (std::log10(f) - minFdB)
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

void AudiogramViewer::drawO(juce::Graphics& g, juce::Point<float> p, float diameter) {
    float r = diameter * 0.5f;

    g.drawEllipse(p.x - r, p.y - r, diameter, diameter, 2.0f);
}

void AudiogramViewer::drawX(juce::Graphics& g, juce::Point<float> p, float length) {
    float r = length * 0.5f;

    g.drawLine(p.x - r, p.y - r, p.x + r, p.y + r, 2.0f);
    g.drawLine(p.x - r, p.y + r, p.x + r, p.y - r, 2.0f);
}

void AudiogramViewer::drawLegend(juce::Graphics& g, juce::Rectangle<int> bounds) {
    float sz = 15.0f;

    g.drawRect(bounds.toFloat(), 1.0f);

    juce::Point<float> xPos;
    juce::Point<float> oPos;

    xPos.setX(static_cast<float>(bounds.getX()) + sz + 5);
    xPos.setY(static_cast<float>(bounds.getY()) + bounds.getHeight() * 0.25f);

    oPos.setX(static_cast<float>(bounds.getX()) + sz + 5);
    oPos.setY(static_cast<float>(bounds.getY()) + bounds.getHeight() * 0.75f);
    
    g.setColour(juce::Colours::blue);
    drawX(g, xPos, sz);

    g.setColour(juce::Colours::red);
    drawO(g, oPos, sz);

    juce::Rectangle<float> leftTextBounds;
    juce::Rectangle<float> rightTextBounds;
    float textX = static_cast<float>(bounds.getX()) + 2.0f * sz + 10;

    leftTextBounds.setBounds(
        textX,
        bounds.getY(),
        bounds.getRight() - textX,
        bounds.getHeight() * 0.5f
    );

    rightTextBounds.setBounds(
        textX,
        bounds.getY() + bounds.getHeight() * 0.5f,
        bounds.getRight() - textX,
        bounds.getHeight() * 0.5f
    );
    
    g.setColour(juce::Colours::black);
    g.drawText(juce::translate("Left"), leftTextBounds, juce::Justification::centredLeft);
    g.drawText(juce::translate("Right"), rightTextBounds, juce::Justification::centredLeft);
}
