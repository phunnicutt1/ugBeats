#pragma once

#include <JuceHeader.h>

namespace UndergroundBeats {

// Forward declare these classes
class FilterPanel;
class EnvelopePanel;

class FilterEnvelopePanel : public juce::Component,
                          public juce::Slider::Listener
{
public:
    FilterEnvelopePanel(const juce::String& name = "Filter Envelope");
    ~FilterEnvelopePanel() override;
    
    void setCutoffEnvelopeAmount(float amount, juce::NotificationType notification = juce::sendNotificationAsync);
    float getCutoffEnvelopeAmount() const;
    
    void setResonanceEnvelopeAmount(float amount, juce::NotificationType notification = juce::sendNotificationAsync);
    float getResonanceEnvelopeAmount() const;
    
    void setCutoffModulationChangeCallback(std::function<void(float)> callback);
    void setResonanceModulationChangeCallback(std::function<void(float)> callback);
    
    FilterPanel& getFilterPanel();
    EnvelopePanel& getEnvelopePanel();
    
    void resized() override;
    void paint(juce::Graphics& g) override;
    void sliderValueChanged(juce::Slider* slider) override;
    
private:
    juce::String panelName;
    
    // For now, use temporary placeholder components
    juce::Component filterPanelComponent;
    juce::Component envelopePanelComponent;
    
    juce::Slider cutoffModSlider;
    juce::Label cutoffModLabel;
    juce::Slider resonanceModSlider;
    juce::Label resonanceModLabel;
    
    float currentCutoffMod = 0.0f;
    float currentResonanceMod = 0.0f;
    
    std::function<void(float)> cutoffModulationChangeCallback;
    std::function<void(float)> resonanceModulationChangeCallback;
    
    void createControls();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterEnvelopePanel)
};

} // namespace UndergroundBeats