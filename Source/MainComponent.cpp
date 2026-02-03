#include "MainComponent.h"
#include "AudiogramAppApplication.h"

//==============================================================================
MainComponent::MainComponent()
{
    soundEngine = std::make_unique<SoundEngine>();
    testController = nullptr;

    // Open logging database
    dbFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Results.db");
    DBG("Database file path: " << dbFile.getFullPathName());
    bool openDB = resultsLogger.openDatabase(dbFile);

    if (openDB)
        DBG("Database opened succesfully.");
    else
        DBG("Database failed to open.");

    // Set look and feel globally
    juce::LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);

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

void MainComponent::showMenuScreen() {
    auto screen = std::make_unique<MenuScreen>();

    /*
    screen->onPureToneClicked = [this] {showPureToneTestScreen();};
    screen->onSpatialClicked = [this] {showSpatialTestScreen();};
    screen->onDigitsInNoiseClicked = [this] {showSpeechInNoiseTestScreen();};
    screen->onDualTaskClicked = [this] {showDualTaskTestScreen();};
    */

    screen->addTest("Pure Tone Test", [this] {
        juce::MessageManager::callAsync([this] { showPureToneTestScreen();});
    });

    screen->addTest("Spatial Test", [this] {
        juce::MessageManager::callAsync([this] { showSpatialTestScreen(); });
    });
    screen->addTest("Digits-in-noise Test", [this] {
        juce::MessageManager::callAsync([this] { showSpeechInNoiseTestScreen(); });
    });
    screen->addTest("Dual-Task Test", [this] {
        juce::MessageManager::callAsync([this] { showDualTaskTestScreen(); });
    });

    screen->onSettingsClicked = [this] {showSettingsScreen();};

    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::showSettingsScreen() {
    auto screen = std::make_unique<SettingsScreen>();

    auto* app = static_cast<AudiogramAppApplication*>(juce::JUCEApplication::getInstance());
    auto* userSettings = app->applicationProperties.getUserSettings();

    screen->onBackClicked = [this] {showMenuScreen();};

    screen->addSetting("Export results database", [this] {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Export Results Database",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
            "*.db");

        auto fileChooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& fc) {
                auto result = fc.getResult();
                if (result != juce::File{}) {
                    dbFile.copyFileTo(result);
                }
                fileChooser.reset();
            });
        });

    screen->addSetting("Select word groups JSON file", [this] {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select word groups JSON file",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
            "*.json");

        auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& fc) {
            auto result = fc.getResult();
            if (result != juce::File{}) {
                auto* app = static_cast<AudiogramAppApplication*>(juce::JUCEApplication::getInstance());
                auto* userSettings = app->applicationProperties.getUserSettings();

                userSettings->setValue("wordGroupsJsonPath", result.getFullPathName());
                userSettings->saveIfNeeded();
                DBG("Word groups JSON path set to " << userSettings->getValue("wordGroupsJsonPath"));
            }
            fileChooser.reset();
            });

        resized();
        });

    screen->addSetting("Select headphone calibration", [] {});
   
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
    showResultsScreen<SpatialResultsScreen, SpatialTestController>();
}

void MainComponent::showSpeechInNoiseTestScreen() {
    testController.reset(new DigitsInNoiseController(*this, *soundEngine));
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
    if (auto* sin = dynamic_cast<DigitsInNoiseController*>(testController.get())) {
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
    showResultsScreen<SpeechInNoiseResultsScreen, DigitsInNoiseController>();
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
        testController->buttonClicked("word" + juce::String(wordIdx));
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
    showResultsScreen<DualTaskResultsScreen, DualTaskTestController>();
}

void MainComponent::showPureToneResultsScreen() {
    showResultsScreen<PureToneResultsScreen, PureToneTestController>();
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    if (currentScreen)
        currentScreen->setBounds(getLocalBounds());
}
