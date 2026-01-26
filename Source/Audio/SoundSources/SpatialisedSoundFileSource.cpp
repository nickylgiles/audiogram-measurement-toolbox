/*
  ==============================================================================

    SpatialisedSoundFileSource.cpp
    Created: 26 Nov 2025 4:04:00pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpatialisedSoundFileSource.h"
#include "SoundFileSource.h"

SpatialisedSoundFileSource::SpatialisedSoundFileSource(double sampleRate, const void* data, size_t size, HRTFManager& hrtfManager, float elevation, float azimuth, float newGain, bool normaliseAudio)
    : spatialiser(hrtfManager), gain(newGain)
{
    player.setSampleRate(sampleRate);
    
    if (!player.loadBinaryData(data, size, normaliseAudio)) {
        return;
    }

    player.startPlaying();

    spatialiser.setSampleRate(sampleRate);
    spatialiser.setDirection(elevation, azimuth);

    tempBuffer.setSize(1, 0);
}

SpatialisedSoundFileSource::SpatialisedSoundFileSource(double sampleRate, const juce::File& file, HRTFManager& hrtfManager, float elevation, float azimuth, float newGain, bool normaliseAudio) 
    : spatialiser(hrtfManager), gain(newGain)
{
    player.setSampleRate(sampleRate);

    if (!player.loadFile(file, normaliseAudio)) {
        return;
    }

    player.startPlaying();

    spatialiser.setSampleRate(sampleRate);
    spatialiser.setDirection(elevation, azimuth);

    tempBuffer.setSize(1, 0);
}

void SpatialisedSoundFileSource::process(float* outputL, float* outputR, int numSamples) {
    tempBuffer.setSize(1, numSamples, false, false, true);
    float* tempWritePtr = tempBuffer.getWritePointer(0);

    for (int i = 0; i < numSamples; ++i) {
        if (player.isFinished()) {
            tempWritePtr[i] = 0.0f;
        }
        else {
            tempWritePtr[i] = player.nextSample() * gain;
        }
    }

    spatialiser.processBlock(tempWritePtr, outputL, outputR, numSamples);
}

bool SpatialisedSoundFileSource::isFinished() const {
    return player.isFinished();
}
