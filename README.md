# Audiogram Measurement Toolbox
Masters project for research audiometry

## Features

- Pure-tone (Hughson-Westlake) audiogram measurement
- Spatial hearing test using HRIR convolution
- Digits-in-noise test
- Dual-task (spatial and speech) test

## Dependencies
This project requires [JUCE v8.0.1](https://github.com/juce-framework/JUCE) to be installed on your machine.  
Open the [Jucer file](./AudiogramApp.jucer) in Projucer.  The global path to the JUCE library should be set in Projucer.
The project also requires HRIR (Head-Related Impulse Response) .wav files which are included in JUCE binary data to run the spatial and dual-task tests, and spoken digit files to run the digits-in-noise test.
A [MATLAB script](./Matlab/ir_extractor.m) is included to create these from a SOFA repository.  The .wav files must be included in Projucer as binary data. 
The digit files are from [Audio MNIST](https://github.com/soerenab/AudioMNIST).
The dual-task test uses speech samples from the [Modified Rhyme Test Audio Library](https://www.nist.gov/ctl/pscr/modified-rhyme-test-audio-library).  These are not included in binary data, and are instead configured via JSON loaded within the toolbox.
[SQLite](https://github.com/clibs/sqlite/) is included as a submodule for logging test results.

## Unit Tests
[Unit tests](./Source/UnitTests) are included for the core audio processing functionality.  These can be run by using the UnitTests build configuration.
