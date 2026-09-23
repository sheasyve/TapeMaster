#include "knob_processing.h"

// --- The Core Knob Logic ---

namespace Knobs {

void CustomKnob::mouseDown(const juce::MouseEvent &e) {
            juce::Slider::mouseDown(e);
            dragStartValue = getValue();
}

void CustomKnob::mouseDrag(const juce::MouseEvent &e) {
    // -- Custom Drag Logic for Knobs with Modifier Keys ---
    if (isEnabled()) {
        if (e.mods.isShiftDown() || e.mods.isCtrlDown() || e.mods.isCommandDown()) {
            int deltaY = -e.getDistanceFromDragStartY();
            int deltaX = e.getDistanceFromDragStartX();
            int totalDeltaPixels = deltaY + deltaX;
            double normalizedSensitivity = 0.001; 
            double startProportion = valueToProportionOfLength(dragStartValue);
            double newProportion = startProportion + (totalDeltaPixels * normalizedSensitivity);
            newProportion = juce::jlimit(0.0, 1.0, newProportion);
            double newValue = proportionOfLengthToValue(newProportion);
            if (getInterval() > 0.0) {
                double snap = getInterval();
                newValue = std::round(newValue / snap) * snap;
            }
            newValue = juce::jlimit(getMinimum(), getMaximum(), newValue);
            setValue(newValue, juce::sendNotificationSync);
        } else {
            juce::Slider::mouseDrag(e);
            dragStartValue = getValue();
        }
    }
}

juce::NormalisableRange<float> makeCustomSkewRange(float start, float end, float skewFactor) {
    return juce::NormalisableRange<float>(start, end, 0.0f, skewFactor);
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() { // Define the knob functionality and parameters.
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // HPF - Custom Range
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"HPF", 1},
        "High Pass",
        makeCustomSkewRange(0.0f, 20000.0f, 0.3f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) {
                return juce::String(juce::roundToInt(value)) + " Hz";
            })
            .withValueFromStringFunction([](const juce::String &text) { 
                return text.removeCharacters(" Hz").getFloatValue(); 
            })));

    // BITS - Continuous interval 
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"BITS", 1},
        "Bit Depth",
        juce::NormalisableRange<float>(1.0f, 16.0f, 0.0f),
        16.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) {
                juce::String text = juce::String(value, 1);
                return text.length() > 6 ? text.substring(0, 6) : text;
            })
            .withValueFromStringFunction([](const juce::String &text) { return text.getFloatValue(); })));

    // RATE - Custom Range
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"RATE", 1},
        "Sample Rate",
        makeCustomSkewRange(1.0f, 44.1f, 0.6f),
        44.1f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) {
                if (std::abs(value) < 0.001f) value = 0.0f;
                juce::String text = juce::String(value, 2);
                return text.length() > 6 ? text.substring(0, 6) : text;
            })
            .withValueFromStringFunction([](const juce::String &text) { return text.getFloatValue(); })));

    // LPF - Custom Range
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"LPF", 1},
        "Low Pass",
        makeCustomSkewRange(20.0f, 20000.0f, 0.3f),
        20000.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) {
                // Round to int to strip decimals, then append Hz
                return juce::String(juce::roundToInt(value)) + " Hz";
            })
            .withValueFromStringFunction([](const juce::String &text) { 
                return text.removeCharacters(" Hz").getFloatValue(); 
            })));

    // MIX - Continuous interval (0.0f)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"MIX", 1},
        "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f),
        1.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](float value, int) {
                float mixed = value * 100.0f;
                if (std::abs(mixed) < 0.01f) mixed = 0.0f;
                juce::String text = juce::String(mixed, 1) + "%";
                return text.length() > 6 ? text.substring(0, 6) : text;
            })
            .withValueFromStringFunction([](const juce::String &text) { return text.getFloatValue() / 100.0f; })));
            
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"THEME_ID", 1}, "Theme ID", 1, 8, 1));
    return layout;
}

} // namespace Knobs