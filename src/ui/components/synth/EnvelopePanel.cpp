/*
 * Underground Beats
 * EnvelopePanel.cpp
 * 
 * Implementation of UI panel for advanced envelope controls
 */

#include "EnvelopePanel.h"

namespace UndergroundBeats {

EnvelopePanel::EnvelopePanel(const juce::String& name)
    : envelopeName(name)
    , currentAttackTime(10.0f)
    , currentDecayTime(100.0f)
    , currentSustainLevel(0.7f)
    , currentReleaseTime(200.0f)
    , currentAttackCurve(1.0f)
    , currentDecayCurve(1.0f)
    , currentReleaseCurve(1.0f)
{
    createControls();
}

EnvelopePanel::~EnvelopePanel()
{
}

void EnvelopePanel::setAttackTime(float timeMs, juce::NotificationType notification)
{
    currentAttackTime = timeMs;
    attackSlider.setValue(timeMs, notification);
    updateVisualizer();
}

float EnvelopePanel::getAttackTime() const
{
    return currentAttackTime;
}

void EnvelopePanel::setDecayTime(float timeMs, juce::NotificationType notification)
{
    currentDecayTime = timeMs;
    decaySlider.setValue(timeMs, notification);
    updateVisualizer();
}

float EnvelopePanel::getDecayTime() const
{
    return currentDecayTime;
}

void EnvelopePanel::setSustainLevel(float level, juce::NotificationType notification)
{
    currentSustainLevel = level;
    sustainSlider.setValue(level, notification);
    updateVisualizer();
}

float EnvelopePanel::getSustainLevel() const
{
    return currentSustainLevel;
}

void EnvelopePanel::setReleaseTime(float timeMs, juce::NotificationType notification)
{
    currentReleaseTime = timeMs;
    releaseSlider.setValue(timeMs, notification);
    updateVisualizer();
}

float EnvelopePanel::getReleaseTime() const
{
    return currentReleaseTime;
}

void EnvelopePanel::setCurves(float attackCurve, float decayCurve, float releaseCurve, juce::NotificationType notification)
{
    currentAttackCurve = attackCurve;
    currentDecayCurve = decayCurve;
    currentReleaseCurve = releaseCurve;
    
    attackCurveSlider.setValue(attackCurve, notification);
    decayCurveSlider.setValue(decayCurve, notification);
    releaseCurveSlider.setValue(releaseCurve, notification);
    
    updateVisualizer();
}

float EnvelopePanel::getAttackCurve() const
{
    return currentAttackCurve;
}

float EnvelopePanel::getDecayCurve() const
{
    return currentDecayCurve;
}

float EnvelopePanel::getReleaseCurve() const
{
    return currentReleaseCurve;
}

void EnvelopePanel::setAttackTimeChangeCallback(std::function<void(float)> callback)
{
    attackTimeChangeCallback = callback;
}

void EnvelopePanel::setDecayTimeChangeCallback(std::function<void(float)> callback)
{
    decayTimeChangeCallback = callback;
}

void EnvelopePanel::setSustainLevelChangeCallback(std::function<void(float)> callback)
{
    sustainLevelChangeCallback = callback;
}

void EnvelopePanel::setReleaseTimeChangeCallback(std::function<void(float)> callback)
{
    releaseTimeChangeCallback = callback;
}

void EnvelopePanel::setCurvesChangeCallback(std::function<void(float, float, float)> callback)
{
    curvesChangeCallback = callback;
}

void EnvelopePanel::loadPreset(const EnvelopePreset& preset)
{
    setAttackTime(preset.attackTime);
    setDecayTime(preset.decayTime);
    setSustainLevel(preset.sustainLevel);
    setReleaseTime(preset.releaseTime);
    setCurves(preset.attackCurve, preset.decayCurve, preset.releaseCurve);
}

EnvelopePreset EnvelopePanel::saveAsPreset(const juce::String& name)
{
    EnvelopePreset preset;
    preset.name = name;
    preset.attackTime = currentAttackTime;
    preset.decayTime = currentDecayTime;
    preset.sustainLevel = currentSustainLevel;
    preset.releaseTime = currentReleaseTime;
    preset.attackCurve = currentAttackCurve;
    preset.decayCurve = currentDecayCurve;
    preset.releaseCurve = currentReleaseCurve;
    return preset;
}

void EnvelopePanel::resized()
{
    // Layout the controls
    int width = getWidth();
    int height = getHeight();
    int margin = 10;
    int controlHeight = 24;
    int labelHeight = 20;
    int visualizerHeight = 80;
    
    // Visualizer at the top
    visualizer.setBounds(margin, margin, width - margin * 2, visualizerHeight);
    
    // ADSR controls below visualizer
    int controlsY = margin + visualizerHeight + margin;
    int controlsWidth = (width - margin * 2) / 2 - 5; // Split controls in two columns
    
    // Attack control (left column)
    attackLabel.setBounds(margin, controlsY, controlsWidth, labelHeight);
    attackSlider.setBounds(margin, controlsY + labelHeight, controlsWidth, controlHeight);
    
    // Decay control (left column)
    int decayY = controlsY + labelHeight + controlHeight + 5;
    decayLabel.setBounds(margin, decayY, controlsWidth, labelHeight);
    decaySlider.setBounds(margin, decayY + labelHeight, controlsWidth, controlHeight);
    
    // Sustain control (left column)
    int sustainY = decayY + labelHeight + controlHeight + 5;
    sustainLabel.setBounds(margin, sustainY, controlsWidth, labelHeight);
    sustainSlider.setBounds(margin, sustainY + labelHeight, controlsWidth, controlHeight);
    
    // Release control (left column)
    int releaseY = sustainY + labelHeight + controlHeight + 5;
    releaseLabel.setBounds(margin, releaseY, controlsWidth, labelHeight);
    releaseSlider.setBounds(margin, releaseY + labelHeight, controlsWidth, controlHeight);
    
    // Right column for curve controls
    int rightColumnX = margin + controlsWidth + 10;
    
    // Attack curve (right column)
    attackCurveLabel.setBounds(rightColumnX, controlsY, controlsWidth, labelHeight);
    attackCurveSlider.setBounds(rightColumnX, controlsY + labelHeight, controlsWidth, controlHeight);
    
    // Decay curve (right column)
    decayCurveLabel.setBounds(rightColumnX, decayY, controlsWidth, labelHeight);
    decayCurveSlider.setBounds(rightColumnX, decayY + labelHeight, controlsWidth, controlHeight);
    
    // Release curve (right column)
    releaseCurveLabel.setBounds(rightColumnX, sustainY, controlsWidth, labelHeight);
    releaseCurveSlider.setBounds(rightColumnX, sustainY + labelHeight, controlsWidth, controlHeight);
}

void EnvelopePanel::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colours::darkgrey.darker(0.2f));
    
    // Draw border
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 1);
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText(envelopeName, getLocalBounds().reduced(10, 10), juce::Justification::topLeft, true);
}

