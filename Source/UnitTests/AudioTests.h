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
#include "../Audio/SoundSources/SoundFileSource.h"


class AudioTests : public juce::UnitTest {
public:
    AudioTests() : juce::UnitTest("Audio Tests") {}

    // Basic tests for sound generation
    void runTest() override {
        // Test ToneSource gain and frequency
        beginTest("Tone gain");
        {

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

        }
        
        beginTest("Noise gain");
        {
            float gain = 0.5f;
            double sampleRate = 48000.0;
            float duration = 5.0f;
            int channel = 0;

            NoiseSource noise(sampleRate, gain, duration, channel);

            int numSamples = static_cast<int>(sampleRate * duration);

            std::vector<float> outputL(numSamples, 0.0f);
            std::vector<float> outputR(numSamples, 0.0f);

            noise.process(outputL.data(), outputR.data(), numSamples);

            auto startSample = static_cast<int>(0.1 * sampleRate); // skip envelope attack
            auto endSample = startSample + static_cast<int>(sampleRate * 4.0); // analyse 4 seconds

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

        // Sound file player
        beginTest("Sound File Player");
        {
            double sampleRate = 48000.0;
            float gain = 0.5f;

            int numSamples = 48000;

            juce::Random random(67);

            // Generate white noise
            juce::AudioBuffer<float> reference(1, numSamples);

            for (int i = 0; i < numSamples; ++i) {
                float n = random.nextFloat() * 2.0f - 1.0f;
                reference.setSample(0, i, n);
            }

            // Save to temporary wav file
            juce::File tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("test", ".wav");

            
            juce::WavAudioFormat wav;
            std::unique_ptr<juce::AudioFormatWriter> writer(wav.createWriterFor(new juce::FileOutputStream(tempFile),sampleRate, 1, 16, {}, 0));
            writer->writeFromAudioSampleBuffer(reference, 0, numSamples);
            writer.reset();
            
            SoundFileSource source(sampleRate, tempFile, gain);

            std::vector<float> outputL(numSamples, 0.0f);
            std::vector<float> outputR(numSamples, 0.0f);

            source.process(outputL.data(), outputR.data(), numSamples);

            for (int i = 0; i < numSamples - 1; ++i) {
                float expected = reference.getSample(0, i) * gain;

                expectWithinAbsoluteError(outputL[i], expected, 1e-4f, "Sound file should match reference");
                expectWithinAbsoluteError(outputR[i], expected, 1e-4f, "Sound file should match reference");
            }

            expect(source.isFinished(), "Playback did not finish");
        }
    }
};

static AudioTests audioTests;