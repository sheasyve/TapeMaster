#include "ui/components/plugin_settings.h"

// --- The Main File for the Settings Page ---

PluginSettings::PluginSettings(PluginTheme::ThemeManager &tm, MyPluginProcessor &p) : themeManager(tm), processor(p) {
    juce::String infotext =
        "SimpleCrush v1.2.\n2026 Syverson Audio.\nAll rights reserved.\nDeveloped by Shea Syverson.";
    setInterceptsMouseClicks(false, true);
    parseSvgIcon(settingsButton, drawableGear, drawableGearHover, SvgAssets::gearIcon);
    settingsButton.setTooltip(SettingsTooltips::settingsBtn);
    addAndMakeVisible(settingsButton);
    themeLabel.setText("Theme", juce::dontSendNotification);
    themeLabel.setJustificationType(juce::Justification::centred);
    addChildComponent(themeLabel);
    themeSelector.setTooltip(SettingsTooltips::theme);
    addChildComponent(themeSelector);
    refreshThemeList();
    fontSizeLabel.setText("Font Size", juce::dontSendNotification);
    fontSizeLabel.setJustificationType(juce::Justification::centred);
    addChildComponent(fontSizeLabel);
    for (const auto &size : fontSizes) { fontSizeSelector.addItem(size.first, size.second); }
    fontSizeSelector.setTooltip(SettingsTooltips::fontSize);
    addChildComponent(fontSizeSelector);
    infoLabel.setText(infotext, juce::dontSendNotification);
    infoLabel.setJustificationType(juce::Justification::centred);
    addChildComponent(infoLabel);
    infoButton.onClick = [] { juce::URL("https://sheasyve.dev/simplecrush").launchInDefaultBrowser(); };
    infoButton.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    infoButton.setAlpha(0.0f);
    infoButton.setTooltip(SettingsTooltips::info);
    addChildComponent(infoButton);
    tooltipToggle.setTooltip(SettingsTooltips::tooltips);
    addChildComponent(tooltipToggle);
    parseSvgIcon(folderButton, drawableFolder, drawableFolderHover, SvgAssets::folderIcon);
    folderButton.setTooltip("Move your Presets and Themes folder");
    addChildComponent(folderButton);
    setupCallbacks();
}

void PluginSettings::updateMenuVisibility() {
    if (isSettingsVisible) refreshThemeList();
    themeLabel.setVisible(isSettingsVisible);
    themeSelector.setVisible(isSettingsVisible);
    fontSizeLabel.setVisible(isSettingsVisible);
    fontSizeSelector.setVisible(isSettingsVisible);
    infoLabel.setVisible(isSettingsVisible);
    infoButton.setVisible(isSettingsVisible);
    tooltipToggle.setVisible(isSettingsVisible);
    folderButton.setVisible(isSettingsVisible);
    if (onSettingsToggled != nullptr) onSettingsToggled(isSettingsVisible);
    resized();
    repaint();
}

void PluginSettings::paint(juce::Graphics &g) {
    if (isSettingsVisible) {
        g.setColour(textColor);
        auto bounds = getLocalBounds();
        float titleSize = bounds.getHeight() * 0.05f;
        g.setFont(textFont.withHeight(titleSize).withStyle(juce::Font::bold));
        int textHeight = (int)titleSize + 10;
        int yPosition = (int)juce::jmap<float>(static_cast<float>(bounds.getHeight()), 340.0f, 600.0f, 40.0f, 79.0f);
        juce::Rectangle<int> titleArea(0, yPosition, bounds.getWidth(), textHeight);
        g.drawText("Settings", titleArea, juce::Justification::centred);
    }
}

