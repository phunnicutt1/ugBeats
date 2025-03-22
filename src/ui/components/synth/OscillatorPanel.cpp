/*
 * Underground Beats
 * OscillatorPanel.cpp
 * 
 * Implementation of UI panel for oscillator controls
 */

#include "OscillatorPanel.h"

namespace UndergroundBeats {

OscillatorPanel::OscillatorPanel(const juce::String& name)
    : oscBankName(name)
{
    createControls();
}

OscillatorPanel::~OscillatorPanel()
{
}

void OscillatorPanel::setFrequency(float frequencyHz, juce::NotificationType notification)
{
    currentFrequency = frequencyHz;
    frequencySlider.setValue(frequencyHz, notification);
}

float OscillatorPanel::getFrequency() const
{
    return currentFrequency;
}

void OscillatorPanel::setWaveform(int oscillatorIndex, WaveformType type, juce::NotificationType notification)
{
    if (oscillatorIndex == 0)
    {
        currentOsc1Waveform = type;
        osc1WaveformCombo.setSelectedItemIndex(waveformTypeToIndex(type), notification);
    }
    else if (oscillatorIndex == 1)
    {
        currentOsc2Waveform = type;
        osc2WaveformCombo.setSelectedItemIndex(waveformTypeToIndex(type), notification);
    }
}

WaveformType OscillatorPanel::getWaveform(int oscillatorIndex) const
{
    return (oscillatorIndex == 0) ? currentOsc1Waveform : currentOsc2Waveform;
}

void OscillatorPanel::setMixLevel(int oscillatorIndex, float level, juce::NotificationType notification)
{
    if (oscillatorIndex == 0)
    {
        currentOsc1MixLevel = level;
        osc1MixLevelSlider.setValue(level, notification);
    }
    else if (oscillatorIndex == 1)
    {
        currentOsc2MixLevel = level;
        osc2MixLevelSlider.setValue(level, notification);
    }
}

float OscillatorPanel::getMixLevel(int oscillatorIndex) const
{
    return (oscillatorIndex == 0) ? currentOsc1MixLevel : currentOsc2MixLevel;
}

void OscillatorPanel::setFineTuning(int oscillatorIndex, float cents, juce::NotificationType notification)
{
    if (oscillatorIndex == 0)
    {
        currentOsc1FineTuning = cents;
        osc1FineTuningSlider.setValue(cents, notification);
    }
    else if (oscillatorIndex == 1)
    {
        currentOsc2FineTuning = cents;
        osc2FineTuningSlider.setValue(cents, notification);
    }
}

float OscillatorPanel::getFineTuning(int oscillatorIndex) const
{
    return (oscillatorIndex == 0) ? currentOsc1FineTuning : currentOsc2FineTuning;
}

void OscillatorPanel::setSyncEnabled(bool enabled, juce::NotificationType notification)
{
    currentSyncEnabled = enabled;
    syncButton.setToggleState(enabled, notification);
}

bool OscillatorPanel::getSyncEnabled() const
{
    return currentSyncEnabled;
}

void OscillatorPanel::setFMEnabled(bool enabled, juce::NotificationType notification)
{
    currentFMEnabled = enabled;
    fmButton.setToggleState(enabled, notification);
    fmAmountSlider.setEnabled(enabled);
    fmAmountLabel.setEnabled(enabled);
}

bool OscillatorPanel::getFMEnabled() const
{
    return currentFMEnabled;
}

void OscillatorPanel::setFMAmount(float amount, juce::NotificationType notification)
{
    currentFMAmount = amount;
    fmAmountSlider.setValue(amount, notification);
}

float OscillatorPanel::getFMAmount() const
{
    return currentFMAmount;
}

void OscillatorPanel::setFrequencyChangeCallback(std::function<void(float)> callback)
{
    frequencyChangeCallback = callback;
}

void OscillatorPanel::setWaveformChangeCallback(std::function<void(int, WaveformType)> callback)
{
    waveformChangeCallback = callback;
}

void OscillatorPanel::setMixLevelChangeCallback(std::function<void(int, float)> callback)
{
    mixLevelChangeCallback = callback;
}

void OscillatorPanel::setFineTuningChangeCallback(std::function<void(int, float)> callback)
{
    fineTuningChangeCallback = callback;
}

void OscillatorPanel::setSyncChangeCallback(std::function<void(bool)> callback)
{
    syncChangeCallback = callback;
}

void OscillatorPanel::setFMChangeCallback(std::function<void(bool)> callback)
{
    fmChangeCallback = callback;
}

void OscillatorPanel::setFMAmountChangeCallback(std::function<void(float)> callback)
{
    fmAmountChangeCallback = callback;
}

void OscillatorPanel::resized()
{
    // Layout the components
    int width = getWidth();
    int height = getHeight();
    int margin = 10;
    int controlHeight = 24;
    int labelHeight = 20;
    int comboBoxWidth = 120;
    
    // Panel title space
    juce::Rectangle<int> titleArea = getLocalBounds().removeFromTop(40);
    
    // Master controls at the top
    juce::Rectangle<int> masterArea = getLocalBounds().removeFromTop(120).reduced(margin);
    
    // Frequency controls
    juce::Rectangle<int> freqArea = masterArea.removeFromTop(controlHeight + labelHeight);
    frequencyLabel.setBounds(freqArea.removeFromLeft(100));
    frequencySlider.setBounds(freqArea);
    
    // FM/Sync controls
    juce::Rectangle<int> modArea = masterArea.removeFromTop(controlHeight + margin);
    syncButton.setBounds(modArea.removeFromLeft(100));
    fmButton.setBounds(modArea.removeFromLeft(100));
    
    juce::Rectangle<int> fmAmountArea = masterArea.removeFromTop(controlHeight + labelHeight);
    fmAmountLabel.setBounds(fmAmountArea.removeFromLeft(100));
    fmAmountSlider.setBounds(fmAmountArea);
    
    // Divide the remaining area into two columns for the oscillators
    juce::Rectangle<int> oscArea = getLocalBounds().withTrimmedTop(160).reduced(margin);
    int halfWidth = oscArea.getWidth() / 2;
    
    // Oscillator 1 controls
    juce::Rectangle<int> osc1Area = oscArea.removeFromLeft(halfWidth).reduced(margin, 0);
    
    // Oscillator 1 waveform
    juce::Rectangle<int> osc1WaveArea = osc1Area.removeFromTop(controlHeight + labelHeight);
    osc1WaveformLabel.setBounds(osc1WaveArea.removeFromTop(labelHeight));
    osc1WaveformCombo.setBounds(osc1WaveArea);
    
    // Oscillator 1 mix level
    juce::Rectangle<int> osc1MixArea = osc1Area.removeFromTop(controlHeight + labelHeight);
    osc1MixLevelLabel.setBounds(osc1MixArea.removeFromTop(labelHeight));
    osc1MixLevelSlider.setBounds(osc1MixArea);
    
    // Oscillator 1 fine tuning
    juce::Rectangle<int> osc1TuneArea = osc1Area.removeFromTop(controlHeight + labelHeight);
    osc1FineTuningLabel.setBounds(osc1TuneArea.removeFromTop(labelHeight));
    osc1FineTuningSlider.setBounds(osc1TuneArea);
    
    // Oscillator 2 controls
    juce::Rectangle<int> osc2Area = oscArea.reduced(margin, 0);
    
    // Oscillator 2 waveform
    juce::Rectangle<int> osc2WaveArea = osc2Area.removeFromTop(controlHeight + labelHeight);
    osc2WaveformLabel.setBounds(osc2WaveArea.removeFromTop(labelHeight));
    osc2WaveformCombo.setBounds(osc2WaveArea);
    
    // Oscillator 2 mix level
    juce::Rectangle<int> osc2MixArea = osc2Area.removeFromTop(controlHeight + labelHeight);
    osc2MixLevelLabel.setBounds(osc2MixArea.removeFromTop(labelHeight));
    osc2MixLevelSlider.setBounds(osc2MixArea);
    
    // Oscillator 2 fine tuning
    juce::Rectangle<int> osc2TuneArea = osc2Area.removeFromTop(controlHeight + labelHeight);
    osc2FineTuningLabel.setBounds(osc2TuneArea.removeFromTop(labelHeight));
    osc2FineTuningSlider.setBounds(osc2TuneArea);
}

void OscillatorPanel::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colours::darkgrey.darker(0.2f));
    
    // Draw border
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 1);
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText(oscBankName, getLocalBounds().reduced(10, 10), juce::Justification::topLeft, true);
    
    // Draw oscillator labels
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    juce::Rectangle<int> oscArea = getLocalBounds().withTrimmedTop(160).reduced(10);
    int halfWidth = oscArea.getWidth() / 2;
    
    g.drawText("Oscillator 1", oscArea.removeFromLeft(halfWidth).reduced(10, 0).withTrimmedBottom(oscArea.getHeight() - 30), juce::Justification::centredTop, true);
    g.drawText("Oscillator 2", oscArea.reduced(10, 0).withTrimmedBottom(oscArea.getHeight() - 30), juce::Justification::centredTop, true);
}

