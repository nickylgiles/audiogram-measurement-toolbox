/*
  ==============================================================================

    SoundFileSource.cpp
    Created: 29 Nov 2025 1:24:15am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SoundFileSource.h"

SoundFileSource::SoundFileSource(double sampleRate, const void* data, size_t size, float newGain, bool normaliseAudio) {
    player.setSampleRate(sampleRate);
    gain = newGain;

    if (!player.loadBinaryData(data, size, normaliseAudio)) {
        return;
    }

    player.startPlaying();
}

SoundFileSource::SoundFileSource(double sampleRate, const juce::File& file, float newGain, bool normaliseAudio) {
    player.setSampleRate(sampleRate);
    gain = newGain;

    if (!player.loadFile(file, normaliseAudio)) {
        return;
    }

    player.startPlaying();
}

void SoundFileSource::process(float* outputL, float* outputR, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
        if (player.isFinished()) {
            outputL[i] = 0.0f;
        }
        else {
            outputL[i] = player.nextSample() * gain;
        }
        outputR[i] = outputL[i];
    }
}

bool SoundFileSource::isFinished() const {
    return player.isFinished();
}
