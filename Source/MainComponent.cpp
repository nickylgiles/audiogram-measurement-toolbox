#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{

    soundEngine = std::make_unique<SoundEngine>();
    testController = nullptr;

    // Open logging database
    dbFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("Results.db");
    DBG("Database file path: " << dbFile.getFullPathName());
    bool openDB = resultsLogger.openDatabase(dbFile);

    if (openDB)
        DBG("Database opened succesfully.");
    else
        DBG("Database failed to open.");

    showMenuScreen();

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.

    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    soundEngine->setSampleRate(sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();

    auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    soundEngine->processBlock(leftChannel, rightChannel, bufferToFill.numSamples);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
   
}
void MainComponent::testEnd() {
    testStarted = false;
    showPureToneResultsScreen();
}
void MainComponent::showMenuScreen() {
    auto screen = std::make_unique<MenuScreen>();

    screen->onPureToneClicked = [this] {showPureToneTestScreen();};
    screen->onSpatialClicked = [this] {showSpatialTestScreen();};
    screen->onSpeechInNoiseClicked = [this] {showSpeechInNoiseTestScreen();};
    screen->onDualTaskClicked = [this] {showDualTaskTestScreen();};

    screen->onSettingsClicked = [this] {showSettingsScreen();};

    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::showSettingsScreen() {
    auto screen = std::make_unique<SettingsScreen>();

    screen->onBackClicked = [this] {showMenuScreen();};

    screen->onExportClicked = [this] {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Export Results Database",
            dbFile.getParentDirectory(),
            "*.db");

        auto fileChooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& fc) {
                auto result = fc.getResult();
                if (result != juce::File{}) {
                    dbFile.copyFileTo(result);
                }
                fileChooser.reset();
            });
        };

    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();

}

void MainComponent::showPureToneTestScreen() {
    testController.reset(new PureToneTestController(*this, *soundEngine));
    testStarted = true;
    testController->startTest();

    auto screen = std::make_unique<PureToneTestScreen>();
    screen->onHearClicked = [this] {
        testController->buttonClicked("hearButton");
        };
    screen->onStopClicked = [this] {
        testStarted = false;
        testController->buttonClicked("stopButton");
        showMenuScreen();
        };

    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::showSpatialTestScreen() {
    testController.reset(new SpatialTestController(*this, *soundEngine));
    testStarted = true;
    testController->startTest();

    auto screen = std::make_unique<SpatialTestScreen>();
    screen->onLeftClicked = [this] {
        testController->buttonClicked("leftButton");
        };
    screen->onRightClicked = [this] {
        testController->buttonClicked("rightButton");
        };
    screen->onStopClicked = [this] {
        testStarted = false;
        testController->buttonClicked("stopButton");
        showMenuScreen();
        };

    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::showSpatialResultsScreen() {
    auto spaTestController = dynamic_cast<SpatialTestController*>(testController.get());
    if (!spaTestController) {
        return;
    }
    SpatialTestResults results = spaTestController->getResults();
    auto screen = std::make_unique<SpatialResultsScreen>();

    screen->setResults(results);
    screen->onExportClicked = [this, results] {
        if (resultsLogger.logSpatialResults(results)) {
            DBG("Spatial results logged successfully.");
        }
        else {
            DBG("Failed to save spatial results.");
        }
    };
    screen->onMenuClicked = [this] {
        showMenuScreen();
    };

    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::showSpeechInNoiseTestScreen() {
    testController.reset(new SpeechInNoiseController(*this, *soundEngine));
    testStarted = true;
    testController->startTest();

    auto screen = std::make_unique<SpeechInNoiseTestScreen>();

    screen->onStopClicked = [this] {
        testStarted = false;
        testController->buttonClicked("stopButton");
        showMenuScreen();
    };

    screen->onDigitClicked = [this](int digit) {
        testController->buttonClicked(juce::String(digit));
    };


    // Get inputsEnabled() from SpeechInNoiseController
    // SafePointer needed in case the screen is deleted before async call is executed
    if (auto* sin = dynamic_cast<SpeechInNoiseController*>(testController.get())) {
        auto* scr = dynamic_cast<SpeechInNoiseTestScreen*>(screen.get());
        juce::Component::SafePointer<SpeechInNoiseTestScreen> scrPtr = scr;
        if (scrPtr) {
            sin->setInputsEnabled = [scrPtr](bool enabled) {
                juce::MessageManager::callAsync([scrPtr, enabled] {
                    if (scrPtr) scrPtr->setDigitsEnabled(enabled);
                    });
                };
        }
    }
    
    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::showSpeechInNoiseResultsScreen() {
    auto sinTestController = dynamic_cast<SpeechInNoiseController*>(testController.get());
    if (!sinTestController) {
        return;
    }
    SpeechInNoiseTestResults results = sinTestController->getResults();

    auto screen = std::make_unique<SpeechInNoiseResultsScreen>();

    screen->setResults(results);
    screen->onExportClicked = [this, results] {
        if (resultsLogger.logSpeechInNoiseResults(results)) {
            DBG("Speech-in-noise results logged successfully.");
        }
        else {
            DBG("Failed to save speechin-noise results.");
        }
    };
    screen->onMenuClicked = [this] {
        showMenuScreen();
        };

    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::showDualTaskTestScreen() {

    testController.reset(new DualTaskTestController(*this, *soundEngine));
    testStarted = true;

    auto screen = std::make_unique<DualTaskTestScreen>();

    screen->onStopClicked = [this] {
        testStarted = false;
        testController->buttonClicked("stopButton");
        showMenuScreen();
        };
    screen->onLeftClicked = [this] {
        testController->buttonClicked("leftButton");
        };
    screen->onRightClicked = [this] {
        testController->buttonClicked("rightButton");
        };

    screen->onWordClicked = [this](int wordIdx) {
        testController->buttonClicked("word" + wordIdx);
        };


    // Async callbacks for enabling/ disabling input buttons on the GUI and setting word list to show
    if (auto* ctr = dynamic_cast<DualTaskTestController*>(testController.get())) {
        auto* scr = dynamic_cast<DualTaskTestScreen*>(screen.get());
        juce::Component::SafePointer<DualTaskTestScreen> scrPtr = scr;
        if (scrPtr) {
            ctr->setInputsEnabled = [scrPtr](bool enabled) {
                juce::MessageManager::callAsync([scrPtr, enabled] {
                    if (scrPtr) scrPtr->setInputEnabled(enabled);
                    });
                };
            DBG("Dual task enable Callback set");

            ctr->setWords = [scrPtr](std::vector<juce::String> words) {
                juce::MessageManager::callAsync([scrPtr, words] {
                    if (scrPtr) scrPtr->setWords(words);
                    });
                };
        }
    }



    testController->startTest();
    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::showDualTaskResultsScreen() {
}

void MainComponent::showPureToneResultsScreen() {
    auto ptTestController = dynamic_cast<PureToneTestController*>(testController.get());
    if (!ptTestController) {
        return;
    }
    PureToneTestResults results = ptTestController->getResults();
    auto screen = std::make_unique<PureToneResultsScreen>();

    screen->setResults(results);
    screen->onExportClicked = [this, results] {
        if (resultsLogger.logPureToneResults(results)) {
            DBG("Pure tone results logged successfully.");
        }
        else {
            DBG("Failed to save pure tone results.");
        }
    };
    screen->onMenuClicked = [this] {
        showMenuScreen();
    };

    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    if (currentScreen)
        currentScreen->setBounds(getLocalBounds());
}