void PluginSettings::resized() {
    auto bounds = getLocalBounds();
    settingsButton.setBounds(5, 5, 25, 25);
    if (isSettingsVisible) {
        folderButton.setBounds(5, 30, 25, 25);
        int menuWidth = (int)(bounds.getWidth() * 0.75f);
        int menuHeight = (int)(bounds.getHeight() * 0.8f);
        auto overlayArea = bounds.withSizeKeepingCentre(menuWidth, menuHeight);
        int totalHeight = overlayArea.getHeight();
        overlayArea.removeFromTop(totalHeight * 0.13f);
        auto themeLabelArea = overlayArea.removeFromTop(totalHeight * 0.12f);
        themeLabel.setBounds(themeLabelArea);
        themeLabel.setFont(juce::Font(themeLabelArea.getHeight() * 0.6f, juce::Font::bold));
        int hPadding = (int)(menuWidth * 0.05f);
        themeSelector.setBounds(overlayArea.removeFromTop(totalHeight * 0.10f).reduced(hPadding, 0));
        overlayArea.removeFromTop(totalHeight * 0.04f);
        auto fontSizeLabelArea = overlayArea.removeFromTop(totalHeight * 0.10f);
        fontSizeLabel.setBounds(fontSizeLabelArea);
        fontSizeLabel.setFont(juce::Font(fontSizeLabelArea.getHeight() * 0.6f, juce::Font::bold));
        fontSizeSelector.setBounds(overlayArea.removeFromTop(totalHeight * 0.10f).reduced(hPadding, 0));
        overlayArea.removeFromTop(totalHeight * 0.04f);
        auto toggleArea = overlayArea.removeFromTop(totalHeight * 0.10f);
        tooltipToggle.setBounds(toggleArea.withSizeKeepingCentre(120, toggleArea.getHeight()));
        auto centeredInfoBounds = overlayArea.withSizeKeepingCentre(overlayArea.getWidth(), (int)(totalHeight * 0.30f));
        infoLabel.setBounds(centeredInfoBounds);
        infoButton.setBounds(centeredInfoBounds);
        infoLabel.setFont(juce::Font(centeredInfoBounds.getHeight() * 0.22f));
    }
}

void PluginSettings::setInitialTheme(int themeId) {
    auto props = themeManager.getThemeProps(themeId);
    int safeId = props.id;
    themeSelector.setSelectedId(safeId, juce::dontSendNotification);
    themeSelector.setTooltip(juce::String(SettingsTooltips::theme) + "\n\n" + props.description);
}

void PluginSettings::refreshThemeList() {
    int currentSelection = themeSelector.getSelectedId();
    themeSelector.clear(juce::dontSendNotification);
    auto availableThemes = themeManager.getAvailableThemes();
    for (const auto &[name, id] : availableThemes) { themeSelector.addItem(name, id); }
    if (currentSelection != 0) {
        auto props = themeManager.getThemeProps(currentSelection);
        themeSelector.setSelectedId(props.id, juce::dontSendNotification);
        themeSelector.setTooltip(juce::String(SettingsTooltips::theme) + "\n\n" + props.description);
    } else if (availableThemes.size() > 0) {
        auto props = themeManager.getThemeProps(-1); 
        themeSelector.setSelectedId(props.id, juce::dontSendNotification);
    }
}

void PluginSettings::setMenuOpen(bool isOpen) {
    isSettingsVisible = isOpen;
    updateMenuVisibility();
}

void PluginSettings::updateIconColors(juce::Colour normal, juce::Colour hover) {
    juce::Colour brightNormal = normal.brighter(0.3f);
    juce::Colour brightHover = hover.brighter(0.3f);
    if (drawableGear != nullptr && drawableGearHover != nullptr) {
        drawableGear->setFill(brightNormal);
        drawableGearHover->setFill(brightHover);
        settingsButton.setImages(drawableGear.get(), drawableGearHover.get(), drawableGearHover.get());
    }
    if (drawableFolder != nullptr) {
        drawableFolder->setFill(brightNormal);
        drawableFolderHover->setFill(brightHover);
        folderButton.setImages(drawableFolder.get(), drawableFolderHover.get(), drawableFolderHover.get());
    }
}

void PluginSettings::setInitialFontSize(int fontSizeId) {
    fontSizeSelector.setSelectedId(fontSizeId, juce::dontSendNotification);
}

void PluginSettings::setInitialTooltipState(bool isEnabled) {
    tooltipToggle.setToggleState(isEnabled, juce::dontSendNotification);
}
