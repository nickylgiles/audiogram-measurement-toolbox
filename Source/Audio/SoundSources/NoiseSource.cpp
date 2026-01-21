/*
  ==============================================================================

    NoiseSource.cpp
    Created: 26 Nov 2025 4:02:53pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "NoiseSource.h"

NoiseSource::NoiseSource(double sampleRate, float gain, float durationSeconds, int ch, bool filterNoise, float centreFrequency) {
    noiseGenerator = NoiseGenerator(filterNoise);
    noiseGenerator.setSampleRate(sampleRate);
    noiseGenerator.setAmplitude(gain);

    if (filterNoise) {
        noiseGenerator.setFrequency(centreFrequency, 0.5f);
        noiseGenerator.prepareFilter();
    }

    envelope = Envelope();
    envelope.setRiseTime(static_cast<int>(sampleRate * 0.1f));
    envelope.setFallTime(static_cast<int>(sampleRate * 0.1f));
    envelope.start(static_cast<int>(sampleRate * durationSeconds));

    channel = ch;
    samplesRemaining = static_cast<int>(sampleRate * durationSeconds);
}

void NoiseSource::process(float* outputL, float* outputR, int numSamples) {
    float* noiseBuffer = (channel == 0 ? outputL : outputR);
    float* oppositeBuffer = (channel == 0 ? outputR : outputL);

    for (int i = 0; i < numSamples; ++i) {
        if (samplesRemaining <= 0) {
            noiseBuffer[i] = 0.0f;
            oppositeBuffer[i] = 0.0f;
            continue;
        }
        float env = envelope.nextSample();
        noiseBuffer[i] = noiseGenerator.nextSample() * env;
        oppositeBuffer[i] = 0.0f;
        --samplesRemaining;
    }
}

bool NoiseSource::isFinished() const {
    return samplesRemaining <= 0;
}
