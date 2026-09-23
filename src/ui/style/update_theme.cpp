#include "../main/plugin_editor.h"

// --- Theme Update Logic ---

void MyPluginEditor::updateTheme(int themeId) {
    currentThemeId = themeId;
    auto theme = themeManager.getThemeProps(themeId);
    themeLnF.currentFont = theme.labelFont.withHeight(getDynamicFontHeight());
    themeLnF.setColour(juce::ScrollBar::thumbColourId, theme.scrollbarThumb);

    // Standard Buttons
    themeLnF.setColour(juce::TextButton::buttonColourId, theme.buttonColor);
    themeLnF.setColour(juce::TextButton::buttonOnColourId, theme.buttonHoverColor);
    themeLnF.setColour(juce::TextButton::textColourOffId, theme.labelText);
    themeLnF.setColour(juce::TextButton::textColourOnId, theme.labelText);

    // Toggle Buttons / Checkboxes
    themeLnF.setColour(juce::ToggleButton::tickColourId, theme.buttonColor);
    themeLnF.setColour(juce::ToggleButton::tickDisabledColourId, theme.buttonColor.withAlpha(0.5f));
    themeLnF.setColour(juce::ToggleButton::textColourId, theme.labelText);

    // ComboBoxes (Dropdown Menus) & Popups
    themeLnF.setColour(juce::ComboBox::backgroundColourId, theme.bgCenter);
    themeLnF.setColour(juce::ComboBox::outlineColourId, theme.buttonHoverColor);
    themeLnF.setColour(juce::ComboBox::arrowColourId, theme.buttonHoverColor);
    themeLnF.setColour(juce::ComboBox::textColourId, theme.labelText);
    themeLnF.setColour(juce::PopupMenu::textColourId, theme.labelText);
    themeLnF.setColour(juce::PopupMenu::highlightedTextColourId, theme.bgCenter);
    themeLnF.setColour(juce::PopupMenu::backgroundColourId, theme.bgCenter);
    themeLnF.setColour(juce::PopupMenu::highlightedBackgroundColourId, theme.buttonHoverColor);
    themeLnF.setColour(juce::TooltipWindow::backgroundColourId, theme.bgCenter.withAlpha(0.8f));
    themeLnF.setColour(juce::TooltipWindow::textColourId, theme.labelText);
    themeLnF.setColour(juce::TooltipWindow::outlineColourId, theme.bgCenter.withAlpha(0.8f));

    sendLookAndFeelChange();

    auto applyRotaryTheme = [&](juce::Slider &slider) {
        slider.setColour(juce::Slider::rotarySliderFillColourId, theme.sliderFill);
        slider.setColour(juce::Slider::rotarySliderOutlineColourId, theme.sliderTrack);
        slider.setColour(juce::Slider::trackColourId, theme.sliderTrack);
        slider.setColour(juce::Slider::thumbColourId, theme.sliderThumb);
        slider.setColour(juce::Slider::textBoxTextColourId, theme.labelText);
    };

    auto applyLabelTheme = [&](juce::Label &label) { label.setColour(juce::Label::textColourId, theme.labelText); };

    // Rotary Sliders
    applyRotaryTheme(pluginControls.hpSlider);
    applyRotaryTheme(pluginControls.lpSlider);
    applyRotaryTheme(pluginControls.bitSlider);
    applyRotaryTheme(pluginControls.rateSlider);

    // Mix Slider
    pluginControls.mixSlider.setColour(juce::Slider::trackColourId, theme.sliderFill);
    pluginControls.mixSlider.setColour(juce::Slider::backgroundColourId, theme.sliderTrack);
    pluginControls.mixSlider.setColour(juce::Slider::thumbColourId, theme.sliderThumb);
    pluginControls.mixSlider.setColour(juce::Slider::textBoxTextColourId, theme.labelText);

    // Main UI Labels
    applyLabelTheme(pluginControls.hpLabel);
    applyLabelTheme(pluginControls.lpLabel);
    applyLabelTheme(pluginControls.bitLabel);
    applyLabelTheme(pluginControls.rateLabel);
    applyLabelTheme(pluginControls.mixLabel);

    // Overlay Labels
    applyLabelTheme(pluginSettings.themeLabel);
    applyLabelTheme(pluginSettings.fontSizeLabel);
    applyLabelTheme(pluginSettings.infoLabel);

    // Icons
    pluginSettings.updateIconColors(theme.buttonColor, theme.buttonHoverColor);
    presetMenu.updateIconColors(theme.buttonColor, theme.buttonHoverColor);

    // Menu Styling & Fonts
    pluginSettings.setThemeStyle(theme.labelText, themeLnF.currentFont);
    presetMenu.setThemeStyle(theme.labelText, themeLnF.currentFont, theme.buttonHoverColor, theme.altText);

    // Overlay Background Colors 
    settingsOverlay.setOverlayColor(theme.settingsOverlay);
    presetOverlay.setOverlayColor(theme.presetOverlay);

    // Refresh UI
    repaint();
}