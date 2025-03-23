/*
 * Underground Beats
 * EffectNodeComponent.cpp
 */

#include "EffectNodeComponent.h"

namespace UndergroundBeats {

EffectNodeComponent::EffectNodeComponent(EffectsChain& chain, int id)
    : effectsChain(chain)
    , nodeId(id)
    , titleLabel("title", "Effect")
    , bypassButton("Bypass")
{
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(bypassButton);
    
    if (auto* effect = effectsChain.getEffect(nodeId)) {
        titleLabel.setText(effect->getName(), juce::dontSendNotification);
        createParameterControls();
    }
    
    bypassButton.onClick = [this]() {
        if (auto* effect = effectsChain.getEffect(nodeId)) {
            effect->setEnabled(!bypassButton.getToggleState());
        }
    };
    
    setSize(200, 150);
}

void EffectNodeComponent::paint(juce::Graphics& g) {
    // Draw background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    // Draw border
    auto bounds = getLocalBounds().toFloat();
    g.setColour(isMouseOverDragHandle ? juce::Colours::orange : juce::Colours::grey);
    g.drawRoundedRectangle(bounds, 5.0f, 1.0f);
    
    // Draw drag handles
    float handleSize = 10.0f;
    g.setColour(juce::Colours::grey);
    g.fillRect(0.0f, bounds.getCentreY() - handleSize/2, handleSize, handleSize); // Input handle
    g.fillRect(bounds.getRight() - handleSize, bounds.getCentreY() - handleSize/2, handleSize, handleSize); // Output handle
}

void EffectNodeComponent::resized() {
    auto bounds = getLocalBounds().reduced(4);
    
    // Layout title and bypass button
    auto topArea = bounds.removeFromTop(24);
    titleLabel.setBounds(topArea.removeFromLeft(topArea.getWidth() - 60));
    bypassButton.setBounds(topArea.reduced(2));
    
    // Layout parameter controls
    bounds.removeFromTop(4);
    for (size_t i = 0; i < parameterControls.size(); ++i) {
        auto rowBounds = bounds.removeFromTop(24);
        if (parameterLabels[i]) {
            parameterLabels[i]->setBounds(rowBounds.removeFromLeft(60));
        }
        parameterControls[i]->setBounds(rowBounds);
        bounds.removeFromTop(4);
    }
}

void EffectNodeComponent::createParameterControls() {
    if (auto* effect = effectsChain.getEffect(nodeId)) {
        for (const auto& [name, param] : effect->getParameters()) {
            // Create label
            auto label = std::make_unique<juce::Label>();
            label->setText(name, juce::dontSendNotification);
            label->setJustificationType(juce::Justification::right);
            addAndMakeVisible(*label);
            parameterLabels.push_back(std::move(label));
            
            // Create slider
            auto slider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal,
                                                       juce::Slider::TextBoxRight);
            slider->setRange(param->getMinValue(), param->getMaxValue());
            slider->setValue(param->getValue(), juce::dontSendNotification);
            
            slider->onValueChange = [this, param, slider = slider.get()]() {
                handleParameterChange(param, slider);
            };
            
            addAndMakeVisible(*slider);
            parameterControls.push_back(std::move(slider));
        }
    }
}

void EffectNodeComponent::handleParameterChange(Effect::Parameter* param, juce::Slider* slider) {
    if (param && slider) {
        param->setValue(static_cast<float>(slider->getValue()));
    }
}

void EffectNodeComponent::updateParameters() {
    if (auto* effect = effectsChain.getEffect(nodeId)) {
        size_t index = 0;
        for (const auto& [name, param] : effect->getParameters()) {
            if (index < parameterControls.size()) {
                parameterControls[index]->setValue(param->getValue(), juce::dontSendNotification);
            }
            ++index;
        }
    }
}

bool EffectNodeComponent::isInterestedInDragSource(const SourceDetails& dragSourceDetails) {
    // Accept drops from other effect nodes
    return dragSourceDetails.description.toString().startsWith("EffectNode:");
}

void EffectNodeComponent::itemDragEnter(const SourceDetails&) {
    isMouseOverDragHandle = true;
    repaint();
}

void EffectNodeComponent::itemDragMove(const SourceDetails&) {
    // Handle drag movement if needed
}

void EffectNodeComponent::itemDragExit(const SourceDetails&) {
    isMouseOverDragHandle = false;
    repaint();
}

void EffectNodeComponent::itemDropped(const SourceDetails& dragSourceDetails) {
    isMouseOverDragHandle = false;
    
    // Extract source node ID from description
    juce::String desc = dragSourceDetails.description.toString();
    if (desc.startsWith("EffectNode:")) {
        int sourceId = desc.fromFirstOccurrenceOf(":", false, false).getIntValue();
        
        // Handle effect reordering through EffectsChain
        // TODO: Implement effect reordering based on drop position
    }
    
    repaint();
}

} // namespace UndergroundBeats
