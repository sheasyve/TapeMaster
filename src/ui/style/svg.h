#pragma once
#include <JuceHeader.h>

// --- SVG Parsing and Icon Management ---

inline auto parseSvgIcon = [](juce::DrawableButton &btn,
                               std::unique_ptr<juce::DrawablePath> &normal,
                               std::unique_ptr<juce::DrawablePath> &hover,
                               const char *svgData) {
    juce::Path path;
    if (auto xml = juce::XmlDocument::parse(svgData)) {
        if (auto pathElement = xml->getChildByName("path"))
            path = juce::Drawable::parseSVGPath(pathElement->getStringAttribute("d"));
    }
    normal = std::make_unique<juce::DrawablePath>();
    normal->setPath(path);
    hover = std::make_unique<juce::DrawablePath>();
    hover->setPath(path);
    btn.setImages(normal.get(), hover.get(), hover.get());
};

namespace SvgAssets {
    
static constexpr const char *listIcon =
    "<svg viewBox='0 0 24 24'><path d='M5 6h14a1 1 0 0 1 0 2H5a1 1 0 0 1 0-2zm0 5h14a1 1 0 0 "
    "1 0 2H5a1 1 0 0 1 0-2zm5 5h9a1 1 0 0 1 0 2H10a1 1 0 0 1 0-2z'/></svg>";

static constexpr const char *folderIcon = "<svg viewBox='0 0 24 24'><path d='M10 4H4c-1.1 0-1.99.9-1.99 2L2 18c0 1.1.9 "
                                          "2 2 2h16c1.1 0 2-.9 2-2V8c0-1.1-.9-2-2-2h-8l-2-2z'/></svg>";

static constexpr const char *saveIcon =
    "<svg viewBox='0 0 24 24'><path d='M17 3H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.1 0 2-.9 2-2V7l-4-4zm-5 "
    "16c-1.66 0-3-1.34-3-3s1.34-3 3-3 3 1.34 3 3-1.34 3-3 3zm3-10H5V5h10v4z'/></svg>";

static constexpr const char *gearIcon =
    "<svg viewBox='0 0 24 24'>"
    "<path d='M19.43 "
    "12.98c.04-.32.07-.64.07-.98s-.03-.66-.07-.98l2.11-1.65c.19-.15.24-.42.12-.64l-2-3.46c-.12-.22-.39-.3-.61-.22l-"
    "2.49 1c-.52-.4-1.08-.73-1.69-.98l-.38-2.65C14.46 2.18 14.25 2 14 2h-4c-.25 0-.46.18-.49.42l-.38 "
    "2.65c-.61.25-1.17.59-1.69.98l-2.49-1c-.23-.09-.49 0-.61.22l-2 3.46c-.13.22-.07.49-.12.64l2.11 "
    "1.65c-.04.32-.07.65-.07.98s.03.66.07.98l-2.11 1.65c-.19.15-.24.42-.12.64l2 "
    "3.46c.12.22.39.3.61.22l2.49-1c.52.4 1.08.73 1.69.98l.38 2.65c.03.24.24.42.49.42h4c.25 0 "
    ".46-.18.49-.42l.38-2.65c.61-.25 1.17-.59 1.69-.98l2.49 1c.23.09.49 0 "
    ".61-.22l2-3.46c.12-.22.07-.49-.12-.64l-2.11-1.65zM12 15.5c-1.93 0-3-1.57-3-3.5s1.07-3.5 3-3.5 3 1.57 3 "
    "3.5-1.07 3.5-3 3.5z'/>"
    "</svg>";

static constexpr const char *diceIcon = R"svg(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
  <path fill-rule="evenodd" d="M3 6C3 4.343 4.343 3 6 3H18C19.657 3 21 4.343 21 6V18C21 19.657 19.657 21 18 21H6C4.343 21 3 19.657 3 18V6ZM6 7.5A1.5 1.5 0 1 0 9 7.5A1.5 1.5 0 1 0 6 7.5ZM6 12A1.5 1.5 0 1 0 9 12A1.5 1.5 0 1 0 6 12ZM6 16.5A1.5 1.5 0 1 0 9 16.5A1.5 1.5 0 1 0 6 16.5ZM15 7.5A1.5 1.5 0 1 0 18 7.5A1.5 1.5 0 1 0 15 7.5ZM15 12A1.5 1.5 0 1 0 18 12A1.5 1.5 0 1 0 15 12ZM15 16.5A1.5 1.5 0 1 0 18 16.5A1.5 1.5 0 1 0 15 16.5Z"/>
</svg>
)svg";

static constexpr const char *deleteIcon =
    "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\">"
    "  <path d=\"M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z\"/>"
    "</svg>";

} // namespace SvgAssets
