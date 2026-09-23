#include "knob_look.h"

// --- Custom Look and Feel for the Knobs ---

void KnobLookAndFeel::drawRotarySlider(juce::Graphics &g,
    int x, int y, int width, int height,
    float sliderPos, const float rotaryStartAngle,
    const float rotaryEndAngle, juce::Slider &slider) {

    float minDimension = (float)juce::jmin(width, height);
    float lineThickness = juce::jlimit(5.0f, 18.0f, minDimension * 0.11f);
    auto radius = (minDimension / 2.0f) - (lineThickness / 2.0f) - 2.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    juce::PathStrokeType strokeType(lineThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);

    g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId).withAlpha(0.3f));
    g.strokePath(backgroundArc, strokeType);

    if (slider.isEnabled()) {
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(valueArc, strokeType);
    }

    auto thumbWidth = lineThickness * 1.2f;
    juce::Point<float> thumbPoint(centreX + radius * std::cos(angle - juce::MathConstants<float>::halfPi),
        centreY + radius * std::sin(angle - juce::MathConstants<float>::halfPi));

    g.setColour(slider.findColour(juce::Slider::thumbColourId));
    g.fillEllipse(thumbPoint.x - thumbWidth / 2.0f, thumbPoint.y - thumbWidth / 2.0f, thumbWidth, thumbWidth);
}