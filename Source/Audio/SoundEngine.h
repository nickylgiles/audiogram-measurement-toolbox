/*
  ==============================================================================

    SoundEngine.h
    Created: 28 Oct 2025 11:07:04pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once


#include "Spatial/HRTFManager.h"
#include "SoundSources/SoundSource.h"
#include "SoundSources/ToneSource.h"
#include "SoundSources/NoiseSource.h"
#include "SoundSources/SoundFileSource.h"
#include "SoundSources/SpatialisedSoundFileSource.h"
#include "SoundSources/SpatialisedNoiseSource.h"
#include "Calibration/CalibrationFilter.h"


class SoundEngine {
public:
    SoundEngine();

    void playSource(std::unique_ptr<SoundSource>&& source);

    void playTone(float frequency, float amplitude, float duration, int channel);
    void playToneMasked(float frequency, float amplitude, float duration, int channel);

    void playSample(const void* data, size_t size, float gain, bool normaliseAudio = false);
    void playSampleSpatial(const void* data, size_t size, float elevation, float azimuth, float gain, bool normaliseAudio = false);

    void playSample(const juce::File& file, float gain, bool normaliseAudio = false);
    void playSampleSpatial(const juce::File& file, float elevation, float azimuth, float gain, bool normaliseAudio = false);

    void playNoise(float amplitude, float duration, int channel);
    void playNoiseSpatial(float amplitude, float duration, float elevation, float azimuth);

    void stop();
    bool isPlaying() const;

    void setSampleRate(double newSampleRate);
    void setSamplesPerBlockExpected(int newSamplesPerBlock);

    void processBlock(float* outputL, float* outputR, int numSamples);

    void loadCalibration(const juce::File& calibrationFile);
    const CalibrationFilter::Metadata& getCalibrationMetadata();

    const float getCalibrationSPLOffset();
    void setCalibrationSPLOffset(float offset);

    std::function<void(bool)> onClip;

    void startRecording(juce::String name);
    void stopRecording();

private:
    void addSource(std::unique_ptr<SoundSource> source);

    CalibrationFilter calibrationFilter;

    juce::AudioBuffer<float> sourceBuffer;
    juce::AudioBuffer<float> tempBuffer;

    double sampleRate;

    std::vector<std::unique_ptr<SoundSource>> sources;
    juce::SpinLock sourceLock;

    HRTFManager hrtfManager;

    float calibrationSPLOffset = 0.0f;

    bool lastBlockClipped = false;

    std::unique_ptr<juce::FileOutputStream> fileStream;
    std::unique_ptr<juce::AudioFormatWriter> writer;
    juce::WavAudioFormat wavFormat;

    bool recording = false;

};

