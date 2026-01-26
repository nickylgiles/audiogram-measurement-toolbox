/*
  ==============================================================================

    PartitionedConvolver.cpp
    Created: 25 Nov 2025 7:42:14pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "PartitionedConvolver.h"

PartitionedConvolver::PartitionedConvolver(int partitionSize)
    : partSize(partitionSize)
{
    NFFT = 2 * partSize;
    int fftOrder = computeFFTOrder(NFFT);
    fft = std::make_unique<juce::dsp::FFT>(fftOrder);
    DBG("FFT size: " << fft->getSize());

    inputTDL.resize(NFFT, { 0.0f, 0.0f });
    outputFFT.resize(NFFT, {0.0f, 0.0f});
    outputIFFT.resize(NFFT, { 0.0f, 0.0f });
    inputBuffer.clear();
    inputBuffer.reserve(partSize);
    outputBuffer.resize(partSize, 0.0f);
    tempFFTBuffer.resize(NFFT, { 0.0f, 0.0f });
}

void PartitionedConvolver::loadIR(const juce::AudioBuffer<float>& irBuffer) {
    int length = irBuffer.getNumSamples();
    numIRParts = (length + partSize - 1) / partSize;

    irPartsFFT.clear();
    irPartsFFT.resize(numIRParts);

    for (int i = 0; i < numIRParts; ++i) {
        irPartsFFT[i].resize(NFFT, {0.0f, 0.0f});
    }

    auto* irReadPtr = irBuffer.getReadPointer(0);

    // Zero-pad parts and perform FFT
    for (int p = 0; p < numIRParts; ++p) {

        int startIdx = p * partSize;
        int numToCopy = juce::jmin(partSize, length - startIdx);

        for (int i = 0; i < numToCopy; ++i) {
            tempFFTBuffer[i] = juce::dsp::Complex<float>(irReadPtr[startIdx + i], 0.0f);
        }

        // Zero rest of final section of IR if necesssary
        for (int n = numToCopy; n < tempFFTBuffer.size(); ++n)
            tempFFTBuffer[n] = juce::dsp::Complex<float>(0.0f, 0.0f);

        // Perform FFT on partition
        fft->perform(tempFFTBuffer.data(), tempFFTBuffer.data(), false);

        // Normalise here for output
        /*
        float invNFFT = 1.0f / NFFT;
        for (int i = 0; i < tempFFTBuffer.size(); ++i) {
            tempFFTBuffer[i] *= invNFFT;
        }
        */

        // Store result in H[p]
        irPartsFFT[p] = tempFFTBuffer;
    }

    DBG("Loaded IR as " << numIRParts << " blocks.");

    // Initialize input block FFT delay line
    inputFDL.clear();
    inputFDL.resize(numIRParts);
    for (int p = 0; p < numIRParts; ++p)
        inputFDL[p].resize(NFFT, { 0.0f, 0.0f });
}

void PartitionedConvolver::processBlock(const float* input, float* output, int numSamples) {
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
        // Fill buffer with incoming samples until full or run out of input samples
        int samplesToFill = static_cast<int>(partSize - inputBuffer.size());
        int samplesToCopy = juce::jmin(numSamples - inputPos, samplesToFill);
        inputBuffer.insert(inputBuffer.end(), input + inputPos, input + inputPos + samplesToCopy);
        inputPos += samplesToCopy;

        if (inputBuffer.size() == partSize) {
            // When buffer has a full partition-sized block, process it
            processPartition(inputBuffer.data(), outputBuffer.data());
            int samplesToWrite = juce::jmin(partSize, numSamples - outputPos);
            std::copy(outputBuffer.begin(), outputBuffer.begin() + samplesToWrite,
                output + outputPos);
            outputPos += samplesToWrite;
            if (samplesToWrite < partSize) {
                leftoverOutput.assign(outputBuffer.begin() + samplesToWrite, outputBuffer.end());
                leftoverPos = 0;
            }
            inputBuffer.clear();
        }
    }

    // Clear unused space on output buffer
    while (outputPos < numSamples) {
        output[outputPos++] = 0.0f;
    }
}


void PartitionedConvolver::processPartition(float* inputPart, float* outputPart) {

    // Shift right half of input buffer to left
    std::move(inputTDL.begin() + partSize, inputTDL.end(), inputTDL.begin());

    // Store new part in right half
    for (int i = 0; i < partSize; ++i) {
        inputTDL[partSize + i] = inputPart[i];
    }

    //Shift X_fdl
    inputFDLidx = (inputFDLidx + 1) % numIRParts;

    // take FFt of current block
    fft->perform(inputTDL.data(), inputFDL[inputFDLidx].data(), false);

    // multiply-accumulate result
    std::vector<juce::dsp::Complex<float>> accumulator(NFFT, { 0.0f, 0.0f });
    for (int p = 0; p < numIRParts; ++p) {
        for (int k = 0; k < NFFT; ++k) {
            int idx = (inputFDLidx - p + numIRParts) % numIRParts; // index of FDL to use (wrap around)
            accumulator[k] += irPartsFFT[p][k] * inputFDL[idx][k];
        }
    }

    // IFFT of accumulated spectral convolutions
    fft->perform(accumulator.data(), outputIFFT.data(), true);

    // Scale IFFT
    float invNFFT = 1.0f / NFFT;
    for (int i = 0; i < outputIFFT.size(); ++i) {
        outputIFFT[i] *= invNFFT;
    }
    
    // Output left half of output
    for (int n = 0; n < partSize; ++n)
        outputPart[n] = outputIFFT[n].real();
}
