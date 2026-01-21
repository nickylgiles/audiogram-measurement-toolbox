/*
  ==============================================================================

    ToneGenerator.cpp
    Created: 29 Oct 2025 3:46:17pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "ToneGenerator.h"

ToneGenerator::ToneGenerator() {
    currentPhase = 0.0;
    frequency = 0.0f;
    amplitude = 0.0f;
    sampleRate = 44100.0;
}

void ToneGenerator::reset() {
    currentPhase = 0.0;
}

void ToneGenerator::setAmplitude(float newAmplitude) {
    amplitude = newAmplitude;
}

void ToneGenerator::setFrequency(float newFrequency) {
    frequency = newFrequency;
}

float ToneGenerator::nextSample() {
    float sample = static_cast<float>(amplitude * std::sin(currentPhase));
    double phaseIncrement = (juce::MathConstants<double>::twoPi * frequency) / sampleRate;
    currentPhase += phaseIncrement;

    if (currentPhase >= juce::MathConstants<double>::twoPi) {
        currentPhase -= juce::MathConstants<double>::twoPi;
    }

    return sample;
}

void ToneGenerator::setSampleRate(double newSampleRate) {
    sampleRate = newSampleRate;
}
