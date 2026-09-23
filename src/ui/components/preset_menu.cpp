#include "preset_menu.h"

// --- Main File for the Preset Menu ---

PresetMenu::PresetMenu(juce::AudioProcessorValueTreeState &vts) : apvts(vts) {
    setInterceptsMouseClicks(false, true);

    // --- Presets List Button ---
    parseSvgIcon(presetsButton, drawableList, drawableListHover, SvgAssets::listIcon);
    presetsButton.setTooltip(PresetTooltips::menuToggle);
    addAndMakeVisible(presetsButton);

    // --- Folder Button ---
    parseSvgIcon(folderButton, drawableFolder, drawableFolderHover, SvgAssets::folderIcon);
    folderButton.setTooltip(PresetTooltips::folder);
    addChildComponent(folderButton);

    // --- Delete Button ---
    parseSvgIcon(deleteButton, drawableDelete, drawableDeleteHover, SvgAssets::deleteIcon);
    deleteButton.setTooltip(PresetTooltips::deletePreset);
    addChildComponent(deleteButton);

    // --- Random Button ---
    parseSvgIcon(randomButton, drawableRandom, drawableRandomHover, SvgAssets::diceIcon);
    randomButton.setTooltip(PresetTooltips::randomize);
    addChildComponent(randomButton);

    // --- Save  ---
    saveTextBox.setTextToShowWhenEmpty("New Preset Name...", juce::Colours::grey);
    saveTextBox.setMultiLine(false);
    saveTextBox.setReturnKeyStartsNewLine(false);
    saveTextBox.setTooltip(PresetTooltips::saveInput);
    addChildComponent(saveTextBox);
    parseSvgIcon(saveButton, drawableSave, drawableSaveHover, SvgAssets::saveIcon);
    saveButton.setTooltip(PresetTooltips::saveButton);
    addChildComponent(saveButton);

    // --- Scrollable List Box ---
    presetList.setModel(this);
    presetList.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    addChildComponent(presetList);

    presetCallbacks();
}

void PresetMenu::SettingsFile(juce::File settingsFile) {
    if (settingsFile.existsAsFile()) {
        if (std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(settingsFile)) {
            juce::String savedPath = xml->getStringAttribute("DataFolder");
            if (savedPath.isNotEmpty()) {
                juce::File savedDir(savedPath);
                if (savedDir.exists() && savedDir.isDirectory()) {
                    presetDirectory = savedDir.getChildFile("Presets");
                    if (!presetDirectory.exists()) presetDirectory.createDirectory();
                }
            }
        }
    }
}

void PresetMenu::loadPresetsFromDirectory() {
    presets.clear();
    juce::Array<juce::File> results;
    presetDirectory.findChildFiles(results, juce::File::findFiles, false, "*.preset");
    for (auto &f : results) { presets.push_back({f.getFileNameWithoutExtension(), f}); }
    presetList.updateContent();
}

void PresetMenu::setPresetDirectory(const juce::File &folder) {
    presetDirectory = folder;
    if (!presetDirectory.exists()) { presetDirectory.createDirectory(); }
    auto settingsFile = presetDirectory.getParentDirectory().getChildFile("settings.xml");
    SettingsFile(settingsFile);
    loadPresetsFromDirectory();
}

void PresetMenu::updateMenuVisibility() {
    deleteButton.setVisible(isPresetsVisible);
    saveTextBox.setVisible(isPresetsVisible);
    saveButton.setVisible(isPresetsVisible);
    randomButton.setVisible(isPresetsVisible);
    presetList.setVisible(isPresetsVisible);
    folderButton.setVisible(isPresetsVisible);
    resized();
    repaint();
}

void PresetMenu::setMenuOpen(bool isOpen) {
    isPresetsVisible = isOpen;
    updateMenuVisibility();
}

void PresetMenu::paint(juce::Graphics &g) {
    if (isPresetsVisible) {
        g.setColour(textColor);
        float titleSize = textFont.getHeight() * 1.2f;
        g.setFont(textFont.withHeight(titleSize).withStyle(juce::Font::bold));
        auto bounds = getLocalBounds();
        int textHeight = (int)titleSize + 10;
        int yPosition = (int)juce::jmap<float>(static_cast<float>(bounds.getHeight()), 340.0f, 600.0f, 40.0f, 79.0f);
        juce::Rectangle<int> titleArea(0, yPosition, bounds.getWidth(), textHeight);
        g.drawText("Presets", titleArea, juce::Justification::centred);
    }
}

