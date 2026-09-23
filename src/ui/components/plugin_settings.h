#pragma once
#include "ui/style/plugin_theme.h"
#include "ui/style/svg.h"
#include "ui/style/tooltips.h"
#include "dsp/plugin_processor.h"
#include <JuceHeader.h>

const std::vector<std::pair<juce::String, int>> fontSizes = {
    {"Normal", 1}, {"Small", 2}, {"Large", 3}, {"Extra Large", 4}};

class PluginSettings : public juce::Component {
public:
    PluginSettings(PluginTheme::ThemeManager &tm, MyPluginProcessor& p); 
    ~PluginSettings() override = default;
    
    void paint(juce::Graphics &g) override;
    void resized() override;
    void setInitialTheme(int themeId);
    void setInitialFontSize(int fontSizeId);
    void updateIconColors(juce::Colour normal, juce::Colour hover);
    void setMenuOpen(bool isOpen);
    bool isMenuOpen() const { return isSettingsVisible; }

    void refreshThemeList();

    // Callbacks
    std::function<void(bool)> onSettingsToggled;
    std::function<void(int)> onThemeChanged;
    std::function<void(int)> onFontSizeChanged;

    // State helper
    void updateMenuVisibility();

    // Components
    juce::Colour textColor = juce::Colours::white;
    juce::Font textFont;
    juce::DrawableButton settingsButton{"Settings", juce::DrawableButton::ImageFitted};
    juce::Label themeLabel;
    juce::ComboBox themeSelector;
    juce::Label fontSizeLabel;
    juce::ComboBox fontSizeSelector;
    juce::Label infoLabel;
    juce::TextButton infoButton;
    std::unique_ptr<juce::DrawablePath> drawableGear;
    std::unique_ptr<juce::DrawablePath> drawableGearHover;
    juce::ToggleButton tooltipToggle{"Show Tooltips"};
    std::function<void(bool)> onTooltipToggled;
    juce::DrawableButton folderButton{"Folder", juce::DrawableButton::ImageFitted};
    std::unique_ptr<juce::DrawablePath> drawableFolder, drawableFolderHover;
    std::unique_ptr<juce::FileChooser> fileChooser;
    std::function<void(const juce::File&)> onDataFolderChanged;

    void setInitialTooltipState(bool isEnabled);
    void setThemeStyle(juce::Colour newColor, juce::Font newFont) {
        textColor = newColor;
        textFont = newFont;
        repaint();
    }
    void launchFolderChooser();

private:
    void setupCallbacks();
    MyPluginProcessor& processor;
    bool isSettingsVisible = false;
    bool isPresetsVisible = false;
    PluginTheme::ThemeManager& themeManager;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginSettings)
};