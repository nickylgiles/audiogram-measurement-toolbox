/*
  ==============================================================================

    SoundFileSource.h
    Created: 29 Nov 2025 1:24:15am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "SoundSource.h"
#include "../Utilities/SoundFilePlayer.h"

class SoundFileSource : public SoundSource {
public:
    SoundFileSource(double sampleRate, const void* data, size_t size, float gain, bool normaliseAudio = false);
    SoundFileSource(double sampleRate, const juce::File& file, float gain, bool normaliseAudio = false);
    void process(float* outputL, float* outputR, int numSamples) override;
    bool isFinished() const override;
private:
    SoundFilePlayer player;
    float gain;
};