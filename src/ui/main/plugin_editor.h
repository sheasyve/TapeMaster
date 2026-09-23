#pragma once
#include "dsp/plugin_processor.h"
#include "ui/components/plugin_settings.h"
#include "ui/components/preset_menu.h"
#include "ui/main/plugin_controls.h"
#include "ui/style/plugin_theme.h"
#include <JuceHeader.h>

class ThemeLookAndFeel : public juce::LookAndFeel_V4 {
public:
    juce::FontOptions currentFont;
    juce::Font getLabelFont(juce::Label &) override { return juce::Font(currentFont); }
};

class MyPluginEditor : public juce::AudioProcessorEditor {
public:
    MyPluginEditor(MyPluginProcessor &);
    ~MyPluginEditor() override;
    void updateTooltipState(bool shouldShowTooltips);
    void paint(juce::Graphics &) override;
    void paintOverChildren(juce::Graphics &) override;
    void resized() override;

private:
    PluginTheme::ThemeManager themeManager; 
    MyPluginProcessor &audioProcessor;
    ThemeLookAndFeel themeLnF;
    PresetMenu presetMenu;
    PluginControls pluginControls;
    PluginSettings pluginSettings{themeManager, audioProcessor};
    juce::Rectangle<int> logoBounds;
    PluginTheme::SettingsBackground settingsOverlay;
    PluginTheme::SettingsBackground presetOverlay;
    bool isSettingsVisible = false;
    bool isPresetsVisible = false;
    int currentThemeId = 1;
    int currentFontSizeId = 1;
    float getDynamicFontHeight() const;
    void updateTheme(int themeId);
    void updateFontSize(int fontSizeId);
    void setLabelsVisible(bool shouldBeVisible);
    void setupCallbacks();
    juce::DropShadow shadowProperties;
    juce::DropShadowEffect overlayShadow;
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;
    std::unique_ptr<juce::DrawablePath> drawableGear;
    std::unique_ptr<juce::DrawablePath> drawableGearHover;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> themeAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyPluginEditor)
};