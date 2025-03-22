#pragma once

#include <JuceHeader.h>

namespace UndergroundBeats {

/**
 * @class EnvelopePanel
 * @brief UI panel for envelope controls
 */
class EnvelopePanel : public juce::Component
{
public:
    EnvelopePanel(const juce::String& name = "Envelope")
        : panelName(name)
    {
        // Stub implementation
    }
    
    ~EnvelopePanel() override = default;
    
    void setAttackTimeChangeCallback(std::function<void(float)> callback) {
        attackTimeChangeCallback = callback;
    }
    
    void setDecayTimeChangeCallback(std::function<void(float)> callback) {
        decayTimeChangeCallback = callback;
    }
    
    void setSustainLevelChangeCallback(std::function<void(float)> callback) {
        sustainLevelChangeCallback = callback;
    }
    
    void setReleaseTimeChangeCallback(std::function<void(float)> callback) {
        releaseTimeChangeCallback = callback;
    }
    
    void setCurvesChangeCallback(std::function<void(float, float, float)> callback) {
        curvesChangeCallback = callback;
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawText(panelName, getLocalBounds(), juce::Justification::centred, true);
    }
    
private:
    juce::String panelName;
    
    std::function<void(float)> attackTimeChangeCallback;
    std::function<void(float)> decayTimeChangeCallback;
    std::function<void(float)> sustainLevelChangeCallback;
    std::function<void(float)> releaseTimeChangeCallback;
    std::function<void(float, float, float)> curvesChangeCallback;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopePanel)
};

} // namespace UndergroundBeats