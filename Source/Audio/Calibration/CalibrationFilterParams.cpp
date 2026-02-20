/*
  ==============================================================================

    CalibrationFilterParams.cpp
    Created: 20 Feb 2026 2:38:39pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "CalibrationFilterParams.h"

CalibrationFilterParams::BiquadParams::BiquadParams(BiquadParams::Type type, float fc, float gain, float q)
    : type(type), fc(fc), gain(gain), q(q) {
}

std::pair<CalibrationFilterParams, CalibrationFilterParams> 
CalibrationFilterParams::loadFromFile(const juce::File& file) {
    return std::pair<CalibrationFilterParams, CalibrationFilterParams>();
}
