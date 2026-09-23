#pragma once
#include <JuceHeader.h>
#include <map>
#include <vector>

namespace PluginTheme {

struct ThemeProps {
    juce::String name;
    int id { 1 };
    juce::String description;
    juce::Colour sliderFill;
    juce::Colour sliderOutline;
    juce::Colour sliderThumb;
    juce::Colour sliderTrack;
    juce::Colour labelText;
    juce::Colour labelShadow;
    juce::Colour altText;
    juce::Colour bgCenter;
    juce::Colour bgEdge;
    juce::Colour buttonColor;
    juce::Colour buttonHoverColor;
    juce::FontOptions labelFont;
    juce::Font logoFont;
    juce::Colour settingsOverlay;
    juce::Colour presetOverlay;
    juce::Colour scrollbarThumb;
};

struct SettingsBackground : public juce::Component {
    juce::Colour currentOverlayColor = juce::Colours::black.withAlpha(0.85f);
    void setOverlayColor(juce::Colour newColor) {
        currentOverlayColor = newColor;
        repaint();
    }
    void paint(juce::Graphics &g) override { g.fillAll(currentOverlayColor); }
};


class ThemeManager {
public:
    ThemeManager();
    ~ThemeManager() = default;
    void refreshThemes();
    ThemeProps getThemeProps(int themeId) const;
    std::vector<std::pair<juce::String, int>> getAvailableThemes() const;
    juce::File getThemesDirectory() const;
    void setThemesDirectory(const juce::File &newThemeDir);
    static ThemeProps loadThemeFromFile(const juce::File &file);
    static juce::Colour parseHexColour(const juce::String& hexString);

private:
    std::map<int, ThemeProps> cachedThemes;
    ThemeProps defaultTheme;
    juce::File customThemesDir;
    void loadDefaultFallbackTheme();
};

} // namespace PluginTheme