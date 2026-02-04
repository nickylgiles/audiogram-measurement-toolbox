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

    addTestToMenu<PureToneTest>(screen.get(), "Pure Tone Test");
    addTestToMenu<SpatialTest>(screen.get(), "Spatial Test");
    addTestToMenu<DigitsInNoiseTest>(screen.get(), "Digits-in-noise Test");
    addTestToMenu<DualTaskTest>(screen.get(), "Dual-task Test");

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
