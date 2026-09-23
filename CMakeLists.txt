#C:\Users\sheam\Documents\redux-plugin\SimpleCrush
cmake_minimum_required(VERSION 3.25)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)

project(SimpleCrush VERSION 1.3.1)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/std:c++latest>")

cmake_policy(SET CMP0177 NEW)
add_subdirectory(JUCE)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(DO_COPY TRUE)
    set(VST3_DIR "C:/Users/sheam/Documents/VST")
else()
    set(DO_COPY FALSE)
    set(VST3_DIR "")
endif()

juce_add_plugin(SimpleCrush
    COMPANY_NAME "Syverson Audio"
    BUNDLE_ID "com.SyversonAudio.SimpleCrush"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE "SyAu" 
    PLUGIN_CODE "Rdx4"
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "SimpleCrush"
    COPY_PLUGIN_AFTER_BUILD ${DO_COPY}
    VST3_COPY_DIR "${VST3_DIR}" 
)

juce_generate_juce_header(SimpleCrush)

target_sources(SimpleCrush PRIVATE
    src/dsp/plugin_processor.cpp
    src/dsp/plugin_processor.h
    src/dsp/audio_processing.cpp
    src/dsp/knob_processing.h
    src/dsp/knob_processing.cpp
    src/ui/main/plugin_editor.cpp
    src/ui/main/plugin_editor.h
    src/ui/main/editor_actions.cpp
    src/ui/main/plugin_controls.h
    src/ui/main/plugin_controls.cpp
    src/ui/style/plugin_theme.h
    src/ui/style/plugin_theme.cpp
    src/ui/style/update_theme.cpp
    src/ui/style/knob_look.h
    src/ui/style/knob_look.cpp
    src/ui/style/svg.h
    src/ui/components/plugin_settings.h
    src/ui/components/plugin_settings.cpp
    src/ui/components/setting_actions.cpp
    src/ui/components/preset_menu.h
    src/ui/components/preset_menu.cpp
    src/ui/components/preset_actions.cpp
)

target_link_libraries(SimpleCrush PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp)
    
target_include_directories(SimpleCrush PRIVATE src)
target_compile_definitions(SimpleCrush PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0)