/*
  ==============================================================================

    PartitionedConvolver.h
    Created: 25 Nov 2025 7:42:14pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PartitionedConvolver {
public:
    PartitionedConvolver(int partitionSize);
    void loadIR(const juce::AudioBuffer<float>& irBuffer);
    void processBlock(const float* input, float* output, int numSamples);

private:

    void processPartition(float* inputPart, float* outputPart);

    std::unique_ptr<juce::dsp::FFT> fft;

    static int computeFFTOrder(int N) {
        int order = 0;
        while ((1 << order) < N)
            ++order;
        return order;
    }

    int NFFT;
    int partSize;

    std::vector<std::vector<juce::dsp::Complex<float>>> inputFDL;
    int inputFDLidx = 0;
    std::vector<juce::dsp::Complex<float>> inputTDL;

    std::vector<std::vector<juce::dsp::Complex<float>>> irPartsFFT;
    int numIRParts;

    std::vector<juce::dsp::Complex<float>> outputFFT;
    std::vector<juce::dsp::Complex<float>> outputIFFT;

    std::vector<float> inputBuffer;
    std::vector<float> leftoverOutput;
    int leftoverPos = 0;

    std::vector<float> outputBuffer; // temp buffer for output of convolution
    std::vector<juce::dsp::Complex<float>> tempFFTBuffer; // temp buffer for loading IRs
};