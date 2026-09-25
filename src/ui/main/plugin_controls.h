#pragma once
#include <JuceHeader.h>
// #include "look_and_feel.h" // Ensure your KnobLookAndFeel is accessible here

class PluginControls : public juce::Component {
public:
    PluginControls(juce::AudioProcessorValueTreeState &apvts);
    ~PluginControls() override;
    // --- Labels ---
    juce::Label wowDepthLabel;
    juce::Label wowRateLabel;
    juce::Label flutterDepthLabel;
    juce::Label flutterRateLabel;
    juce::Label driveLabel;
    juce::Label hpLabel;
    juce::Label lpLabel;
    juce::Label mixLabel;

    // --- Sliders ---
    juce::Slider wowDepthSlider;
    juce::Slider wowRateSlider;
    juce::Slider flutterDepthSlider;
    juce::Slider flutterRateSlider;
    juce::Slider driveSlider;
    juce::Slider hpSlider;
    juce::Slider lpSlider;
    juce::Slider mixSlider;

    juce::Rectangle<int> logoBounds;

    void resized() override;
    void lookAndFeelChanged() override;

private:
    // --- Attachments ---
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> wowDepthAttachment;
    std::unique_ptr<SliderAttachment> wowRateAttachment;
    std::unique_ptr<SliderAttachment> flutterDepthAttachment;
    std::unique_ptr<SliderAttachment> flutterRateAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> hpAttachment;
    std::unique_ptr<SliderAttachment> lpAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginControls)
};