void OscillatorPanel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &osc1WaveformCombo)
    {
        int index = osc1WaveformCombo.getSelectedItemIndex();
        currentOsc1Waveform = indexToWaveformType(index);
        
        // Call the callback if set
        if (waveformChangeCallback)
        {
            waveformChangeCallback(0, currentOsc1Waveform);
        }
    }
    else if (comboBoxThatHasChanged == &osc2WaveformCombo)
    {
        int index = osc2WaveformCombo.getSelectedItemIndex();
        currentOsc2Waveform = indexToWaveformType(index);
        
        // Call the callback if set
        if (waveformChangeCallback)
        {
            waveformChangeCallback(1, currentOsc2Waveform);
        }
    }
}

void OscillatorPanel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &frequencySlider)
    {
        currentFrequency = static_cast<float>(frequencySlider.getValue());
        
        // Call the callback if set
        if (frequencyChangeCallback)
        {
            frequencyChangeCallback(currentFrequency);
        }
    }
    else if (slider == &osc1MixLevelSlider)
    {
        currentOsc1MixLevel = static_cast<float>(osc1MixLevelSlider.getValue());
        
        // Call the callback if set
        if (mixLevelChangeCallback)
        {
            mixLevelChangeCallback(0, currentOsc1MixLevel);
        }
    }
    else if (slider == &osc2MixLevelSlider)
    {
        currentOsc2MixLevel = static_cast<float>(osc2MixLevelSlider.getValue());
        
        // Call the callback if set
        if (mixLevelChangeCallback)
        {
            mixLevelChangeCallback(1, currentOsc2MixLevel);
        }
    }
    else if (slider == &osc1FineTuningSlider)
    {
        currentOsc1FineTuning = static_cast<float>(osc1FineTuningSlider.getValue());
        
        // Call the callback if set
        if (fineTuningChangeCallback)
        {
            fineTuningChangeCallback(0, currentOsc1FineTuning);
        }
    }
    else if (slider == &osc2FineTuningSlider)
    {
        currentOsc2FineTuning = static_cast<float>(osc2FineTuningSlider.getValue());
        
        // Call the callback if set
        if (fineTuningChangeCallback)
        {
            fineTuningChangeCallback(1, currentOsc2FineTuning);
        }
    }
    else if (slider == &fmAmountSlider)
    {
        currentFMAmount = static_cast<float>(fmAmountSlider.getValue());
        
        // Call the callback if set
        if (fmAmountChangeCallback)
        {
            fmAmountChangeCallback(currentFMAmount);
        }
    }
}

