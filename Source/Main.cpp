/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "AudiogramAppApplication.h"
#include "MainComponent.h"

#if RUN_UNIT_TESTS
#include "UnitTests/AudioTests.h"
#include "UnitTests/PartitionedConvolverTests.h"
#endif

// ===== MainWindow definition =====
class AudiogramAppApplication::MainWindow : public juce::DocumentWindow {
public:
    MainWindow(juce::String name)
        : DocumentWindow(name,
            juce::Desktop::getInstance().getDefaultLookAndFeel()
            .findColour(juce::ResizableWindow::backgroundColourId),
            DocumentWindow::allButtons) {
        setUsingNativeTitleBar(true);
        setContentOwned(new MainComponent(), true);

        #if JUCE_IOS || JUCE_ANDROID
                setFullScreen(true);
        #else
                setResizable(true, true);
                centreWithSize(getWidth(), getHeight());
        #endif

        setVisible(true);
    }

    void closeButtonPressed() override {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
};

// ===== Application methods =====
const juce::String AudiogramAppApplication::getApplicationName() {
    return ProjectInfo::projectName;
}

const juce::String AudiogramAppApplication::getApplicationVersion() {
    return ProjectInfo::versionString;
}

bool AudiogramAppApplication::moreThanOneInstanceAllowed() {
    return true;
}

void AudiogramAppApplication::initialise(const juce::String&) {
#if RUN_UNIT_TESTS
    juce::UnitTestRunner unitTestRunner;
    unitTestRunner.runAllTests();
    quit();
#else
    juce::PropertiesFile::Options options;
    options.applicationName = getApplicationName();
    options.filenameSuffix = "settings";
    options.folderName = "AudiogramApp";

    applicationProperties.setStorageParameters(options);

    mainWindow.reset(new MainWindow(getApplicationName()));
#endif
}

void AudiogramAppApplication::shutdown() {
    mainWindow = nullptr;
}

START_JUCE_APPLICATION(AudiogramAppApplication)
