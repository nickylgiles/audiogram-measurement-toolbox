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

    std::vector<std::vector<juce::dsp::Complex<float>>> X_fdl;
    std::vector<std::vector<juce::dsp::Complex<float>>> H;

    std::vector<juce::dsp::Complex<float>> Y;
    
    std::vector<juce::dsp::Complex<float>> yifft;
    
    std::vector<juce::dsp::Complex<float>> x_tdl;

    int NFFT;
    int partSize;

    int hParts;

    std::vector<float> inputBuffer;

    std::vector<float> leftoverOutput;

    std::vector<float> outputBuf; // temp buffer for output of convolution
    std::vector<juce::dsp::Complex<float>> tempFFTBuffer; // temp buffer for loading IRs
    int leftoverPos = 0;

};