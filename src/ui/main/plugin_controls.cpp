#include "plugin_controls.h"

// --- Plugin Controls on the Main Page ---

PluginControls::PluginControls(juce::AudioProcessorValueTreeState &apvts) {
    setInterceptsMouseClicks(false, true);
    hpSlider.setLookAndFeel(&KnobLookAndFeel);
    lpSlider.setLookAndFeel(&KnobLookAndFeel);
    bitSlider.setLookAndFeel(&KnobLookAndFeel);
    rateSlider.setLookAndFeel(&KnobLookAndFeel);
    
    // --- High Pass Knob ---
    hpSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    hpSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    hpSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    hpSlider.setTooltip(PluginTooltips::highPass);
    addAndMakeVisible(hpSlider);
    hpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "HPF", hpSlider);
    hpLabel.setText("HIGH PASS", juce::dontSendNotification);
    hpLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(hpLabel);

    // --- Low Pass Knob ---
    lpSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lpSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    lpSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    lpSlider.setTooltip(PluginTooltips::lowPass);
    addAndMakeVisible(lpSlider);
    lpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "LPF", lpSlider);
    lpLabel.setText("LOW PASS", juce::dontSendNotification);
    lpLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lpLabel);

    // --- Bit Depth Knob ---
    bitSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bitSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    bitSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    bitSlider.setTooltip(PluginTooltips::bitDepth);
    addAndMakeVisible(bitSlider);
    bitAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "BITS", bitSlider);
    bitLabel.setText("BIT DEPTH", juce::dontSendNotification);
    bitLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bitLabel);

    // --- Sample Rate Knob ---
    rateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    rateSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    rateSlider.setTooltip(PluginTooltips::sampleRate);
    addAndMakeVisible(rateSlider);
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "RATE", rateSlider);
    rateLabel.setText("SAMPLE RATE", juce::dontSendNotification);
    rateLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(rateLabel);

    // --- Mix Knob ---
    mixSlider.setSliderStyle(juce::Slider::LinearVertical);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 20);
    mixSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    mixSlider.setTooltip(PluginTooltips::mix);
    addAndMakeVisible(mixSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "MIX", mixSlider);
    mixLabel.setText("MIX", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel);
}

PluginControls::~PluginControls() {
    hpSlider.setLookAndFeel(nullptr);
    lpSlider.setLookAndFeel(nullptr);
    bitSlider.setLookAndFeel(nullptr);
    rateSlider.setLookAndFeel(nullptr);
}

void PluginControls::resized() {
    auto fullBounds = getLocalBounds();
    auto bounds = fullBounds.reduced(fullBounds.getWidth() * 0.066f);
    const int gap = bounds.getWidth() * 0.05f;

    // 1. Calculate a gap that grows proportionally with the plugin window
    int dynamicGap = fullBounds.getHeight() * 0.025f; 
    int labelBoxHeight = 24; // Height for the text itself
    int totalTopSpace = labelBoxHeight + dynamicGap;

    // --- Mix Section (Right Side) ---
    int mixWidth = bounds.getWidth() * 0.17f;
    auto mixPanel = bounds.removeFromRight(mixWidth + gap);
    mixPanel.removeFromRight(gap);
    auto mixArea = mixPanel;
    mixLabel.setBounds(mixArea.removeFromTop(labelBoxHeight));
    mixArea.removeFromTop(dynamicGap); 
    mixSlider.setBounds(mixArea);
    logoBounds = bounds.removeFromTop(bounds.getHeight() * 0.12f);

    // --- Top Area: Filters (Small & Centered) ---
    auto topArea = bounds.removeFromTop(bounds.getHeight() * 0.45f);
    int maxTopW = topArea.getWidth() * 0.35f;
    int maxTopH = topArea.getHeight() - totalTopSpace;
    int filterKnobSize = jmin(maxTopW, maxTopH);
    int topGap = gap / 3;
    int filterAreaWidth = (filterKnobSize * 2) + topGap;
    int filterAreaHeight = filterKnobSize + totalTopSpace;
    auto filterArea = topArea.withSizeKeepingCentre(filterAreaWidth, filterAreaHeight);
    auto hpArea = filterArea.removeFromLeft(filterKnobSize);
    hpLabel.setBounds(hpArea.removeFromTop(labelBoxHeight));
    hpArea.removeFromTop(dynamicGap);
    hpSlider.setBounds(hpArea);
    filterArea.removeFromLeft(topGap);
    auto lpArea = filterArea;
    lpLabel.setBounds(lpArea.removeFromTop(labelBoxHeight));
    lpArea.removeFromTop(dynamicGap); 
    lpSlider.setBounds(lpArea);
    bounds.removeFromTop(bounds.getHeight() * 0.10f);

    // --- Bottom Area: Main Controls (Big) ---
    auto bottomArea = bounds;
    int maxBottomW = bottomArea.getWidth() * 0.45f;
    int maxBottomH = bottomArea.getHeight() - totalTopSpace;
    int bottomKnobSize = jmin(maxBottomW, maxBottomH);
    int bottomGap = gap * 0.25f;
    int bottomTotalWidth = (bottomKnobSize * 2) + bottomGap;
    int bottomTotalHeight = bottomKnobSize + totalTopSpace;
    auto centeredBottomArea = bottomArea.withSizeKeepingCentre(bottomTotalWidth, bottomTotalHeight);
    auto bitArea = centeredBottomArea.removeFromLeft(bottomKnobSize);
    bitLabel.setBounds(bitArea.removeFromTop(labelBoxHeight));
    bitArea.removeFromTop(dynamicGap); 
    bitSlider.setBounds(bitArea);
    centeredBottomArea.removeFromLeft(bottomGap);
    auto rateArea = centeredBottomArea;
    rateLabel.setBounds(rateArea.removeFromTop(labelBoxHeight));
    rateArea.removeFromTop(dynamicGap); 
    rateSlider.setBounds(rateArea);
}

void PluginControls::lookAndFeelChanged() {
    float currentFontHeight = getLookAndFeel().getLabelFont(mixLabel).getHeight();
    int dynamicHeight = juce::jmax(20, (int)currentFontHeight + 6);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, dynamicHeight);
    hpSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, dynamicHeight);
    lpSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, dynamicHeight);
    bitSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, dynamicHeight);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, dynamicHeight);
}