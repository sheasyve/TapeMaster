#include "dsp/plugin_processor.h"
#include "ui/main/plugin_editor.h"

// --- Main Editor onClick Logic ---

void MyPluginEditor::setupCallbacks() {

    // --- Theme Changed ---
    pluginSettings.onThemeChanged = [this](int selectedId) {
        updateTheme(selectedId);
        audioProcessor.saveThemeId(selectedId);
    };

    // --- Settings Menu Toggled (Closes Presets if open) ---
    pluginSettings.onSettingsToggled = [this](bool isOpen) {
        settingsOverlay.setVisible(isOpen);
        if (isOpen) {
            presetMenu.setMenuOpen(false);
            presetOverlay.setVisible(false);
        }
        bool isAnyMenuOpen = settingsOverlay.isVisible() || presetOverlay.isVisible();
        setLabelsVisible(!isAnyMenuOpen);
        repaint();
    };

    // --- Preset Menu Toggled (Closes Settings if open) ---
    presetMenu.onPresetsToggled = [this](bool isOpen) {
        presetOverlay.setVisible(isOpen);
        if (isOpen) {
            pluginSettings.setMenuOpen(false);
            settingsOverlay.setVisible(false);
        }
        bool isAnyMenuOpen = settingsOverlay.isVisible() || presetOverlay.isVisible();
        setLabelsVisible(!isAnyMenuOpen);
        repaint();
    };

    // --- Font Size Changed ---
    pluginSettings.onFontSizeChanged = [this](int selectedId) {
        updateFontSize(selectedId);
        audioProcessor.saveFontSizeId(selectedId);
    };

    // --- Tooltip Toggled ---
    pluginSettings.onTooltipToggled = [this](bool isEnabled) {
        updateTooltipState(isEnabled);
        audioProcessor.saveTooltipState(isEnabled);
    };

    // --- Data Folder Changed ---
    pluginSettings.onDataFolderChanged = [this](juce::File newFolder) {
        presetMenu.setPresetDirectory(newFolder.getChildFile("Presets"));
        themeManager.setThemesDirectory(newFolder.getChildFile("Themes"));
        pluginSettings.refreshThemeList();
        updateTheme(currentThemeId);
        repaint();
    };

    // --- Folder Icon Clicked ---
    presetMenu.onFolderIconClicked = [this]() { pluginSettings.launchFolderChooser(); };
}