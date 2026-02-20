/*
  ==============================================================================

    CalibrationFilterParams.h
    Created: 20 Feb 2026 2:23:20pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct CalibrationFilterParams {
    struct BiquadParams {
        enum class Type {
            None,
            PK,
            LP,
            HP
        };

        BiquadParams(BiquadParams::Type type, float fc, float gain, float q);

        float fc = 0.0f;
        float gain = 0.0f;
        float q = 1.0f;
        Type type = Type::None;
    };

    enum class Channel { LEFT, RIGHT };

    static std::pair<CalibrationFilterParams, CalibrationFilterParams>
        loadFromFile(const juce::File& file);

private:
    std::vector<BiquadParams> biquads;
    Channel channel;
};
