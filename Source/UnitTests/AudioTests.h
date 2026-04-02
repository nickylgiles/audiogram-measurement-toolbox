/*
  ==============================================================================

    AudioTests.h
    Created: 2 Apr 2026 12:56:22am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Audio/SoundSources/ToneSource.h"
#include "../Audio/SoundSources/NoiseSource.h"

class AudioTests : public juce::UnitTest {
public:
    AudioTests() : juce::UnitTest("Audio Tests") {}

    // Basic tests for sound generation
    void runTest() override {
        // Test ToneSource gain and frequency
        beginTest("Tone gain");

        float frequency = 480.0f;
        double sampleRate = 48000.0;
        float gain = 0.5f;
        int channel = 0;
        float duration = 5.0f;

        ToneSource tone(sampleRate, frequency, gain, duration, channel); 
        
        int numSamples = static_cast<int>(sampleRate * duration);
        float maxSample = 0.0f;

        std::vector<float> outputL(numSamples, 0.0f);
        std::vector<float> outputR(numSamples, 0.0f);

        tone.process(outputL.data(), outputR.data(), numSamples);

        for (int i = 0; i < numSamples; ++i) {
            maxSample = std::max(maxSample, std::abs(outputL[i]));
            expectWithinAbsoluteError(outputR[i], 0.0f, 1e-4f, "ToneSource opposite channel not empty");
        }
        
        logMessage("ToneSource gain = " + juce::String(maxSample) + juce::String(". Expected = ") + juce::String(gain));

        expectWithinAbsoluteError(maxSample, gain, 1e-4f, "ToneSource gain incorrect");
        logMessage("ToneSource test 1 passed");


        // Test frequency by counting zero crossings
        beginTest("Tone frequency");
        int startSample = static_cast<int>(0.1 * sampleRate); // skip envelope attack
        int endSample = startSample + static_cast<int>(sampleRate * 4.0);
        int zeroCrossings = 0;

        for (int i = startSample + 1; i < endSample; ++i) {
            if ((outputL[i - 1] >= 0.0f && outputL[i] < 0.0f) ||
                (outputL[i - 1] < 0.0f && outputL[i] >= 0.0f)) {
                zeroCrossings++;
            }
        }

        float estFreq = (zeroCrossings / 2.0f) * (sampleRate / (endSample - startSample));
        
        logMessage("Expected frequency = " + juce::String(frequency));
        logMessage("Zero-crossings estimated frequency = " + juce::String(estFreq));

        expectWithinAbsoluteError(estFreq, frequency, 0.1f, "ToneSource frequency incorrect");
        logMessage("ToneSource test 2 passed");



        beginTest("Noise gain");

        gain = 0.5f;
        sampleRate = 48000.0;
        duration = 5.0f;
        channel = 0;

        NoiseSource noise(sampleRate, gain, duration, channel);

        numSamples = static_cast<int>(sampleRate * duration);

        outputL.resize(numSamples);
        outputR.resize(numSamples);

        noise.process(outputL.data(), outputR.data(), numSamples);

        startSample = static_cast<int>(0.1 * sampleRate); // skip envelope attack
        endSample = startSample + static_cast<int>(sampleRate * 4.0); // analyse 4 seconds

        double sumSquares = 0.0;

        for (int i = startSample; i < endSample; ++i) {
            float s = outputL[i];
            sumSquares += s * s;
        }

        float rms = std::sqrt(sumSquares / (endSample - startSample));


        float expectedRMS = gain / std::sqrt(3.0f);

        logMessage("Measured RMS = " + juce::String(rms));
        logMessage("Expected RMS = " + juce::String(expectedRMS));

        expectWithinAbsoluteError(rms, expectedRMS, 0.01f, "NoiseSource does not match expected energy");
    }
};

static AudioTests audioTests;