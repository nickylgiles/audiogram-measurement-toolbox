/*
  ==============================================================================

    SpatialisedSoundFileSource.h
    Created: 26 Nov 2025 4:04:00pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "SoundSource.h"
#include "../Spatial/HRTFManager.h"
#include "../Spatial/Spatialiser.h"
#include "../Utilities/SoundFilePlayer.h"

class SpatialisedSoundFileSource : public SoundSource {
public:
    SpatialisedSoundFileSource(double sampleRate, const void* data, size_t size, HRTFManager& hrtfManager, float elevation, float azimuth, float gain, bool normaliseAudio = false);
    SpatialisedSoundFileSource(double sampleRate, const juce::File& file, HRTFManager& hrtfManager, float elevation, float azimuth, float gain, bool normaliseAudio = false);
    void process(float* outputL, float* outputR, int numSamples) override;
    bool isFinished() const override;
private:
    SoundFilePlayer player;
    Spatialiser spatialiser;

    juce::AudioBuffer<float> tempBuffer;
    float gain;
};