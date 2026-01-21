/*
  ==============================================================================

    ToneSource.cpp
    Created: 26 Nov 2025 4:02:29pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "ToneSource.h"

// Pure tone source with 0.1 second envelope
ToneSource::ToneSource(double sampleRate, float frequency, float gain, float durationSeconds, int ch) {

    toneGenerator = ToneGenerator();
    toneGenerator.setSampleRate(sampleRate);
    toneGenerator.setAmplitude(gain);
    toneGenerator.setFrequency(frequency);

    envelope = Envelope();
    envelope.setRiseTime(static_cast<int>(sampleRate * 0.1f));
    envelope.setFallTime(static_cast<int>(sampleRate * 0.1f));
    envelope.start(static_cast<int>(sampleRate * durationSeconds));

    channel = ch;
    samplesRemaining = static_cast<int>(sampleRate * durationSeconds);
}

void ToneSource::process(float* outputL, float* outputR, int numSamples) {
    float* toneBuffer = (channel == 0 ? outputL : outputR);
    float* oppositeBuffer = (channel == 0 ? outputR : outputL);

    for (int i = 0; i < numSamples; ++i) {
        if (samplesRemaining <= 0) {
            toneBuffer[i] = 0.0f;
            oppositeBuffer[i] = 0.0f;
            continue;
        }
        float env = envelope.nextSample();
        toneBuffer[i] = toneGenerator.nextSample() * env;
        oppositeBuffer[i] = 0.0f;

        --samplesRemaining;
    }
}

bool ToneSource::isFinished() const {
    return samplesRemaining <= 0;
}