void OscillatorPanel::buttonClicked(juce::Button* button)
{
    if (button == &syncButton)
    {
        currentSyncEnabled = syncButton.getToggleState();
        
        // Call the callback if set
        if (syncChangeCallback)
        {
            syncChangeCallback(currentSyncEnabled);
        }
    }
    else if (button == &fmButton)
    {
        currentFMEnabled = fmButton.getToggleState();
        fmAmountSlider.setEnabled(currentFMEnabled);
        fmAmountLabel.setEnabled(currentFMEnabled);
        
        // Call the callback if set
        if (fmChangeCallback)
        {
            fmChangeCallback(currentFMEnabled);
        }
    }
}

void OscillatorPanel::createControls()
{
    // Master frequency control
    frequencyLabel.setText("Frequency (Hz)", juce::dontSendNotification);
    frequencyLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(frequencyLabel);
    
    frequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    frequencySlider.setRange(20.0, 10000.0, 0.1);
    frequencySlider.setSkewFactor(0.3f); // Logarithmic scaling for more intuitive frequency control
    frequencySlider.setValue(currentFrequency);
    frequencySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    frequencySlider.addListener(this);
    addAndMakeVisible(frequencySlider);
    
    // FM/Sync controls
    syncButton.setToggleState(currentSyncEnabled, juce::dontSendNotification);
    syncButton.addListener(this);
    addAndMakeVisible(syncButton);
    
    fmButton.setToggleState(currentFMEnabled, juce::dontSendNotification);
    fmButton.addListener(this);
    addAndMakeVisible(fmButton);
    
    fmAmountLabel.setText("FM Amount", juce::dontSendNotification);
    fmAmountLabel.setJustificationType(juce::Justification::centredLeft);
    fmAmountLabel.setEnabled(currentFMEnabled);
    addAndMakeVisible(fmAmountLabel);
    
    fmAmountSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    fmAmountSlider.setRange(0.0, 10.0, 0.01);
    fmAmountSlider.setValue(currentFMAmount);
    fmAmountSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    fmAmountSlider.addListener(this);
    fmAmountSlider.setEnabled(currentFMEnabled);
    addAndMakeVisible(fmAmountSlider);
    
    // Oscillator 1 controls
    osc1WaveformLabel.setText("Waveform", juce::dontSendNotification);
    osc1WaveformLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(osc1WaveformLabel);
    
    osc1WaveformCombo.addItem("Sine", 1);
    osc1WaveformCombo.addItem("Triangle", 2);
    osc1WaveformCombo.addItem("Sawtooth", 3);
    osc1WaveformCombo.addItem("Square", 4);
    osc1WaveformCombo.addItem("Noise", 5);
    osc1WaveformCombo.setSelectedItemIndex(waveformTypeToIndex(currentOsc1Waveform));
    osc1WaveformCombo.addListener(this);
    addAndMakeVisible(osc1WaveformCombo);
    
    osc1MixLevelLabel.setText("Mix Level", juce::dontSendNotification);
    osc1MixLevelLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(osc1MixLevelLabel);
    
    osc1MixLevelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    osc1MixLevelSlider.setRange(0.0, 1.0, 0.01);
    osc1MixLevelSlider.setValue(currentOsc1MixLevel);
    osc1MixLevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    osc1MixLevelSlider.addListener(this);
    addAndMakeVisible(osc1MixLevelSlider);
    
    osc1FineTuningLabel.setText("Fine Tuning (cents)", juce::dontSendNotification);
    osc1FineTuningLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(osc1FineTuningLabel);
    
    osc1FineTuningSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    osc1FineTuningSlider.setRange(-100.0, 100.0, 0.1);
    osc1FineTuningSlider.setValue(currentOsc1FineTuning);
    osc1FineTuningSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    osc1FineTuningSlider.addListener(this);
    addAndMakeVisible(osc1FineTuningSlider);
    
    // Oscillator 2 controls
    osc2WaveformLabel.setText("Waveform", juce::dontSendNotification);
    osc2WaveformLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(osc2WaveformLabel);
    
    osc2WaveformCombo.addItem("Sine", 1);
    osc2WaveformCombo.addItem("Triangle", 2);
    osc2WaveformCombo.addItem("Sawtooth", 3);
    osc2WaveformCombo.addItem("Square", 4);
    osc2WaveformCombo.addItem("Noise", 5);
    osc2WaveformCombo.setSelectedItemIndex(waveformTypeToIndex(currentOsc2Waveform));
    osc2WaveformCombo.addListener(this);
    addAndMakeVisible(osc2WaveformCombo);
    
    osc2MixLevelLabel.setText("Mix Level", juce::dontSendNotification);
    osc2MixLevelLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(osc2MixLevelLabel);
    
    osc2MixLevelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    osc2MixLevelSlider.setRange(0.0, 1.0, 0.01);
    osc2MixLevelSlider.setValue(currentOsc2MixLevel);
    osc2MixLevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    osc2MixLevelSlider.addListener(this);
    addAndMakeVisible(osc2MixLevelSlider);
    
    osc2FineTuningLabel.setText("Fine Tuning (cents)", juce::dontSendNotification);
    osc2FineTuningLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(osc2FineTuningLabel);
    
    osc2FineTuningSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    osc2FineTuningSlider.setRange(-100.0, 100.0, 0.1);
    osc2FineTuningSlider.setValue(currentOsc2FineTuning);
    osc2FineTuningSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
    osc2FineTuningSlider.addListener(this);
    addAndMakeVisible(osc2FineTuningSlider);
}

WaveformType OscillatorPanel::indexToWaveformType(int index)
{
    switch (index)
    {
        case 0: return WaveformType::Sine;
        case 1: return WaveformType::Triangle;
        case 2: return WaveformType::Sawtooth;
        case 3: return WaveformType::Square;
        case 4: return WaveformType::Noise;
        default: return WaveformType::Sine;
    }
}

int OscillatorPanel::waveformTypeToIndex(WaveformType type)
{
    switch (type)
    {
        case WaveformType::Sine: return 0;
        case WaveformType::Triangle: return 1;
        case WaveformType::Sawtooth: return 2;
        case WaveformType::Square: return 3;
        case WaveformType::Noise: return 4;
        default: return 0;
    }
}

} // namespace UndergroundBeats
