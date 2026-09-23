#pragma once
#include "ui/style/plugin_theme.h"
#include "ui/style/svg.h"
#include "ui/style/tooltips.h"
#include <JuceHeader.h>

struct Preset {
    juce::String name;
    juce::File file;
};

class PresetMenu : public juce::Component, public juce::ListBoxModel {
public:
    PresetMenu(juce::AudioProcessorValueTreeState &vts);
    ~PresetMenu() override = default;

    void SettingsFile(juce::File settingsFile);
    void loadPresetsFromDirectory();
    void paint(juce::Graphics &g) override;
    void resized() override;
    void setMenuOpen(bool isOpen);
    bool isMenuOpen() const { return isPresetsVisible; }
    void updateMenuVisibility();
    void updateIconColors(juce::Colour normal, juce::Colour hover);
    void setThemeStyle(
        juce::Colour newColor, juce::Font newFont, juce::Colour newHighlightBg, juce::Colour newHighlightText) {
        textColor = newColor;
        textFont = newFont;
        highlightColor = newHighlightBg;
        highlightTextColor = newHighlightText;
        repaint();
    }
    void setPresetDirectory(const juce::File &folder);
    std::function<void(bool)> onPresetsToggled;

    // ListBoxModel overrides
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent &) override;
    std::function<void()> onFolderIconClicked;

private:
    void presetCallbacks();

    juce::AudioProcessorValueTreeState &apvts;
    juce::File presetDirectory;
    juce::Colour textColor = juce::Colours::white;
    juce::Font textFont;
    juce::Colour highlightColor;
    juce::Colour highlightTextColor;
    // --- Top Menu Buttons ---
    juce::DrawableButton presetsButton{"Presets", juce::DrawableButton::ImageFitted};
    std::unique_ptr<juce::DrawablePath> drawableList, drawableListHover;

    // --- Inner Preset Panel Buttons ---
    juce::DrawableButton folderButton{"Folder", juce::DrawableButton::ImageFitted};
    std::unique_ptr<juce::DrawablePath> drawableFolder, drawableFolderHover;
    juce::DrawableButton saveButton{"Save", juce::DrawableButton::ImageFitted};
    std::unique_ptr<juce::DrawablePath> drawableSave, drawableSaveHover;
    juce::DrawableButton deleteButton{"Delete", juce::DrawableButton::ImageFitted};
    std::unique_ptr<juce::DrawablePath> drawableDelete, drawableDeleteHover;
    juce::DrawableButton randomButton{"Random", juce::DrawableButton::ImageFitted};
    std::unique_ptr<juce::DrawablePath> drawableRandom, drawableRandomHover;

    // --- UI Components ---
    juce::TextEditor saveTextBox;
    juce::ListBox presetList;

    // State & Data
    bool isPresetsVisible = false;
    std::vector<Preset> presets;

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetMenu)
};