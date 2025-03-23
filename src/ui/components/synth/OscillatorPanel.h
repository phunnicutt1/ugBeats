#pragma once

#include <JuceHeader.h>
#include "../../../synthesis/Oscillator.h"

namespace UndergroundBeats {

/**
 * @class OscillatorPanel
 * @brief UI panel for oscillator controls
 */
class OscillatorPanel : public juce::Component
{
public:
    OscillatorPanel(const juce::String& name = "Oscillator")
        : panelName(name)
    {
        // Stub implementation
    }
    
    ~OscillatorPanel() override = default;
    
    void setFrequencyChangeCallback(std::function<void(float)> callback) {
        frequencyChangeCallback = callback;
    }
    
    void setWaveformChangeCallback(std::function<void(int, WaveformType)> callback) {
        waveformChangeCallback = callback;
    }
    
    void setMixLevelChangeCallback(std::function<void(int, float)> callback) {
        mixLevelChangeCallback = callback;
    }
    
    void setFineTuningChangeCallback(std::function<void(int, float)> callback) {
        fineTuningChangeCallback = callback;
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawText(panelName, getLocalBounds(), juce::Justification::centred, true);
    }
    
private:
    juce::String panelName;
    
    std::function<void(float)> frequencyChangeCallback;
    std::function<void(int, WaveformType)> waveformChangeCallback;
    std::function<void(int, float)> mixLevelChangeCallback;
    std::function<void(int, float)> fineTuningChangeCallback;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorPanel)
};

} // namespace UndergroundBeats