void EnvelopePanel::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &attackSlider)
    {
        currentAttackTime = static_cast<float>(attackSlider.getValue());
        
        // Call the callback if set
        if (attackTimeChangeCallback)
        {
            attackTimeChangeCallback(currentAttackTime);
        }
        
        updateVisualizer();
    }
    else if (slider == &decaySlider)
    {
        currentDecayTime = static_cast<float>(decaySlider.getValue());
        
        // Call the callback if set
        if (decayTimeChangeCallback)
        {
            decayTimeChangeCallback(currentDecayTime);
        }
        
        updateVisualizer();
    }
    else if (slider == &sustainSlider)
    {
        currentSustainLevel = static_cast<float>(sustainSlider.getValue());
        
        // Call the callback if set
        if (sustainLevelChangeCallback)
        {
            sustainLevelChangeCallback(currentSustainLevel);
        }
        
        updateVisualizer();
    }
    else if (slider == &releaseSlider)
    {
        currentReleaseTime = static_cast<float>(releaseSlider.getValue());
        
        // Call the callback if set
        if (releaseTimeChangeCallback)
        {
            releaseTimeChangeCallback(currentReleaseTime);
        }
        
        updateVisualizer();
    }
    else if (slider == &attackCurveSlider || slider == &decayCurveSlider || slider == &releaseCurveSlider)
    {
        currentAttackCurve = static_cast<float>(attackCurveSlider.getValue());
        currentDecayCurve = static_cast<float>(decayCurveSlider.getValue());
        currentReleaseCurve = static_cast<float>(releaseCurveSlider.getValue());
        
        // Call the callback if set
        if (curvesChangeCallback)
        {
            curvesChangeCallback(currentAttackCurve, currentDecayCurve, currentReleaseCurve);
        }
        
        updateVisualizer();
    }
}

