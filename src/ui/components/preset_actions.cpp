#include "preset_menu.h"

// --- Preset onClick Logic ---

void PresetMenu::presetCallbacks() {

    presetsButton.onClick = [this]() {
        isPresetsVisible = !isPresetsVisible;
        updateMenuVisibility();
        if (onPresetsToggled != nullptr) onPresetsToggled(isPresetsVisible);
    };

    folderButton.onClick = [this]() {
        if (onFolderIconClicked) onFolderIconClicked();
    };

    deleteButton.onClick = [this]() {
        int selectedRow = presetList.getSelectedRow();
        if (juce::isPositiveAndBelow(selectedRow, (int)presets.size())) {
            auto file = presets[selectedRow].file;
            if (file.existsAsFile()) {
                file.deleteFile();
                loadPresetsFromDirectory();
                saveTextBox.setText("", juce::dontSendNotification);
                juce::Logger::writeToLog("Deleted preset: " + file.getFileName());
            }
        }
    };

    saveButton.onClick = [this]() {
        juce::String newPresetName = saveTextBox.getText();
        if (newPresetName.isNotEmpty()) {
            auto file = presetDirectory.getChildFile(newPresetName).withFileExtension(".preset");
            auto state = apvts.copyState();
            std::unique_ptr<juce::XmlElement> xml(state.createXml());
            if (xml != nullptr) {
                if (xml->writeTo(file)) { loadPresetsFromDirectory(); }
            }
        }
    };

    randomButton.onClick = [this]() {
        auto &rng = juce::Random::getSystemRandom();
        for (auto child : apvts.state) {
            if (child.hasProperty("id")) {
                juce::String paramID = child.getProperty("id").toString();
                if (auto *param = apvts.getParameter(paramID)) {
                    float randomNormalizedValue = rng.nextFloat();
                    param->setValueNotifyingHost(randomNormalizedValue);
                }
            }
        }
    };
}
