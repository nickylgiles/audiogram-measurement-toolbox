#include "MainComponent.h"
#include "AudiogramAppApplication.h"
#include "Tests/Test.h"
#include "Tests/PureToneTest.h"

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

    screen->onSettingsClicked = [this] {showSettingsScreen();};

    screen->addTest("Pure Tone Test", 
        [this] {
            currentTest = std::make_unique<PureToneTest>(*this, *soundEngine);
            currentTest->displayInfo();
        });

    screen->addTest("Spatial Test", [this] { juce::MessageManager::callAsync([this] {
        auto infoScreen = std::make_unique<TestInfoScreen>(
            "Spatial Test",
            "Two sounds will play from different directions.  Press \"Left\" if the second sound comes to the left of the first; "
            "press \"Right\" if the second sound comes to the right of the first. "
            "If you are unsure, guess."
            "\n\nPress \"Start\" to begin the test.",
            [this] { showSpatialTestScreen();},
            [this] { showMenuScreen();}
        );

        showScreen(std::move(infoScreen));
        });});

    screen->addTest("Digits-in-noise Test", [this] { juce::MessageManager::callAsync([this] {
        auto infoScreen = std::make_unique<TestInfoScreen>(
            "Digits-in-noise Test",
            "You will hear three digits read aloud.  Afterwards, you must input the digits you hear in the correct order to the keypad."
            "If you are unsure, guess."
            "\n\nPress \"Start\" to begin the test.",
            [this] { showSpeechInNoiseTestScreen();},
            [this] { showMenuScreen();}
        );

        showScreen(std::move(infoScreen));
        });});
    
    screen->addTest("Dual-Task Test", [this] { juce::MessageManager::callAsync([this] {
        auto infoScreen = std::make_unique<TestInfoScreen>(
            "Dual-Task Test",
            "You will hear two words spoken from different directions.  Afterwards, you must choose the second word that was spoken from the options presented. "
            "Press \"Left\" if the second sound comes to the left of the first; "
            "press \"Right\" if the second sound comes to the right of the first. "
            "If you are unsure, guess. "
            "\n\nPress \"Start\" to begin the test.",
            [this] { showDualTaskTestScreen();},
            [this] { showMenuScreen();}
        );

        showScreen(std::move(infoScreen));
        });});
    
    showScreen(std::move(screen));
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
   
    showScreen(std::move(screen));

}


void MainComponent::showSpatialTestScreen() {
    testController.reset(new SpatialTestController(*this, *soundEngine));
    testStarted = true;
    testController->startTest();

    testController->onTestFinished = [this] {showSpatialResultsScreen();};

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

    showScreen(std::move(screen));
}

void MainComponent::showSpatialResultsScreen() {
    //showResultsScreen<SpatialResultsScreen, SpatialTestController>();
}

void MainComponent::showSpeechInNoiseTestScreen() {
    testController.reset(new DigitsInNoiseController(*this, *soundEngine));
    testStarted = true;
    testController->startTest();

    testController->onTestFinished = [this] {showSpeechInNoiseResultsScreen();};

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
    
    showScreen(std::move(screen));
}

void MainComponent::showSpeechInNoiseResultsScreen() {
    auto tc = dynamic_cast<DigitsInNoiseController*>(testController.get());
    if (tc)
        showResultsScreen<SpeechInNoiseResultsScreen, DigitsInNoiseController>(*tc);
}

void MainComponent::showDualTaskTestScreen() {
    testController.reset(new DualTaskTestController(*this, *soundEngine));
    testStarted = true;

    testController->onTestFinished = [this] { showDualTaskResultsScreen();};

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

    showScreen(std::move(screen));
}

void MainComponent::showDualTaskResultsScreen() {
    //showResultsScreen<DualTaskResultsScreen, DualTaskTestController>();
}

void MainComponent::showScreen(std::unique_ptr<juce::Component>&& screen) {
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
