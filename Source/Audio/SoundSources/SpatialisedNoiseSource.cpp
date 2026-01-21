/*
  ==============================================================================

    SpatialisedNoiseSource.cpp
    Created: 26 Nov 2025 4:03:16pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpatialisedNoiseSource.h"

SpatialisedNoiseSource::SpatialisedNoiseSource(double sampleRate, float gain, float durationSeconds, HRTFManager& hrtfManager, float elevation, float azimuth)
    : spatialiser(hrtfManager)
{
    noiseGenerator = NoiseGenerator(false);
    noiseGenerator.setSampleRate(sampleRate);
    noiseGenerator.setAmplitude(gain);

    envelope = Envelope();
    envelope.setRiseTime(static_cast<int>(sampleRate * 0.1f));
    envelope.setFallTime(static_cast<int>(sampleRate * 0.1f));
    envelope.start(static_cast<int>(sampleRate * durationSeconds));

    samplesRemaining = static_cast<int>(sampleRate * durationSeconds);

    spatialiser.setSampleRate(sampleRate);
    spatialiser.setDirection(elevation, azimuth);

    tempBuffer.setSize(1, 0);
}

void SpatialisedNoiseSource::process(float* outputL, float* outputR, int numSamples) {

    tempBuffer.setSize(1, numSamples);
    float* tempWritePtr = tempBuffer.getWritePointer(0);
    for (int i = 0; i < numSamples; ++i) {
        if (samplesRemaining <= 0) {
            tempWritePtr[i] = 0.0f;
            continue;
        }
        float env = envelope.nextSample();
        tempWritePtr[i] = noiseGenerator.nextSample() * env;
        --samplesRemaining;
    }

    spatialiser.processBlock(tempWritePtr, outputL, outputR, numSamples);

}

bool SpatialisedNoiseSource::isFinished() const {
    return samplesRemaining <= 0;
}