void PresetMenu::resized() {
    auto bounds = getLocalBounds();
    float dynamicFontSize = bounds.getHeight() * 0.04f;
    textFont = textFont.withHeight(dynamicFontSize);
    saveTextBox.setFont(textFont.withHeight(dynamicFontSize * 1.0f));
    int dynamicRowHeight = juce::roundToInt(dynamicFontSize * 1.8f);
    presetList.setRowHeight(dynamicRowHeight);

    // --- Top Right Stack ---
    presetsButton.setBounds(bounds.getWidth() - 30, 5, 25, 25);
    randomButton.setBounds(bounds.getWidth() - 27.5, 30, 22.5, 22.5);
    saveButton.setBounds(bounds.getWidth() - 27.5, 55, 22.5, 22.5);

    // --- Top Left Stack ---
    folderButton.setBounds(5, 30, 25, 25);
    deleteButton.setBounds(5, 55, 25, 25);

    if (isPresetsVisible) {
        int menuWidth = (int)(bounds.getWidth() * 0.7f);
        int menuHeight = (int)(bounds.getHeight() * 0.7f);
        auto overlayArea = bounds.withSizeKeepingCentre(menuWidth, menuHeight);
        overlayArea.translate(0, 25);
        auto topBar = overlayArea.removeFromTop(dynamicRowHeight);
        saveTextBox.setBounds(topBar.reduced(10, 0));
        overlayArea.removeFromTop(10);
        presetList.setBounds(overlayArea);
    }
}

void PresetMenu::updateIconColors(juce::Colour normal, juce::Colour hover) {
    juce::Colour brightNormal = normal.brighter(0.3f);
    juce::Colour brightHover = hover.brighter(0.3f);
    if (drawableList != nullptr) {
        drawableList->setFill(brightNormal);
        drawableListHover->setFill(brightHover);
        presetsButton.setImages(drawableList.get(), drawableListHover.get(), drawableListHover.get());
    }
    if (drawableDelete != nullptr) {
        drawableDelete->setFill(brightNormal);
        drawableDeleteHover->setFill(brightHover);
        deleteButton.setImages(drawableDelete.get(), drawableDeleteHover.get(), drawableDeleteHover.get());
    }
    if (drawableSave != nullptr) {
        drawableSave->setFill(brightNormal);
        drawableSaveHover->setFill(brightHover);
        saveButton.setImages(drawableSave.get(), drawableSaveHover.get(), drawableSaveHover.get());
    }
    if (drawableRandom != nullptr) {
        drawableRandom->setFill(brightNormal);
        drawableRandomHover->setFill(brightHover);
        randomButton.setImages(drawableRandom.get(), drawableRandomHover.get(), drawableRandomHover.get());
    }
    if (drawableFolder != nullptr) {
        drawableFolder->setFill(brightNormal);
        drawableFolderHover->setFill(brightHover);
        folderButton.setImages(drawableFolder.get(), drawableFolderHover.get(), drawableFolderHover.get());
    }
}

int PresetMenu::getNumRows() { return (int)presets.size(); }

void PresetMenu::paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) {
    if (rowIsSelected) {
        g.fillAll(highlightColor);
        g.setColour(highlightTextColor);
    } else {
        g.setColour(textColor);
    }
    g.setFont(textFont);
    if (juce::isPositiveAndBelow(rowNumber, (int)presets.size())) {
        juce::String presetName = presets[rowNumber].file.getFileNameWithoutExtension();
        g.drawText(presetName, 8, 0, width - 16, height, juce::Justification::centredLeft, true);
    }
}

void PresetMenu::listBoxItemClicked(int row, const juce::MouseEvent &) {
    if (juce::isPositiveAndBelow(row, (int)presets.size())) {
        auto file = presets[row].file;
        std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(file);

        if (xml != nullptr) {
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
            juce::Logger::writeToLog("Loaded preset: " + presets[row].name);
            saveTextBox.setText(presets[row].name, juce::dontSendNotification);
        }
    }
}
