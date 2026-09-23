#include "ui/main/plugin_editor.h"
#include "dsp/plugin_processor.h"

// --- Main UI Controller File ---

MyPluginEditor::MyPluginEditor(MyPluginProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p), presetMenu(p.apvts), pluginControls(p.apvts) {
    juce::File savedDataFolder(audioProcessor.getSavedDataFolder());
    presetMenu.setPresetDirectory(savedDataFolder.getChildFile("Presets"));
    themeManager.setThemesDirectory(savedDataFolder.getChildFile("Themes")); 
    pluginSettings.refreshThemeList();
    pluginSettings.onDataFolderChanged = [this](juce::File newFolder) {
        presetMenu.setPresetDirectory(newFolder.getChildFile("Presets"));
        themeManager.setThemesDirectory(newFolder.getChildFile("Themes")); 
        pluginSettings.refreshThemeList();                                
        repaint();
    };

    setLookAndFeel(&themeLnF);
    addAndMakeVisible(pluginControls);
    addChildComponent(settingsOverlay);
    addChildComponent(presetOverlay);
    addAndMakeVisible(pluginSettings);
    addAndMakeVisible(presetMenu);
    setupCallbacks();

    int initialTheme = audioProcessor.getSavedThemeId();
    pluginSettings.setInitialTheme(initialTheme);
    int initialFontSize = audioProcessor.getSavedFontSizeId();
    pluginSettings.setInitialFontSize(initialFontSize);
    currentFontSizeId = initialFontSize;

    bool initialTooltipState = audioProcessor.getSavedTooltipState();
    pluginSettings.setInitialTooltipState(initialTooltipState);
    updateTooltipState(initialTooltipState);
    updateTheme(initialTheme);

    setResizable(true, true);
    setResizeLimits(340, 340, 599, 599);
    getConstrainer()->setFixedAspectRatio(1.0);
    auto savedSize = audioProcessor.getWindowSize();
    setSize(savedSize.x, savedSize.y);
}

MyPluginEditor::~MyPluginEditor() {
    audioProcessor.saveWindowSize(getWidth(), getHeight());
    setLookAndFeel(nullptr);
}

void MyPluginEditor::paint(juce::Graphics &g) {
    auto themeProps = themeManager.getThemeProps(currentThemeId);
    auto center = getLocalBounds().getCentre().toFloat();
    float radius = juce::jmax(getWidth(), getHeight()) * 0.7f;
    juce::ColourGradient gradient(
        themeProps.bgCenter, center.x, center.y, themeProps.bgEdge, center.x, center.y + radius, true);
    g.setGradientFill(gradient);
    g.fillAll();
}

void MyPluginEditor::resized() {
    auto theme = themeManager.getThemeProps(currentThemeId);
    float dynamicFontHeight = getDynamicFontHeight();
    themeLnF.currentFont = theme.labelFont.withHeight(dynamicFontHeight);
    sendLookAndFeelChange();
    auto fullBounds = getLocalBounds();
    settingsOverlay.setBounds(fullBounds);
    presetOverlay.setBounds(fullBounds);
    pluginControls.setBounds(fullBounds);
    pluginSettings.setBounds(fullBounds);
    presetMenu.setBounds(fullBounds);
}

void MyPluginEditor::updateFontSize(int fontSizeId) {
    currentFontSizeId = fontSizeId;
    updateTheme(currentThemeId);
}

float MyPluginEditor::getDynamicFontHeight() const {
    float baseFontHeight = juce::jmap<float>(static_cast<float>(getWidth()), 300.0f, 900.0f, 14.0f, 22.0f);
    float fontMultiplier = 1.1f;                        // Normal
    if (currentFontSizeId == 2) fontMultiplier = 0.9f;  // Small
    if (currentFontSizeId == 3) fontMultiplier = 1.2f;  // Large
    if (currentFontSizeId == 4) fontMultiplier = 1.27f; // Extra Large
    return baseFontHeight * fontMultiplier;
}

void MyPluginEditor::paintOverChildren(juce::Graphics &g) {
    // Draw the plugin logo and name with dynamic scaling based on the window size and menu visibility
    bool isAnyMenuOpen = settingsOverlay.isVisible() || presetOverlay.isVisible();
    auto themeProps = themeManager.getThemeProps(currentThemeId);
    if (pluginControls.isVisible()) {
        float dynamicFontHeight = getWidth() * 0.025f;
        juce::Font logoFont = themeProps.logoFont;
        float logoScale = isAnyMenuOpen ? 2.5f : 4.0f;
        g.setFont(logoFont.withHeight(dynamicFontHeight * logoScale));
        juce::Rectangle<int> textBounds;
        if (isAnyMenuOpen) {
            int yPosition = (int)(getHeight() * 0.035f);
            int textHeight = (int)(dynamicFontHeight * logoScale * 1.5f);
            textBounds = juce::Rectangle<int>(0, yPosition, getWidth(), textHeight);
        } else {
            int standardOffset = (int)(getHeight() * 0.015f);
            textBounds = pluginControls.logoBounds.translated((int)(getWidth() * 0.02f), -standardOffset);
        }
        g.setColour(themeProps.labelShadow);
        g.drawText("SimpleCrush", textBounds.translated(2, 2), juce::Justification::centred);
        g.setColour(themeProps.labelText);
        g.drawText("SimpleCrush", textBounds, juce::Justification::centred);
    }
}

void MyPluginEditor::setLabelsVisible(bool shouldBeVisible) {
    pluginControls.hpLabel.setVisible(shouldBeVisible);
    pluginControls.lpLabel.setVisible(shouldBeVisible);
    pluginControls.bitLabel.setVisible(shouldBeVisible);
    pluginControls.rateLabel.setVisible(shouldBeVisible);
    pluginControls.mixLabel.setVisible(shouldBeVisible);
}

void MyPluginEditor::updateTooltipState(bool shouldShowTooltips) {
    // Update the tooltip window based on the user's preference
    if (shouldShowTooltips) {
        if (tooltipWindow == nullptr) {
            tooltipWindow = std::make_unique<juce::TooltipWindow>(this, 1700);
            tooltipWindow->setOpaque(false);
        }
    } else {
        tooltipWindow.reset();
    }
}