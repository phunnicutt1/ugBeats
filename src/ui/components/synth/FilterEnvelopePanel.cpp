#include "FilterEnvelopePanel.h"

namespace UndergroundBeats {

FilterEnvelopePanel::FilterEnvelopePanel(const juce::String& name)
    : panelName(name),
      currentCutoffMod(0.0f),
      currentResonanceMod(0.0f)
{
    addAndMakeVisible(filterPanelComponent);
    addAndMakeVisible(envelopePanelComponent);
    
    createControls();
}

FilterEnvelopePanel::~FilterEnvelopePanel()
{
    cutoffModSlider.removeListener(this);
    resonanceModSlider.removeListener(this);
}

void FilterEnvelopePanel::createControls()
{
    // Set up cutoff modulation slider
    cutoffModSlider.setRange(-1.0, 1.0, 0.01);
    cutoffModSlider.setValue(0.0, juce::dontSendNotification);
    cutoffModSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    cutoffModSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffModSlider.addListener(this);
    
    cutoffModLabel.setText("Cutoff Mod", juce::dontSendNotification);
    cutoffModLabel.attachToComponent(&cutoffModSlider, true);
    
    // Set up resonance modulation slider
    resonanceModSlider.setRange(-1.0, 1.0, 0.01);
    resonanceModSlider.setValue(0.0, juce::dontSendNotification);
    resonanceModSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    resonanceModSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    resonanceModSlider.addListener(this);
    
    resonanceModLabel.setText("Res Mod", juce::dontSendNotification);
    resonanceModLabel.attachToComponent(&resonanceModSlider, true);
    
    addAndMakeVisible(cutoffModSlider);
    addAndMakeVisible(cutoffModLabel);
    addAndMakeVisible(resonanceModSlider);
    addAndMakeVisible(resonanceModLabel);
}

void FilterEnvelopePanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(juce::FontHeavy, 18.0f));
    g.drawText(panelName, getLocalBounds().removeFromTop(25), juce::Justification::centred, true);
}

void FilterEnvelopePanel::resized()
{
    auto area = getLocalBounds().reduced(5);
    
    // Title area
    area.removeFromTop(25);
    
    // Reserve space for modulation sliders
    auto sliderArea = area.removeFromBottom(80);
    
    // Split the main area between filter and envelope panels
    auto filterArea = area.removeFromLeft(area.getWidth() / 2);
    auto envelopeArea = area;
    
    // Position panels
    filterPanelComponent.setBounds(filterArea.reduced(5));
    envelopePanelComponent.setBounds(envelopeArea.reduced(5));
    
    // Position modulation sliders
    sliderArea.removeFromTop(10); // Spacing
    
    int labelWidth = 80;
    int sliderHeight = 24;
    
    auto cutoffModArea = sliderArea.removeFromTop(sliderHeight);
    cutoffModSlider.setBounds(cutoffModArea.withTrimmedLeft(labelWidth));
    
    sliderArea.removeFromTop(10); // Spacing
    
    auto resonanceModArea = sliderArea.removeFromTop(sliderHeight);
    resonanceModSlider.setBounds(resonanceModArea.withTrimmedLeft(labelWidth));
}

void FilterEnvelopePanel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &cutoffModSlider)
    {
        currentCutoffMod = static_cast<float>(slider->getValue());
        
        if (cutoffModulationChangeCallback)
        {
            cutoffModulationChangeCallback(currentCutoffMod);
        }
    }
    else if (slider == &resonanceModSlider)
    {
        currentResonanceMod = static_cast<float>(slider->getValue());
        
        if (resonanceModulationChangeCallback)
        {
            resonanceModulationChangeCallback(currentResonanceMod);
        }
    }
}

void FilterEnvelopePanel::setCutoffEnvelopeAmount(float amount, juce::NotificationType notification)
{
    currentCutoffMod = amount;
    cutoffModSlider.setValue(amount, notification);
}

float FilterEnvelopePanel::getCutoffEnvelopeAmount() const
{
    return currentCutoffMod;
}

void FilterEnvelopePanel::setResonanceEnvelopeAmount(float amount, juce::NotificationType notification)
{
    currentResonanceMod = amount;
    resonanceModSlider.setValue(amount, notification);
}

float FilterEnvelopePanel::getResonanceEnvelopeAmount() const
{
    return currentResonanceMod;
}

void FilterEnvelopePanel::setCutoffModulationChangeCallback(std::function<void(float)> callback)
{
    cutoffModulationChangeCallback = callback;
}

void FilterEnvelopePanel::setResonanceModulationChangeCallback(std::function<void(float)> callback)
{
    resonanceModulationChangeCallback = callback;
}

// Stub implementations for now
FilterPanel& FilterEnvelopePanel::getFilterPanel()
{
    // This is a temporary stub implementation
    // The real implementation will be added when FilterPanel is fully defined
    static juce::ScopedJuceInitialiser_GUI init;
    static std::runtime_error exception("FilterPanel not fully implemented yet");
    throw exception;
}

EnvelopePanel& FilterEnvelopePanel::getEnvelopePanel()
{
    // This is a temporary stub implementation
    // The real implementation will be added when EnvelopePanel is fully defined
    static juce::ScopedJuceInitialiser_GUI init;
    static std::runtime_error exception("EnvelopePanel not fully implemented yet");
    throw exception;
}

} // namespace UndergroundBeats