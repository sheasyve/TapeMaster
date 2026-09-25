#include "plugin_controls.h"

PluginControls::PluginControls(juce::AudioProcessorValueTreeState &apvts) {
    setInterceptsMouseClicks(false, true);

    // Helper lambda to set up rotary sliders rapidly
    auto setupRotary = [this, &apvts](juce::Slider& slider, juce::Label& label, 
                                      std::unique_ptr<SliderAttachment>& attach, 
                                      const juce::String& paramId, const juce::String& name) {
        // slider.setLookAndFeel(&KnobLookAndFeel); // Uncomment if using a custom LookAndFeel
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(slider);

        label.setText(name, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);

        attach = std::make_unique<SliderAttachment>(apvts, paramId, slider);
    };

    // 1. Top Row: Modulation
    setupRotary(wowDepthSlider,     wowDepthLabel,     wowDepthAttachment,     "WOW_DEPTH",     "WOW DEPTH");
    setupRotary(wowRateSlider,      wowRateLabel,      wowRateAttachment,      "WOW_RATE",      "WOW RATE");
    setupRotary(flutterDepthSlider, flutterDepthLabel, flutterDepthAttachment, "FLUTTER_DEPTH", "FLUTTER DPTH");
    setupRotary(flutterRateSlider,  flutterRateLabel,  flutterRateAttachment,  "FLUTTER_RATE",  "FLUTTER RATE");

    // 2. Bottom Row: Tone & Drive
    setupRotary(driveSlider, driveLabel, driveAttachment, "TAPE_DRIVE", "DRIVE");
    setupRotary(hpSlider,    hpLabel,    hpAttachment,    "HPF",        "HIGH PASS");
    setupRotary(lpSlider,    lpLabel,    lpAttachment,    "LPF",        "LOW PASS");

    // 3. Right Side: Mix Slider (Vertical)
    mixSlider.setSliderStyle(juce::Slider::LinearVertical);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 20);
    mixSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(mixSlider);
    mixLabel.setText("MIX", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel);
    mixAttachment = std::make_unique<SliderAttachment>(apvts, "MIX", mixSlider);
}

PluginControls::~PluginControls() {
    // wowDepthSlider.setLookAndFeel(nullptr); // Clean up custom LookAndFeels here
    // (Repeat for all sliders if LookAndFeel is used)
}

void PluginControls::resized() {
    auto fullBounds = getLocalBounds();
    auto bounds = fullBounds.reduced(fullBounds.getWidth() * 0.05f);
    const int gap = bounds.getWidth() * 0.04f;

    int dynamicGap = fullBounds.getHeight() * 0.02f; 
    int labelBoxHeight = 20;

    // --- Mix Section (Right Side) ---
    int mixWidth = bounds.getWidth() * 0.15f;
    auto mixArea = bounds.removeFromRight(mixWidth);
    bounds.removeFromRight(gap); // Gap between rotaries and mix
    
    mixLabel.setBounds(mixArea.removeFromTop(labelBoxHeight));
    mixArea.removeFromTop(dynamicGap); 
    mixSlider.setBounds(mixArea);

    // --- Logo Area ---
    logoBounds = bounds.removeFromTop(bounds.getHeight() * 0.15f);
    bounds.removeFromTop(gap);

    // --- Rotary Knobs Grid ---
    auto layOutKnob = [&](juce::Slider& sl, juce::Label& lb, juce::Rectangle<int> area) {
        lb.setBounds(area.removeFromTop(labelBoxHeight));
        area.removeFromTop(dynamicGap);
        int size = juce::jmin(area.getWidth(), area.getHeight());
        sl.setBounds(area.withSizeKeepingCentre(size, size));
    };

    // Split remaining area into Top and Bottom rows
    auto topRowArea = bounds.removeFromTop(bounds.getHeight() * 0.48f);
    bounds.removeFromTop(bounds.getHeight() * 0.1f); // Vertical gap between rows
    auto bottomRowArea = bounds;

    // Top Row: 4 Knobs (Wow/Flutter)
    int topItemWidth = topRowArea.getWidth() / 4;
    layOutKnob(wowDepthSlider,     wowDepthLabel,     topRowArea.removeFromLeft(topItemWidth));
    layOutKnob(wowRateSlider,      wowRateLabel,      topRowArea.removeFromLeft(topItemWidth));
    layOutKnob(flutterDepthSlider, flutterDepthLabel, topRowArea.removeFromLeft(topItemWidth));
    layOutKnob(flutterRateSlider,  flutterRateLabel,  topRowArea.removeFromLeft(topItemWidth));

    // Bottom Row: 3 Knobs (Drive, HP, LP) - Centered visually
    int bottomItemWidth = bottomRowArea.getWidth() / 3;
    layOutKnob(driveSlider, driveLabel, bottomRowArea.removeFromLeft(bottomItemWidth));
    layOutKnob(hpSlider,    hpLabel,    bottomRowArea.removeFromLeft(bottomItemWidth));
    layOutKnob(lpSlider,    lpLabel,    bottomRowArea.removeFromLeft(bottomItemWidth));
}

void PluginControls::lookAndFeelChanged() {
    float currentFontHeight = getLookAndFeel().getLabelFont(mixLabel).getHeight();
    int dynamicHeight = juce::jmax(20, (int)currentFontHeight + 6);

    auto updateTextBox = [&](juce::Slider& s) { s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, dynamicHeight); };
    
    updateTextBox(wowDepthSlider);
    updateTextBox(wowRateSlider);
    updateTextBox(flutterDepthSlider);
    updateTextBox(flutterRateSlider);
    updateTextBox(driveSlider);
    updateTextBox(hpSlider);
    updateTextBox(lpSlider);
    updateTextBox(mixSlider);
}