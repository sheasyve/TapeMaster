#pragma once
#include <JuceHeader.h>
#include "../../dsp/knob_processing.h"
#include "ui/style/knob_look.h"
#include "ui/style/tooltips.h"

class PluginControls : public juce::Component {
public:
    PluginControls(juce::AudioProcessorValueTreeState &apvts);
    ~PluginControls();

    void resized() override;
    juce::Rectangle<int> logoBounds;
    Knobs::CustomKnob hpSlider;
    Knobs::CustomKnob lpSlider;
    Knobs::CustomKnob bitSlider;
    Knobs::CustomKnob rateSlider;
    Knobs::CustomKnob mixSlider;
    juce::Label hpLabel, lpLabel, bitLabel, rateLabel, mixLabel;

private:
    KnobLookAndFeel KnobLookAndFeel;
    void lookAndFeelChanged() override;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginControls)
};