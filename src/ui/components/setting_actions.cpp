#include "ui/components/plugin_settings.h"

// --- Settings Page onClick Logic ---

void PluginSettings::setupCallbacks() {

    themeSelector.onChange = [this] {
        int selectedId = themeSelector.getSelectedId();
        auto props = themeManager.getThemeProps(selectedId);
        juce::String newTooltip = props.description + "\n\n" + juce::String(SettingsTooltips::theme);
        themeSelector.setTooltip(newTooltip);
        if (onThemeChanged != nullptr) { onThemeChanged(selectedId); }
    };

    fontSizeSelector.onChange = [this]() {
        if (onFontSizeChanged != nullptr) { onFontSizeChanged(fontSizeSelector.getSelectedId()); }
    };

    infoButton.onClick = [] { juce::URL("https://sheasyve.dev/simplecrush").launchInDefaultBrowser(); };

    tooltipToggle.onClick = [this]() {
        if (onTooltipToggled != nullptr) { onTooltipToggled(tooltipToggle.getToggleState()); }
    };

    settingsButton.onClick = [this]() {
        isSettingsVisible = !isSettingsVisible;
        updateMenuVisibility();
    };

    folderButton.onClick = [this]() { launchFolderChooser(); };

}

void PluginSettings::launchFolderChooser() {
    fileChooser = std::make_unique<juce::FileChooser>("Select new Data folder (containing Themes and Presets)...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "");
    auto folderChooserFlags = juce::FileBrowserComponent::canSelectDirectories | juce::FileBrowserComponent::openMode;
    fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser &fc) {
        auto newFolder = fc.getResult();
        if (newFolder.isDirectory()) {
            if (newFolder.getFileName().equalsIgnoreCase("Presets") ||
                newFolder.getFileName().equalsIgnoreCase("Themes")) {
                newFolder = newFolder.getParentDirectory();
            }
            processor.saveDataFolder(newFolder.getFullPathName());
            if (onDataFolderChanged) { onDataFolderChanged(newFolder); }
        }
    });
}