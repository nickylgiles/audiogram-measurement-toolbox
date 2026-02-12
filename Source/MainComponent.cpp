#include "MainComponent.h"
#include "AudiogramAppApplication.h"

#include "Tests/Test.h"
#include "Tests/PureToneTest.h"
#include "Tests/SpatialTest.h"
#include "Tests/DigitsInNoiseTest.h"
#include "Tests/DualTaskTest.h"

//==============================================================================
MainComponent::MainComponent()
{
    soundEngine = std::make_unique<SoundEngine>();

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

    // Get pointer to user settings
    auto* app = static_cast<AudiogramAppApplication*>(juce::JUCEApplication::getInstance());
    userSettings = app->applicationProperties.getUserSettings();

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
    soundEngine->setSampleRate(sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    soundEngine->processBlock(leftChannel, rightChannel, bufferToFill.numSamples);
}

void MainComponent::releaseResources()
{
    
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::showMenuScreen() {
    auto screen = std::make_unique<MenuScreen>();

    screen->setUserId(userSettings->getValue("userId"));

    screen->onSettingsClicked = [this] {showSettingsScreen();};

    addTestToMenu<PureToneTest>(screen.get());
    addTestToMenu<SpatialTest>(screen.get());
    addTestToMenu<DigitsInNoiseTest>(screen.get());
    addTestToMenu<DualTaskTest>(screen.get());

    showScreen(std::move(screen));
    
}

void MainComponent::showSettingsScreen() {
    auto screen = std::make_unique<SettingsScreen>();

    screen->onBackClicked = [this] {showMenuScreen();};

    juce::String currentId = userSettings->getValue("userId", "");

    screen->addTextSetting(juce::translate("Set User ID"), [this](const juce::String& newId) {

        userSettings->setValue("userId", newId);

        DBG("User ID set to: " << userSettings->getValue("userId"));
        },
        currentId
    );

    screen->addButtonSetting(juce::translate("Export result DB"), [this] {
        fileChooser = std::make_unique<juce::FileChooser>(
            juce::translate("Export Results DB"),
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

    screen->addButtonSetting(juce::translate("Select word groups JSON file"), [this] {
        fileChooser = std::make_unique<juce::FileChooser>(
            juce::translate("Select word groups JSON file"),
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
            "*.json");

        auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& fc) {
            auto result = fc.getResult();
            if (result != juce::File{}) {

                userSettings->setValue("wordGroupsJsonPath", result.getFullPathName());
                userSettings->saveIfNeeded();
                DBG("Word groups JSON path set to " << userSettings->getValue("wordGroupsJsonPath"));
            }
            fileChooser.reset();
            });

        resized();
        });

    screen->addButtonSetting(juce::translate("Select headphone calibration"), [] {});
    
    screen->addButtonSetting(juce::translate("Change language"), [this] {
        auto* localisedStrings = new juce::LocalisedStrings(
            juce::String::createStringFromData(BinaryData::ga_lng, BinaryData::ga_lngSize),
            false);

        juce::LocalisedStrings::setCurrentMappings(localisedStrings);
    });

    screen->addTitleSetting(juce::translate("Load Test Configurations JSON"));

    addTestConfigSetting<PureToneTest>(screen.get());


   
    showScreen(std::move(screen));
}

void MainComponent::showScreen(std::unique_ptr<juce::Component>&& screen) {
    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

void MainComponent::resized()
{

    if (currentScreen)
        currentScreen->setBounds(getLocalBounds());
}
