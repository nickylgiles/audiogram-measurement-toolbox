/*
  ==============================================================================

    SoundFilePlayer.h
    Created: 1 Nov 2025 9:05:31am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SoundFilePlayer {
public:
    SoundFilePlayer();
    bool loadFile(const juce::File& file, bool normalise = false);
    bool loadBinaryData(const void* data, size_t size, bool normalise = false);
    bool loadNoise(int length);
    bool loadNoise(int length, juce::dsp::IIR::Coefficients<float> filterCoeffs);
    void setSampleRate(double newSampleRate);
    void reset();
    float nextSample();
    bool fileIsLoaded() const;
    bool soundIsPlaying() const;
    void startPlaying();
    int getLength();
    bool isFinished() const;
    int samplesRemaining();
private:
    void normaliseBuffer(juce::AudioBuffer<float>& buffer);
    juce::AudioBuffer<float> buffer;
    juce::AudioFormatManager audioFormatManager;
    double currentSample;
    int totalSamples;
    double sampleRate;
    bool fileLoaded;
    bool soundPlaying;

    double fileSampleRate;
    double playbackIncrement;
};