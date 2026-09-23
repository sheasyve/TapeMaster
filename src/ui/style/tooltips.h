#pragma once

// --- Tooltip Definitions ---

struct PluginTooltips {
    static constexpr const char* highPass   = "Removes low frequencies.";
    static constexpr const char* lowPass    = "Removes high frequencies.";
    static constexpr const char* bitDepth   = "Reduces bit depth for classic bitcrushing distortion.";
    static constexpr const char* sampleRate = "Reduces sample rate for a lo-fi effect.";
    static constexpr const char* mix        = "Blends between the clean and crushed signals.";
};

struct PresetTooltips {
    static constexpr const char* menuToggle   = "Toggle preset browser.";
    static constexpr const char* folder       = "Select the folder containing the ""Presets"" and ""Themes"".";
    static constexpr const char* deletePreset = "Delete the selected preset.";
    static constexpr const char* randomize    = "Randomize all knob values.";
    static constexpr const char* saveInput    = "Enter a name for your new preset.";
    static constexpr const char* saveButton   = "Save current settings as a preset.";
};

struct SettingsTooltips {
    static constexpr const char* settingsBtn = "Toggle the settings menu.";
    static constexpr const char* theme       = "Add your own theme by placing a .json file in the 'Themes' folder.";
    static constexpr const char* fontSize    = "Adjust the size of the text and labels.";
    static constexpr const char* tooltips    = "Show or hide these hover hints.";
    static constexpr const char* info        = "Visit the developer's website.";
};
