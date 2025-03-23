/*
 * Underground Beats
 * FilterPanel.cpp
 * 
 * Implementation of the filter control panel
 */

#include "FilterPanel.h"

namespace UndergroundBeats {

FilterPanel::FilterPanel(const juce::String& name)
    : panelName(name)
{
    // Create filter type selector
    filterTypeSelector.reset(new juce::ComboBox("Filter Type"));
    filterTypeSelector->addItem("Low Pass", static_cast<int>(FilterType::LowPass));
    filterTypeSelector->addItem("High Pass", static_cast<int>(FilterType::HighPass));
    filterTypeSelector->addItem("Band Pass", static_cast<int>(FilterType::BandPass));
    filterTypeSelector->addListener(this);
    addAndMakeVisible(filterTypeSelector.get());
    
    // Create cutoff frequency slider
    cutoffSlider.reset(new juce::Slider("Cutoff"));
    cutoffSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    cutoffSlider->setRange(20.0, 20000.0, 1.0);
    cutoffSlider->setSkewFactorFromMidPoint(1000.0);
    cutoffSlider->setValue(1000.0);
    cutoffSlider->addListener(this);
    addAndMakeVisible(cutoffSlider.get());
    
    // Create resonance slider
    resonanceSlider.reset(new juce::Slider("Resonance"));
    resonanceSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    resonanceSlider->setRange(0.1, 10.0, 0.1);
    resonanceSlider->setValue(0.707);
    resonanceSlider->addListener(this);
    addAndMakeVisible(resonanceSlider.get());
    
    // Create labels
    cutoffLabel.reset(new juce::Label("Cutoff", "Cutoff"));
    cutoffLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(cutoffLabel.get());
    
    resonanceLabel.reset(new juce::Label("Resonance", "Resonance"));
    resonanceLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(resonanceLabel.get());
}

void FilterPanel::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    // Filter type selector at the top
    filterTypeSelector->setBounds(area.removeFromTop(30));
    
    // Space between controls
    area.removeFromTop(10);
    
    // Cutoff and resonance controls side by side
    auto controlsArea = area.removeFromTop(100);
    auto cutoffArea = controlsArea.removeFromLeft(controlsArea.getWidth() / 2);
    auto resonanceArea = controlsArea;
    
    // Cutoff controls
    cutoffLabel->setBounds(cutoffArea.removeFromTop(20));
    cutoffSlider->setBounds(cutoffArea);
    
    // Resonance controls
    resonanceLabel->setBounds(resonanceArea.removeFromTop(20));
    resonanceSlider->setBounds(resonanceArea);
}

void FilterPanel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == cutoffSlider.get())
    {
        if (cutoffChangeCallback)
            cutoffChangeCallback(static_cast<float>(cutoffSlider->getValue()));
    }
    else if (slider == resonanceSlider.get())
    {
        if (resonanceChangeCallback)
            resonanceChangeCallback(static_cast<float>(resonanceSlider->getValue()));
    }
}

void FilterPanel::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == filterTypeSelector.get())
    {
        if (filterTypeChangeCallback)
        {
            auto type = static_cast<FilterType>(filterTypeSelector->getSelectedId());
            filterTypeChangeCallback(type);
        }
    }
}

} // namespace UndergroundBeats
