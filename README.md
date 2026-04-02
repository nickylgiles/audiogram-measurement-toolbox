# Audiogram Measurement Toolbox
Masters project for research audiometry

## Installation
This project requires [JUCE v8.0.1](https://github.com/juce-framework/JUCE) to be installed on your machine.  
Open the [Jucer file](./AudiogramApp.jucer) in Projucer.  The global path to the JUCE library should be set in Projucer.
The project also requires HRIR (Head-Related Impulse Response) .wav files which are included in JUCE binary data to run the spatial and dual-task tests, and spoken digit files to run the digits-in-noise test.
A [MATLAB script](./Matlab/ir_extractor.m) is included to create these from a SOFA repository.  The .wav files must be included in Projucer as binary data.
