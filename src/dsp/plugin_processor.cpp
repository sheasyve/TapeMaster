#include "plugin_processor.h"
#include "knob_processing.h"
#include "ui/main/plugin_editor.h"

// --- Main Processing file, calls upon the audio processing file ---

MyPluginProcessor::MyPluginProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      apvts(*this, nullptr, "Parameters", Knobs::createParameterLayout())

#endif
{
    initPropertiesFile();
    apvts.state = juce::ValueTree(juce::Identifier("Parameters"));
}

MyPluginProcessor::~MyPluginProcessor() {}

bool MyPluginProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
    auto mainOut = layouts.getMainOutputChannelSet();
    auto mainIn = layouts.getMainInputChannelSet();
    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo()) return false;
    if (mainOut != mainIn) return false;
    return true;
}

// --- DSP ---

void MyPluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    auto numChannels = getTotalNumInputChannels();
    heldSamples.assign(numChannels, 0.0f);
    sampleCounters.assign(numChannels, 0);
    for (int i = 0; i < 2; ++i) {
        highPassFilters[i].reset();
        lowPassFilters[i].reset();
    }
}

void MyPluginProcessor::releaseResources() {}

// --- Editor Linking and UI ---

bool MyPluginProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor *MyPluginProcessor::createEditor() { return new MyPluginEditor(*this); }
void MyPluginProcessor::getStateInformation(juce::MemoryBlock &destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if (xml != nullptr) copyXmlToBinary(*xml, destData);
}

void MyPluginProcessor::setStateInformation(const void *data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr) {
        if (xmlState->hasTagName(apvts.state.getType())) { apvts.replaceState(juce::ValueTree::fromXml(*xmlState)); }
    }
}

inline juce::NormalisableRange<float> makeCustomSkewRange(float min, float max, float skew) {
    return juce::NormalisableRange<float>(
        min,
        max,
        [skew](float start, float end, float norm) { return start + (end - start) * std::pow(norm, 1.0f / skew); },
        [skew](float start, float end, float val) {
            float normalized = (val - start) / (end - start);
            return std::pow(std::max(0.0f, normalized), skew);
        },
        [](float start, float end, float val) { return juce::jlimit(start, end, val); });
}

void MyPluginProcessor::initPropertiesFile() {
    juce::PropertiesFile::Options options;
    options.applicationName = "SimpleCrush";
    options.filenameSuffix = ".xml";
    options.folderName = "SimpleCrush";
    options.storageFormat = juce::PropertiesFile::storeAsXML;
    appProperties.setStorageParameters(options);
}

void MyPluginProcessor::saveWindowSize(int width, int height) {
    if (auto *props = appProperties.getUserSettings()) {
        props->setValue("WindowWidth", width);
        props->setValue("WindowHeight", height);
        props->saveIfNeeded();
    }
}

juce::Point<int> MyPluginProcessor::getWindowSize() {
    if (auto *props = appProperties.getUserSettings()) {
        return {props->getIntValue("WindowWidth", 340), props->getIntValue("WindowHeight", 340)};
    }
    return {340, 340};
}

void MyPluginProcessor::saveFontSizeId(int fontSizeId) {
    if (auto *props = appProperties.getUserSettings()) {
        props->setValue("FontSizeId", fontSizeId);
        props->saveIfNeeded();
    }
}

int MyPluginProcessor::getSavedFontSizeId() {
    if (auto *props = appProperties.getUserSettings()) { return props->getIntValue("FontSizeId", 1); }
    return 1;
}

void MyPluginProcessor::saveTooltipState(bool isEnabled) {
    if (auto *props = appProperties.getUserSettings()) {
        props->setValue("ShowTooltips", isEnabled);
        props->saveIfNeeded();
    }
}

bool MyPluginProcessor::getSavedTooltipState() {
    if (auto *props = appProperties.getUserSettings()) { return props->getBoolValue("ShowTooltips", false); }
    return false;
}

void MyPluginProcessor::saveDataFolder(const juce::String &folderPath) {
    if (auto *props = appProperties.getUserSettings()) {
        props->setValue("DataFolder", folderPath);
        props->saveIfNeeded();
    }
}

juce::String MyPluginProcessor::getSavedDataFolder() {
    if (appProperties.getUserSettings() == nullptr) {
        initPropertiesFile();
    }

    if (auto *props = appProperties.getUserSettings()) {
        props->reload(); 
        
        juce::String savedPath = props->getValue("DataFolder", "");
        if (savedPath.isNotEmpty()) { return savedPath; }
    }

#if JUCE_WINDOWS
    juce::String regPath =
        juce::WindowsRegistry::getValue("HKEY_LOCAL_MACHINE\\Software\\Syverson Audio\\SimpleCrush\\DataPath");

    if (regPath.isNotEmpty()) {
        saveDataFolder(regPath);
        return regPath;
    }
#endif

    juce::File defaultFolder = juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory)
                                   .getChildFile("Syverson Audio")
                                   .getChildFile("SimpleCrush");

    return defaultFolder.getFullPathName();
}

// Boilerplate

const juce::String MyPluginProcessor::getName() const { return JucePlugin_Name; }
bool MyPluginProcessor::acceptsMidi() const { return false; }
bool MyPluginProcessor::producesMidi() const { return false; }
bool MyPluginProcessor::isMidiEffect() const { return false; }
double MyPluginProcessor::getTailLengthSeconds() const { return 0.0; }
int MyPluginProcessor::getNumPrograms() { return 1; }
int MyPluginProcessor::getCurrentProgram() { return 0; }
void MyPluginProcessor::setCurrentProgram(int index) {}
const juce::String MyPluginProcessor::getProgramName(int index) { return {}; }
void MyPluginProcessor::changeProgramName(int index, const juce::String &newName) {}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new MyPluginProcessor(); }