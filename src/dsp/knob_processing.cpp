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

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // 1. Wow Depth (Slow Warble Amount)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"WOW_DEPTH", 1},
        "Wow Depth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        15.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    // 2. Wow Rate (Slow Warble Speed)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"WOW_RATE", 1},
        "Wow Rate",
        juce::NormalisableRange<float>(0.1f, 5.0f, 0.01f, 0.5f),
        1.2f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")
    ));

    // 3. Flutter Depth (Rapid Shudder Amount)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FLUTTER_DEPTH", 1},
        "Flutter Depth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        10.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    // 4. Flutter Rate (Mechanical Frequency / Sprocket Rate)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FLUTTER_RATE", 1},
        "Flutter Rate",
        juce::NormalisableRange<float>(5.0f, 50.0f, 0.1f),
        24.0f, // 24 Hz default for standard film transport rate
        juce::AudioParameterFloatAttributes().withLabel("Hz")
    ));

    // 5. Tape Drive (Warm Saturation)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"TAPE_DRIVE", 1},
        "Drive",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")
    ));

    // 6. High Pass Filter (Sub-Rumble Cut)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"HPF", 1},
        "High Pass",
        juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f, 0.3f),
        20.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel("Hz")
            .withStringFromValueFunction([](float value, int) {
                if (value <= 20.05f) return juce::String("Off");
                return juce::String(juce::roundToInt(value)) + " Hz";
            })
            .withValueFromStringFunction([](const juce::String& text) {
                if (text.equalsIgnoreCase("Off")) return 20.0f;
                return text.upToFirstOccurrenceOf(" ", false, false).getFloatValue();
            })
    ));

    // 7. Low Pass Filter (Tape Head High-End Roll-off)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"LPF", 1},
        "Low Pass",
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f, 0.25f),
        20000.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel("Hz")
            .withStringFromValueFunction([](float value, int) {
                if (value >= 1000.0f)
                    return juce::String(value / 1000.0f, 1) + " kHz";
                return juce::String(juce::roundToInt(value)) + " Hz";
            })
            .withValueFromStringFunction([](const juce::String& text) {
                juce::String clean = text.trim();
                if (clean.endsWithIgnoreCase("k") || clean.endsWithIgnoreCase("khz"))
                    return clean.upToFirstOccurrenceOf("k", false, true).getFloatValue() * 1000.0f;
                return clean.upToFirstOccurrenceOf(" ", false, false).getFloatValue();
            })
    ));

    // 8. Dry / Wet Mix
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"MIX", 1},
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")
    ));

    return layout;
}

} // namespace Knobs