void EnvelopePanel::createControls()
{
    // Add the visualizer
    addAndMakeVisible(visualizer);
    
    // Attack control
    attackLabel.setText("Attack (ms)", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(attackLabel);
    
    attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    attackSlider.setRange(1.0, 1000.0, 1.0);
    attackSlider.setSkewFactor(0.5); // Logarithmic scaling for more intuitive control
    attackSlider.setValue(currentAttackTime);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    attackSlider.addListener(this);
    addAndMakeVisible(attackSlider);
    
    // Decay control
    decayLabel.setText("Decay (ms)", juce::dontSendNotification);
    decayLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(decayLabel);
    
    decaySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    decaySlider.setRange(1.0, 2000.0, 1.0);
    decaySlider.setSkewFactor(0.5); // Logarithmic scaling
    decaySlider.setValue(currentDecayTime);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    decaySlider.addListener(this);
    addAndMakeVisible(decaySlider);
    
    // Sustain control
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(sustainLabel);
    
    sustainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(currentSustainLevel);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    sustainSlider.addListener(this);
    addAndMakeVisible(sustainSlider);
    
    // Release control
    releaseLabel.setText("Release (ms)", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(releaseLabel);
    
    releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    releaseSlider.setRange(1.0, 3000.0, 1.0);
    releaseSlider.setSkewFactor(0.5); // Logarithmic scaling
    releaseSlider.setValue(currentReleaseTime);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    releaseSlider.addListener(this);
    addAndMakeVisible(releaseSlider);
    
    // Attack curve control
    attackCurveLabel.setText("Attack Curve", juce::dontSendNotification);
    attackCurveLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(attackCurveLabel);
    
    attackCurveSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    attackCurveSlider.setRange(0.1, 5.0, 0.01);
    attackCurveSlider.setValue(currentAttackCurve);
    attackCurveSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    attackCurveSlider.addListener(this);
    addAndMakeVisible(attackCurveSlider);
    
    // Decay curve control
    decayCurveLabel.setText("Decay Curve", juce::dontSendNotification);
    decayCurveLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(decayCurveLabel);
    
    decayCurveSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    decayCurveSlider.setRange(0.1, 5.0, 0.01);
    decayCurveSlider.setValue(currentDecayCurve);
    decayCurveSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    decayCurveSlider.addListener(this);
    addAndMakeVisible(decayCurveSlider);
    
    // Release curve control
    releaseCurveLabel.setText("Release Curve", juce::dontSendNotification);
    releaseCurveLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(releaseCurveLabel);
    
    releaseCurveSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    releaseCurveSlider.setRange(0.1, 5.0, 0.01);
    releaseCurveSlider.setValue(currentReleaseCurve);
    releaseCurveSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    releaseCurveSlider.addListener(this);
    addAndMakeVisible(releaseCurveSlider);
    
    // Initialize the visualizer
    updateVisualizer();
}

void EnvelopePanel::updateVisualizer()
{
    visualizer.setParameters(currentAttackTime, currentDecayTime, currentSustainLevel, currentReleaseTime);
    visualizer.setCurves(currentAttackCurve, currentDecayCurve, currentReleaseCurve);
}

} // namespace UndergroundBeats
