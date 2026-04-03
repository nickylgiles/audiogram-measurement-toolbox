/*
  ==============================================================================

    SoundEngineTests.h
    Created: 3 Apr 2026 1:38:42pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Audio/SoundEngine.h"

class SoundEngineTests : public juce::UnitTest {
public:
    SoundEngineTests() : juce::UnitTest("Sound Engine Tests") {}

    void runTest() override {
        beginTest("Source lifetimes");
        {
            SoundEngine engine;
            const double sampleRate = 48000.0;
            const int blockSize = 480; // 10 ms block
            engine.setSampleRate(sampleRate);
            engine.setSamplesPerBlockExpected(blockSize);

            expect(engine.isPlaying() == false, "SoundEngine should not be playing without active sources");

            std::array<float, blockSize>  outputL = {};
            std::array<float, blockSize>  outputR = {};
            engine.processBlock(outputL.data(), outputR.data(), blockSize);

            beginTest("No sources playing");
            for (int i = 0; i < blockSize; ++i) {
                expectWithinAbsoluteError(outputL[i], 0.0f, tolerance, "Output should be silent.");
                expectWithinAbsoluteError(outputR[i], 0.0f, tolerance, "Output should be silent.");
            }

            beginTest("Adding sources");
            float duration = 1.0f;
            engine.playNoise(0.1f, duration, 0);
            engine.playTone(1000.0f, 0.1f, duration, 1);

            engine.processBlock(outputL.data(), outputR.data(), blockSize);

            expect(engine.isPlaying() == true, "Sound sources should be playing.");

            for (int i = 0; i < duration * sampleRate; i += blockSize) {
                engine.processBlock(outputL.data(), outputR.data(), blockSize);
            }

            expect(engine.isPlaying() == false, "SoundEngine should not be playing without active sources");

            beginTest("Clipping");

            bool clipCalled = false;
            engine.onClip = [&](bool clipping) {
                if (clipping)
                    clipCalled = true;
            };
            engine.playTone(1000.0f, 2.0f, 2.0f, 0);
            for (int i = 0; i < sampleRate * 2.0f; i += blockSize) {
                engine.processBlock(outputL.data(), outputR.data(), blockSize);

                if (clipCalled)
                    break;
            }

            expect(clipCalled, "SoundEngine onClip not called");


        }
    }
private:
    float tolerance = 1e-5f;
};

static SoundEngineTests soundEngineTests;