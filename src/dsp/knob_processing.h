#pragma once
#include <JuceHeader.h>

namespace Knobs {
    juce::NormalisableRange<float> makeCustomSkewRange(float start, float end, float skewFactor);
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    class CustomKnob : public juce::Slider {
    public:
        CustomKnob() { 
            setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); 
        }
        void mouseDown(const juce::MouseEvent &e) override;
        void mouseDrag(const juce::MouseEvent &e) override;

    private:
        double dragStartValue = 0.0;
    };

} // namespace Knobs