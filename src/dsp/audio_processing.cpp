#include "plugin_processor.h"

// --- Core Audio Logic ---

inline float hermiteInterpolate(float y0, float y1, float y2, float y3, float frac) {
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
    return ((c3 * frac + c2) * frac + c1) * frac + c0;
}

void MyPluginProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto actualBufferChannels = buffer.getNumChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        if (i < actualBufferChannels) buffer.clear(i, 0, buffer.getNumSamples());
    }

    // --- Fetch Parameters ---
    float wowDepth     = apvts.getRawParameterValue("WOW_DEPTH")->load();
    float wowRate      = apvts.getRawParameterValue("WOW_RATE")->load();
    float flutterDepth = apvts.getRawParameterValue("FLUTTER_DEPTH")->load();
    float flutterRate  = apvts.getRawParameterValue("FLUTTER_RATE")->load();
    float driveDb      = apvts.getRawParameterValue("TAPE_DRIVE")->load();
    float mix          = apvts.getRawParameterValue("MIX")->load() / 100.0f; // Scale to 0.0 - 1.0

    float sampleRate = getSampleRate();
    int channelsToProcess = std::min(totalNumInputChannels, actualBufferChannels);

    if (sampleRate > 0.0) setFilters(buffer, midiMessages, mix, sampleRate, channelsToProcess);

    // Convert Drive dB to linear gain multiplier
    float driveLinear = juce::Decibels::decibelsToGain(driveDb);

    // Call the Processor for each channel
    for (int channel = 0; channel < channelsToProcess; ++channel) { 
        process(buffer, channel, channelsToProcess, wowDepth, wowRate, flutterDepth, flutterRate, driveLinear, mix, sampleRate);
    }

    if (sampleRate > 0.0) {
        int numSamples = buffer.getNumSamples();
        globalWowPhase += (juce::MathConstants<float>::twoPi * wowRate / sampleRate) * numSamples;
        globalWowPhase = std::fmod(globalWowPhase, juce::MathConstants<float>::twoPi);

        globalFlutterPhase += (juce::MathConstants<float>::twoPi * flutterRate / sampleRate) * numSamples;
        globalFlutterPhase = std::fmod(globalFlutterPhase, juce::MathConstants<float>::twoPi);
    }
}

void MyPluginProcessor::process(juce::AudioBuffer<float> &buffer,
    int channel, int channelsToProcess,
    float wowDepth, float wowRate, float flutterDepth, float flutterRate, float driveLinear, float mix, float sampleRate) {

    auto *channelData = buffer.getWritePointer(channel);
    auto *delayData = delayBuffer.getWritePointer(channel);
    int delayBufferSize = delayBuffer.getNumSamples();

    // Local phase copies so Left and Right channels trace the exact same modulation curve
    float currentWowPhase = globalWowPhase;
    float currentFlutterPhase = globalFlutterPhase;

    float wowPhaseIncr = (juce::MathConstants<float>::twoPi * wowRate) / sampleRate;
    float flutterPhaseIncr = (juce::MathConstants<float>::twoPi * flutterRate) / sampleRate;

    // Convert % depth into actual time deviations (e.g., max 5ms for wow, 1ms for flutter)
    float baseDelaySamples = 0.02f * sampleRate; // 20ms baseline delay
    float maxWowSamples = 0.005f * sampleRate * (wowDepth / 100.0f);
    float maxFlutterSamples = 0.001f * sampleRate * (flutterDepth / 100.0f);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        float drySample = channelData[sample];

        // 1. Write incoming audio to the ring buffer
        delayData[writeIndices[channel]] = drySample;

        // 2. Compute physical tape speed deviation
        float lfoWow = std::sin(currentWowPhase) * maxWowSamples;
        float lfoFlutter = std::sin(currentFlutterPhase) * maxFlutterSamples;
        
        currentWowPhase += wowPhaseIncr;
        currentFlutterPhase += flutterPhaseIncr;

        // 3. Compute read position (modulo math to stay inside buffer)
        float currentDelay = baseDelaySamples + lfoWow + lfoFlutter;
        float readPos = static_cast<float>(writeIndices[channel]) - currentDelay;
        
        while (readPos < 0.0f) readPos += delayBufferSize;
        while (readPos >= delayBufferSize) readPos -= delayBufferSize;

        // 4. Retrieve 4 neighboring points for Hermite interpolation
        int iPos = static_cast<int>(readPos);
        float frac = readPos - static_cast<float>(iPos);

        int idx0 = (iPos - 1 + delayBufferSize) % delayBufferSize;
        int idx1 = iPos;
        int idx2 = (iPos + 1) % delayBufferSize;
        int idx3 = (iPos + 2) % delayBufferSize;

        float wetSample = hermiteInterpolate(
            delayData[idx0], delayData[idx1], delayData[idx2], delayData[idx3], frac
        );

        // 5. Tape Filters (Head frequency response)
        if (channel < 2) { 
            wetSample = highPassFilters[channel].processSingleSampleRaw(wetSample);
            wetSample = lowPassFilters[channel].processSingleSampleRaw(wetSample);
        }

        // 6. Magnetic Saturation (tanh)
        wetSample = std::tanh(wetSample * driveLinear);

        // 7. Dry/Wet Mix
        channelData[sample] = (drySample * (1.0f - mix)) + (wetSample * mix);

        // 8. Advance circular buffer write index
        writeIndices[channel] = (writeIndices[channel] + 1) % delayBufferSize;
    }
}

void MyPluginProcessor::setFilters(juce::AudioBuffer<float> &buffer,
    juce::MidiBuffer &midiMessages, float mix, float sampleRate, int channelsToProcess) {
    
    float hpFreq = apvts.getRawParameterValue("HPF")->load();
    float lpFreq = apvts.getRawParameterValue("LPF")->load();
    float maxFreq = sampleRate / 2.0f * 0.99f; 

    auto hpCoeffs = juce::IIRCoefficients::makeHighPass(sampleRate, std::clamp(hpFreq, 20.0f, maxFreq));
    auto lpCoeffs = juce::IIRCoefficients::makeLowPass(sampleRate, std::clamp(lpFreq, 20.0f, maxFreq));

    for (int i = 0; i < channelsToProcess; ++i) {
        if (i < 2) {
            highPassFilters[i].setCoefficients(hpCoeffs);
            lowPassFilters[i].setCoefficients(lpCoeffs);
        }
    }
}