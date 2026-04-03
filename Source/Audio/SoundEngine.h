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

// So can log like DBG in release configuration
#if BENCHMARKING
#define NOMINMAX
#include <Windows.h>
#define BENCH_DBG(x) do { \
    juce::String str_bdbg; \
    str_bdbg << x << "\n"; \
    OutputDebugStringA(str_bdbg.toUTF8()); \
} while(0)
#endif


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

    // Performance benchmarking
#if BENCHMARKING
    static const int maxBenchmarkBlocks = 10000;
    std::array<double, maxBenchmarkBlocks> blockProcessingTimes{};
    std::array<double, maxBenchmarkBlocks> outputCopyTimes{};
    std::array<double, maxBenchmarkBlocks> calibrationTimes{};
    std::array<double, maxBenchmarkBlocks> sourceProcessingTimes{};
    std::array<double, maxBenchmarkBlocks> lockTimes{};
    std::array<double, maxBenchmarkBlocks> tempBufferTimes{};
    int blockIdx = 0;

    void resetBenchmarks() {
        blockProcessingTimes.fill(0.0);
        outputCopyTimes.fill(0.0);
        calibrationTimes.fill(0.0);
        sourceProcessingTimes.fill(0.0);
        lockTimes.fill(0.0);
        tempBufferTimes.fill(0.0);
        int blockIdx = 0;
    }

    void printStageStats(juce::String name, std::array<double, maxBenchmarkBlocks>& times) {
        double sum = 0.0;
        double minVal = std::numeric_limits<double>::max();
        double maxVal = 0.0;
        int count = 0;

        for (double t : times) {
            if (t > 0.0) {
                sum += t;
                minVal = (t < minVal) ? t : minVal;
                maxVal = (t > maxVal) ? t : maxVal;
                ++count;
            }
        }

        if (count > 0) {
            BENCH_DBG(" " << name
                << " | Max: " << maxVal << " ms"
                << " | Min: " << minVal << " ms"
                << " | Avg: " << (sum / count) << " ms");
        }
    }

    void printBenchmarkStats() {
        double sum = 0.0;
        double minVal = std::numeric_limits<double>::max();
        double maxVal = 0.0;

        int count = 0;

        for (int i = 0; i < maxBenchmarkBlocks; ++i) {
            auto t = blockProcessingTimes[i];
            if (t > 0.0) {
                sum += t;
                minVal = (t < minVal) ? t : minVal;
                maxVal = (t > maxVal) ? t : maxVal;

                ++count;
            }
        }

        if (count == 0)
            return;

        BENCH_DBG("--------SoundEngine Performance Benchmarks--------");
        BENCH_DBG("Average block time: " << (sum / count) << " ms");
        BENCH_DBG("Min block time:     " << minVal << " ms");
        BENCH_DBG("Max block time:     " << maxVal << " ms");
        BENCH_DBG("--------------------------------------------------");

        BENCH_DBG("Stage                 | Max Time (ms)  | Min Time (ms) | Avg Time (ms)");
        BENCH_DBG("------------------------------------------------------------------------");
        
        
        printStageStats("Temp buffer setup   ", tempBufferTimes);
        printStageStats("Lock acquisition    ", lockTimes);
        printStageStats("Source processing   ", sourceProcessingTimes);
        printStageStats("Calibration filter  ", calibrationTimes);
        printStageStats("Output copy & clip  ", outputCopyTimes);
        BENCH_DBG("------------------------------------------------------------------------");

    }



#endif

};

