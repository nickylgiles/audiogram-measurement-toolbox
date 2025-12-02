/*
  ==============================================================================

    PartitionedConvolver.cpp
    Created: 25 Nov 2025 7:42:14pm
    Author:  nicky_hgjk9m6

    Implementation based on Uniformly Partitioned Overlap-Save (UPOLS)

  ==============================================================================
*/

#include "PartitionedConvolver.h"

PartitionedConvolver::PartitionedConvolver(int partitionSize)
    : partSize(partitionSize)
{
    NFFT = 2 * partSize;
    int fftOrder = computeFFTOrder(NFFT);
    fft = std::make_unique<juce::dsp::FFT>(fftOrder);

    x_tdl.resize(NFFT, 0.0f);

    Y.resize(NFFT, {0.0f, 0.0f});

    yifft.resize(NFFT, 0.0f);

    inputBuffer.clear();
}

void PartitionedConvolver::loadIR(const juce::AudioBuffer<float>& irBuffer) {
    int length = irBuffer.getNumSamples();
    hParts = (length + partSize - 1) / partSize;

    H.clear();
    H.resize(hParts);

    for (int i = 0; i < hParts; ++i) {
        H[i].resize(NFFT, {0.0f, 0.0f});
    }

    auto* irReadPtr = irBuffer.getReadPointer(0);

    // Zero-pad parts and perform FFT
    std::vector<juce::dsp::Complex<float>> tempBuf;
    tempBuf.resize(NFFT, {0.0f, 0.0f});

    for (int p = 0; p < hParts; ++p) {
        for (int n = 0; n < tempBuf.size(); ++n)
            tempBuf[n] = juce::dsp::Complex<float>(0.0f, 0.0f);

        int startIdx = p * partSize;
        int numToCopy = juce::jmin(partSize, length - startIdx);

        for (int i = 0; i < numToCopy; ++i) {
            tempBuf[i] = juce::dsp::Complex<float>(irReadPtr[startIdx + i], 0.0f);
        }

        // Perform FFT on partition
        fft->perform(tempBuf.data(), tempBuf.data(), false);

        // Store result in H[p]
        H[p] = tempBuf;
    }

    DBG("Loaded IR as " << hParts << " blocks.");

    // Initialize input block FFT delay line
    X_fdl.clear();
    X_fdl.resize(hParts);
    for (int p = 0; p < hParts; ++p)
        X_fdl[p].resize(NFFT, { 0.0f, 0.0f });
}

void PartitionedConvolver::processBlock(const float* input, float* output, int numSamples) {
    DBG("Block size = " << numSamples);
    int inputPos = 0;
    int outputPos = 0;

    // Handle leftover samples from previous block output
    while (leftoverPos < leftoverOutput.size() && outputPos < numSamples)
        output[outputPos++] = leftoverOutput[leftoverPos++];

    if (leftoverPos >= leftoverOutput.size()) {
        leftoverOutput.clear();
        leftoverPos = 0;
    }

    while (inputPos < numSamples) {
        inputBuffer.push_back(input[inputPos]);
        ++inputPos;

        if (inputBuffer.size() == partSize) {
            std::vector<float> outputBuf(partSize, 0.0f); // temp buffer for output of convolution
            processPartition(inputBuffer.data(), outputBuf.data());

            int samplesToWrite = juce::jmin(partSize, numSamples - outputPos);

            std::copy(outputBuf.begin(), outputBuf.begin() + samplesToWrite,
                output + outputPos);

            outputPos += samplesToWrite;

            if (samplesToWrite < partSize) {
                leftoverOutput.assign(outputBuf.begin() + samplesToWrite, outputBuf.end());
                leftoverPos = 0;
            }

            inputBuffer.clear();
        }
    }
}

void PartitionedConvolver::processPartition(float* inputPart, float* outputPart) {

    // Shift right half of input buffer to left
    std::move(x_tdl.begin() + partSize, x_tdl.end(), x_tdl.begin());

    // Store new part in right half
    for (int i = 0; i < partSize; ++i) {
        x_tdl[partSize + i] = inputPart[i];
    }

    //Shift X_fdl

    for (int p = hParts - 1; p > 0; --p)
        X_fdl[p] = X_fdl[p - 1];

    // take FFt of current block
    fft->perform(x_tdl.data(), X_fdl[0].data(), false);

    // multiply-accumulate result
    std::vector<juce::dsp::Complex<float>> accumulator(NFFT, { 0.0f, 0.0f });
    for (int p = 0; p < hParts; ++p) {
        for (int k = 0; k < NFFT; ++k) {
            accumulator[k] += H[p][k] * X_fdl[p][k];
        }
    }

    // IFFT of accumulated spectral convolutions
    fft->perform(accumulator.data(), yifft.data(), true);

    // Normalise
    for (int n = 0; n < NFFT; ++n)
        yifft[n] /= NFFT;

    float sum = 0.0f;
    for (int n = 0; n < NFFT; ++n)
        sum += std::abs(yifft[n].real());
    DBG("Partition sum = " << sum);

    // Output right half of output
    for (int n = 0; n < partSize; ++n)
        outputPart[n] = yifft[partSize + n].real();
}
