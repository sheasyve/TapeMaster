#include "plugin_processor.h"

// --- Core Audio Logic ---

void MyPluginProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto actualBufferChannels = buffer.getNumChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        if (i < actualBufferChannels) buffer.clear(i, 0, buffer.getNumSamples());
    }
    // --- Parameters---
    auto *rawBits = apvts.getRawParameterValue("BITS");
    float bits = (rawBits != nullptr) ? rawBits->load() : 16.0f;
    float targetRateKHz = apvts.getRawParameterValue("RATE")->load();
    float targetRateHz = targetRateKHz * 1000.0f;
    int rate = std::max(1, static_cast<int>(getSampleRate() / targetRateHz));
    if (rate < 1) rate = 1;
    float totalLevels = std::pow(2.0f, bits);
    if (heldSamples.size() < static_cast<size_t>(totalNumInputChannels))
        heldSamples.resize(totalNumInputChannels, 0.0f);
    if (sampleCounters.size() < static_cast<size_t>(totalNumInputChannels))
        sampleCounters.resize(totalNumInputChannels, 0);
    int channelsToProcess = std::min(totalNumInputChannels, actualBufferChannels);
    float mix = apvts.getRawParameterValue("MIX")->load();
    float sampleRate = getSampleRate();
    if (sampleRate > 0.0) setFilters(buffer, midiMessages, mix, sampleRate, channelsToProcess);
    for (int channel = 0; channel < channelsToProcess; ++channel) { // Call the Processor for each channel
        process(buffer, midiMessages, channel, channelsToProcess, rate, totalLevels, mix);
    }
}

void MyPluginProcessor::process(juce::AudioBuffer<float> &buffer,
    juce::MidiBuffer &midiMessages,
    int channel, int channelsToProcess,
    int rate, float totalLevels, float mix) {

    // --- Main Audio Processing Loop ---
    auto *channelData = buffer.getWritePointer(channel);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        float drySample = channelData[sample];
        if (sampleCounters[channel] % rate ==
            0) { // Sample and Hold: Only update held amplitude if the counter hits rate
            heldSamples[channel] = std::round(drySample * totalLevels) / totalLevels;
        }
        float wetSample = heldSamples[channel];
        if (channel < 2) { // Apply filters to the crushed audio (only processing L and R channels)
            wetSample = highPassFilters[channel].processSingleSampleRaw(wetSample);
            wetSample = lowPassFilters[channel].processSingleSampleRaw(wetSample);
        }
        // Blend the dry and wet signals based on the mix knob position
        channelData[sample] = (drySample * (1.0f - mix)) + (wetSample * mix);
        sampleCounters[channel]++;
    }
}

void MyPluginProcessor::setFilters(juce::AudioBuffer<float> &buffer,
    juce::MidiBuffer &midiMessages,
    float mix,
    float sampleRate,
    int channelsToProcess) {
    float hpFreq = apvts.getRawParameterValue("HPF")->load();
    float lpFreq = apvts.getRawParameterValue("LPF")->load();
    float maxFreq = sampleRate / 2.0f * 0.99f; // Prevent the low pass from exceeding the Nyquist limit
    auto hpCoeffs = juce::IIRCoefficients::makeHighPass(sampleRate, std::max(20.0f, hpFreq));
    auto lpCoeffs = juce::IIRCoefficients::makeLowPass(sampleRate, std::min(maxFreq, lpFreq));
    for (int i = 0; i < channelsToProcess; ++i) {
        if (i < 2) {
            highPassFilters[i].setCoefficients(hpCoeffs);
            lowPassFilters[i].setCoefficients(lpCoeffs);
        }
    }
}