#include "MainComponent.h"
#include "AudiogramAppApplication.h"

#include "Tests/Test.h"
#include "Tests/PureToneTest.h"
#include "Tests/SpatialTest.h"
#include "Tests/DigitsInNoiseTest.h"
#include "Tests/DualTaskTest.h"
#include "Tests/TestCalibrationTest.h"

//==============================================================================
MainComponent::MainComponent()
{
    soundEngine = std::make_unique<SoundEngine>();

    // Warning label for if clipping occurs
    addAndMakeVisible(clippingWarningLabel);
    clippingWarningLabel.setColour(juce::Label::textColourId, juce::Colours::red);

    soundEngine->onClip = [this](bool clipping) {
        juce::MessageManager::callAsync([this, clipping] {
            clippingWarningLabel.setText(juce::translate(
                clipping ? "Warning: Output exceeded threshold.  Audio muted. " : ""),
                juce::dontSendNotification
            );}
        );
    };

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

    // Set language if set in user settings
    juce::String currentLanguage = userSettings->getValue("language", "en");

    if (currentLanguage == "en") {
        setLanguageFromData(BinaryData::en_lng, BinaryData::en_lngSize);
    }

    if (currentLanguage == "ga") {
        setLanguageFromData(BinaryData::ga_lng, BinaryData::ga_lngSize);
    }

    // Load calibration profile if has been set
    juce::String calibPath = userSettings->getValue("calibrationJsonPath", "");
    if (!calibPath.isEmpty()) {
        juce::File calibFile(calibPath);
        if (calibFile.existsAsFile()) {
            soundEngine->loadCalibration(calibFile);
            DBG("Loaded headphone calibration");
        }
    }

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
    
    // Dump HTL spline to debug
    #if JUCE_DEBUG 
    dumpSplineDebug();
    #endif

}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.

    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    soundEngine->setSamplesPerBlockExpected(samplesPerBlockExpected);
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

    juce::String calibrationId = soundEngine->getCalibrationMetadata().calibrationId;
    screen->setCalibrationId(calibrationId);

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

    screen->addButtonSetting(juce::translate("Select word groups JSON file") + " (" + userSettings->getValue("wordGroupsJsonPath", juce::translate("Not Set")) + ")", [this] {
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
                refreshSettingsScreen();
            }
            fileChooser.reset();
            });
        });

    screen->addButtonSetting(juce::translate("Select headphone calibration") + " (" + userSettings->getValue("calibrationJsonPath", juce::translate("Not Set")) + ")", [this] {
        fileChooser = std::make_unique<juce::FileChooser>(
            juce::translate("Select headphone calibration"),
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
            "*.json");

    auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& fc) {
        auto result = fc.getResult();
        if (result != juce::File{}) {

            userSettings->setValue("calibrationJsonPath", result.getFullPathName());
            userSettings->saveIfNeeded();
            DBG("Calibration JSON path set to " << userSettings->getValue("calibrationJsonPath"));
            
            if (soundEngine)
                soundEngine->loadCalibration(result);

            refreshSettingsScreen();
        }
        fileChooser.reset();
        });
    });

    screen->addButtonSetting(juce::translate("Check Levels"), [this] {
        // Call async to allow settings screen to handle the button press before it is destroyed
        juce::MessageManager::callAsync([this]() {
            currentTest = std::make_unique<TestCalibrationTest>(*this, *soundEngine);
            currentTest->startTest();
            });
        });
    
    screen->addButtonSetting(juce::translate("Change language"), [this] {
        juce::String currentLanguage = userSettings->getValue("language", "en");
        DBG("Current language: " << currentLanguage);

        if (currentLanguage == "en") {
            setLanguageFromData(BinaryData::ga_lng, BinaryData::ga_lngSize);

            userSettings->setValue("language", "ga");
            userSettings->saveIfNeeded();

            DBG("Language changed to Irish");
        }

        if (currentLanguage == "ga") {
            setLanguageFromData(BinaryData::en_lng, BinaryData::en_lngSize);

            userSettings->setValue("language", "en");
            userSettings->saveIfNeeded();

            DBG("Language changed to English");
        }

        refreshSettingsScreen();
    });

    screen->addTitleSetting(juce::translate("Load Test Configurations"));

    addTestConfigSetting<PureToneTest>(screen.get());
    addTestConfigSetting<SpatialTest>(screen.get());
    addTestConfigSetting<DigitsInNoiseTest>(screen.get());
    addTestConfigSetting<DualTaskTest>(screen.get());

    showScreen(std::move(screen));
}

void MainComponent::refreshSettingsScreen() {
    juce::MessageManager::callAsync([this] { 
        showSettingsScreen();
    });
}

void MainComponent::showScreen(std::unique_ptr<juce::Component>&& screen) {

    if (currentScreen) {
        removeChildComponent(currentScreen.get());
    }

    currentScreen = std::move(screen);
    addAndMakeVisible(currentScreen.get());
    resized();
}

juce::String MainComponent::getUserId() {
    if (!userSettings) {
        return juce::String{ };
    }

    return userSettings->getValue("userId", juce::String{ });
}

void MainComponent::setLanguageFromData(const void* data, int size) {
    auto* localisedStrings = new juce::LocalisedStrings(
        juce::String::createStringFromData(data, size),
        false);

    juce::LocalisedStrings::setCurrentMappings(localisedStrings);
    DBG("Localisation mappings set");
}

#if JUCE_DEBUG
void MainComponent::dumpSplineDebug() {
    juce::File file = juce::File::getSpecialLocation(
        juce::File::userDocumentsDirectory).getChildFile("HLT_spline.csv");

    juce::FileOutputStream stream(file);
    stream << "frequency,tf\n";

    float logMin = std::log10(20.0f);
    float logMax = std::log10(20000.0f);
    int nPoints = 1000;

    for (int i = 0; i < nPoints; ++i) {
        float logF = logMin + i * (logMax - logMin) / nPoints;
        float f = std::pow(10.0f, logF);
        float tF = HTL::toSPL(0.0f, 0.0f, f);
        stream << f << "," << tF << "\n";
    }

    DBG("Spline dumped to " + file.getFullPathName());
}
#endif

void MainComponent::resized() {
    if (currentScreen)
        currentScreen->setBounds(getLocalBounds());

    clippingWarningLabel.setBounds(0, getHeight() - 40, getWidth(), 40);
}
