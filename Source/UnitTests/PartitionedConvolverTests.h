/*
  ==============================================================================

    PartitionedConvolverTests.h
    Created: 2 Apr 2026 1:53:31pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Audio/Spatial/PartitionedConvolver.h"

class PartitionedConvolverTests : public juce::UnitTest {
public:
    PartitionedConvolverTests() : juce::UnitTest("Partitioned Convolver Tests") {}

    void runTest() override {
        // Test with delayed impulse IR on sine
        beginTest("Impulse response");
        {
            const int partSize = 128;
            const int irLength = 256;
            PartitionedConvolver conv(partSize);

            juce::AudioBuffer<float> ir(1, irLength);
            ir.clear();
            ir.setSample(0, 10, 1.0f); // impulse at sample 10

            conv.loadIR(ir);

            const int totalSamples = irLength * 4;

            std::vector<float> input(totalSamples, 0.0f);

            for (int i = 0; i < totalSamples; ++i) {
                input[i] = 0.5f * std::sin(i * 0.05);
            }

            std::vector<float> output(totalSamples, 0.0f);
            conv.processBlock(input.data(), output.data(), totalSamples);

            const int latency = 10;

            for (int i = partSize; i < irLength; ++i) {
                expectWithinAbsoluteError(output[i + latency], input[i], tolerance,
                    "Output sample " + juce::String(i) + " should match IR");
            }
        }

        // Test on random noise vs naive convolution in TD
        // Compare processing time
        beginTest("White Noise");
        {
            const int partSize = 256;
            const int irLength = 1024;
            const int totalSamples = 4096;

            PartitionedConvolver conv(partSize);
            juce::Random random(67);

            juce::AudioBuffer<float> ir(1, irLength);
            for (int i = 0; i < irLength; ++i)
                ir.setSample(0, i, random.nextFloat() * 2.0f - 1.0f);

            conv.loadIR(ir);

            std::vector<float> input(totalSamples);
            for (int i = 0; i < totalSamples; ++i)
                input[i] = random.nextFloat() * 2.0f - 1.0f;

            std::vector<float> output(totalSamples, 0.0f);
            juce::int64 pcStart = juce::Time::getHighResolutionTicks();
            conv.processBlock(input.data(), output.data(), totalSamples);
            juce::int64 pcFinish = juce::Time::getHighResolutionTicks();

            double pcProcessingTimeMs = juce::Time::highResolutionTicksToSeconds(pcFinish - pcStart) * 1000.0;

            std::vector<float> reference(totalSamples, 0.0f);

            // Naive time-domain convolution
            juce::int64 tdStart = juce::Time::getHighResolutionTicks();
            for (int n = 0; n < totalSamples; ++n) {
                float sum = 0.0f;

                for (int k = 0; k < irLength; ++k) {
                    int inputIndex = n - k;

                    if (inputIndex >= 0 && inputIndex < totalSamples)
                        sum += input[inputIndex] * ir.getSample(0, k);
                }

                reference[n] = sum;
            }
            juce::int64 tdFinish = juce::Time::getHighResolutionTicks();

            double tdProcessingTimeMs = juce::Time::highResolutionTicksToSeconds(tdFinish - tdStart) * 1000.0;

            for (int i = partSize; i < totalSamples; ++i) {
                expectWithinAbsoluteError(output[i], reference[i], tolerance,
                    "Mismatch at sample " + juce::String(i));
            }

            for (int i = partSize; i < totalSamples; ++i) {
                expectWithinAbsoluteError(output[i], reference[i], tolerance,
                    "Mismatch at sample " + juce::String(i));
            }

            // Get SNR of convolution vs error
            beginTest("White Noise SNR");
            double signalEnergy = 0.0;
            double errorEnergy = 0.0;

            for (int i = 0; i < totalSamples; ++i) {
                double signal = reference[i];
                double error = output[i] - reference[i];

                signalEnergy += signal * signal;
                errorEnergy += error * error;
            }

            double snr = 10.0 * std::log10(signalEnergy / errorEnergy);

            DBG("SNR = " << snr << " dB");

            expect(snr > 120.0, "SNR too low: " + juce::String(snr) + " dB");

            DBG("Processing Time");
            DBG("Time-domain: " << tdProcessingTimeMs << " ms");
            DBG("Partitioned convolver: " << pcProcessingTimeMs << " ms");

        }

    }
private:
    const float tolerance = 1e-4f;

};

static PartitionedConvolverTests partitionedConvolverTests;