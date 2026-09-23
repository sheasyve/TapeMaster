#include "plugin_theme.h"
#include <algorithm>

// -- Theme Manager Implementation ---

namespace PluginTheme {

ThemeManager::ThemeManager() {
    loadDefaultFallbackTheme();
    refreshThemes();
}

juce::Colour ThemeManager::parseHexColour(const juce::String &hexString) {
    juce::String localHex = hexString;
    if (localHex.startsWithChar('#')) {
        localHex = localHex.substring(1);
        if (localHex.length() == 8) {
            juce::String alpha = localHex.substring(6, 8);
            juce::String rgb = localHex.substring(0, 6);
            localHex = alpha + rgb;
        } else if (localHex.length() == 6) {
            localHex = "FF" + localHex;
        }
        localHex = "0x" + localHex;
    }
    return juce::Colour::fromString(localHex);
}

ThemeProps ThemeManager::loadThemeFromFile(const juce::File &file) {
    ThemeProps theme;
    if (!file.existsAsFile()) return theme;

    var parsedJson = juce::JSON::parse(file);
    if (auto *obj = parsedJson.getDynamicObject()) {
        theme.name = obj->getProperty("name").toString();
        theme.description = obj->getProperty("description").toString();

        auto colorsVar = obj->getProperty("colors");
        if (auto *colors = colorsVar.getDynamicObject()) {
            theme.sliderFill = parseHexColour(colors->getProperty("sliderFill").toString());
            theme.sliderOutline = parseHexColour(colors->getProperty("sliderOutline").toString());
            theme.sliderThumb = parseHexColour(colors->getProperty("sliderThumb").toString());
            theme.sliderTrack = parseHexColour(colors->getProperty("sliderTrack").toString());
            theme.labelText = parseHexColour(colors->getProperty("labelText").toString());
            theme.labelShadow = parseHexColour(colors->getProperty("labelShadow").toString());
            theme.altText = parseHexColour(colors->getProperty("altText").toString());
            theme.bgCenter = parseHexColour(colors->getProperty("bgCenter").toString());
            theme.bgEdge = parseHexColour(colors->getProperty("bgEdge").toString());
            theme.buttonColor = parseHexColour(colors->getProperty("buttonColor").toString());
            theme.buttonHoverColor = parseHexColour(colors->getProperty("buttonHoverColor").toString());
            theme.settingsOverlay = parseHexColour(colors->getProperty("settingsOverlay").toString());
            theme.presetOverlay = parseHexColour(colors->getProperty("presetOverlay").toString());
            theme.scrollbarThumb = parseHexColour(colors->getProperty("scrollbarThumb").toString());
        }

        auto fontVar = obj->getProperty("font");
        if (auto *font = fontVar.getDynamicObject()) {
            juce::String fontName = font->getProperty("name").toString();
            float height = static_cast<float>(font->getProperty("height"));
            bool isBold = static_cast<bool>(font->getProperty("isBold"));
            bool isItalic = static_cast<bool>(font->getProperty("isItalic"));
            int styleFlags = juce::Font::plain;
            if (isBold) styleFlags |= juce::Font::bold;
            if (isItalic) styleFlags |= juce::Font::italic;
            theme.labelFont = juce::FontOptions(height, styleFlags).withName(fontName);
        }

        auto logoFontVar = obj->getProperty("logoFont");
        if (auto *logoFont = logoFontVar.getDynamicObject()) {
            juce::String fontName = logoFont->getProperty("name").toString();
            bool isBold = static_cast<bool>(logoFont->getProperty("isBold"));
            bool isItalic = static_cast<bool>(logoFont->getProperty("isItalic"));
            int styleFlags = juce::Font::plain;
            if (isBold) styleFlags |= juce::Font::bold;
            if (isItalic) styleFlags |= juce::Font::italic;
            theme.logoFont = juce::FontOptions(20.0f, styleFlags).withName(fontName);
        } else {
            theme.logoFont = juce::FontOptions(20.0f, juce::Font::bold).withName("Arial");
        }
    }
    return theme;
}

void ThemeManager::refreshThemes() {
    cachedThemes.clear();
    std::vector<ThemeProps> tempThemes;
    bool jsonDefaultFound = false;
    auto files = getThemesDirectory().findChildFiles(juce::File::findFiles, false, "*.json");
    for (const auto &file : files) {
        auto props = loadThemeFromFile(file);
        if (props.name.isNotEmpty()) { 
            if (props.name.equalsIgnoreCase("Panda Trueno") || 
                props.name.equalsIgnoreCase("Panda Trueno (Default)")) {
                jsonDefaultFound = true;
            }
            tempThemes.push_back(props); 
        }
    }

    if (!jsonDefaultFound) {
        loadDefaultFallbackTheme(); 
        tempThemes.push_back(defaultTheme);
    }

    std::sort(tempThemes.begin(), tempThemes.end(), [](const ThemeProps &a, const ThemeProps &b) {
        return a.name.compareIgnoreCase(b.name) < 0;
    });

    int currentId = 1;
    for (auto &theme : tempThemes) {
        theme.id = currentId;
        cachedThemes[currentId] = theme;
        if (theme.name.equalsIgnoreCase("Panda Trueno") || 
            theme.name.equalsIgnoreCase("Panda Trueno (Default)")) {
            defaultTheme = theme;
        }
        
        currentId++;
    }
}

ThemeProps ThemeManager::getThemeProps(int themeId) const {
    auto it = cachedThemes.find(themeId);
    if (it != cachedThemes.end()) return it->second;
    return defaultTheme;
}

std::vector<std::pair<juce::String, int>> ThemeManager::getAvailableThemes() const {
    std::vector<std::pair<juce::String, int>> list;
    for (const auto &[id, props] : cachedThemes) { list.push_back({props.name, id}); }
    return list;
}

void ThemeManager::loadDefaultFallbackTheme() {
    defaultTheme.name = "Panda Trueno (Default)";
    defaultTheme.id = 1;
    defaultTheme.sliderFill = juce::Colour(0xFFFFFFFF);
    defaultTheme.sliderOutline = juce::Colour(0xFF1A1A1A);
    defaultTheme.sliderThumb = juce::Colour(0xFFE50000);
    defaultTheme.sliderTrack = juce::Colour(0xFF333333);
    defaultTheme.labelText = juce::Colour(0xFFFFFFFF);
    defaultTheme.labelShadow = juce::Colour(0x7E2B2B2B);
    defaultTheme.altText = juce::Colour(0xFFAAAAAA);
    defaultTheme.bgCenter = juce::Colour(0xFF2B2B2B);
    defaultTheme.bgEdge = juce::Colour(0xFF050505);
    defaultTheme.buttonColor = juce::Colour(0xBDFFFFFF);
    defaultTheme.buttonHoverColor = juce::Colour(0xFFE50000);
    defaultTheme.labelFont = juce::FontOptions(15.0f, juce::Font::bold | juce::Font::italic).withName("Arial");
    defaultTheme.logoFont = juce::FontOptions(20.0f, juce::Font::bold).withName("Arial");
    defaultTheme.settingsOverlay = juce::Colours::black.withAlpha(0.90f);
    defaultTheme.presetOverlay = juce::Colours::black.withAlpha(0.80f);
    defaultTheme.scrollbarThumb = juce::Colour(0xFFE50000);
}

void ThemeManager::setThemesDirectory(const juce::File &newThemeDir) {
    customThemesDir = newThemeDir;
    if (!customThemesDir.exists()) { customThemesDir.createDirectory(); }
    refreshThemes();
}

juce::File ThemeManager::getThemesDirectory() const { return customThemesDir; }
} // namespace PluginTheme