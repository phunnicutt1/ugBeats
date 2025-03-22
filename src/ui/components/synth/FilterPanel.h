#pragma once

#include <JuceHeader.h>
#include "../../../synthesis/Filter.h"

namespace UndergroundBeats {

/**
 * @class FilterPanel
 * @brief UI panel for filter controls
 */
class FilterPanel : public juce::Component
{
public:
    FilterPanel(const juce::String& name = "Filter")
        : panelName(name)
    {
        // Stub implementation
    }
    
    ~FilterPanel() override = default;
    
    void setFilterTypeChangeCallback(std::function<void(FilterType)> callback) {
        filterTypeChangeCallback = callback;
    }
    
    void setCutoffChangeCallback(std::function<void(float)> callback) {
        cutoffChangeCallback = callback;
    }
    
    void setResonanceChangeCallback(std::function<void(float)> callback) {
        resonanceChangeCallback = callback;
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawText(panelName, getLocalBounds(), juce::Justification::centred, true);
    }
    
private:
    juce::String panelName;
    
    std::function<void(FilterType)> filterTypeChangeCallback;
    std::function<void(float)> cutoffChangeCallback;
    std::function<void(float)> resonanceChangeCallback;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterPanel)
};

} // namespace UndergroundBeats