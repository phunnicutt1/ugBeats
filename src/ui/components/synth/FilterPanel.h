#pragma once

#include <JuceHeader.h>
#include "../../../synthesis/Filter.h"

namespace UndergroundBeats {

/**
 * @class FilterPanel
 * @brief UI panel for filter controls
 */
class FilterPanel : public juce::Component,
                   public juce::Slider::Listener,
                   public juce::ComboBox::Listener
{
public:
    FilterPanel(const juce::String& name = "Filter");
    
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
    
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    
    juce::String panelName;
    
    std::unique_ptr<juce::ComboBox> filterTypeSelector;
    std::unique_ptr<juce::Slider> cutoffSlider;
    std::unique_ptr<juce::Slider> resonanceSlider;
    
    std::unique_ptr<juce::Label> cutoffLabel;
    std::unique_ptr<juce::Label> resonanceLabel;
    
    std::function<void(FilterType)> filterTypeChangeCallback;
    std::function<void(float)> cutoffChangeCallback;
    std::function<void(float)> resonanceChangeCallback;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterPanel)
};

} // namespace UndergroundBeats