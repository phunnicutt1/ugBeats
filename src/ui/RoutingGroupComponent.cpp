/*
 * Underground Beats
 * RoutingGroupComponent.cpp
 */

#include "RoutingGroupComponent.h"

namespace UndergroundBeats {

RoutingGroupComponent::RoutingGroupComponent(EffectsChain& chain, int id)
    : effectsChain(chain)
    , nodeId(id)
    , titleLabel("title", "Group")
{
    createHeaderControls();
    
    // Set up flex box layout
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    flexBox.alignContent = juce::FlexBox::AlignContent::flexStart;
    
    setSize(300, 200);
}

void RoutingGroupComponent::paint(juce::Graphics& g) {
    // Draw background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)
              .darker(0.1f));
    
    // Draw border
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(bounds, 5.0f, 2.0f);
    
    // Draw routing type indicator
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText(isSerialGroup() ? "Serial" : "Parallel",
               bounds.removeFromTop(20.0f),
               juce::Justification::centredRight);
}

void RoutingGroupComponent::resized() {
    auto bounds = getLocalBounds().reduced(4);
    
    // Layout header controls
    auto headerArea = bounds.removeFromTop(24);
    titleLabel.setBounds(headerArea.removeFromLeft(60));
    routingTypeCombo.setBounds(headerArea.removeFromLeft(100).reduced(2));
    
    if (mixSlider && !isSerialGroup()) {
        mixSlider->setBounds(headerArea.removeFromLeft(120).reduced(2));
    }
    
    // Layout child components using FlexBox
    bounds.removeFromTop(4);
    flexBox.performLayout(bounds);
}

void RoutingGroupComponent::createHeaderControls() {
    // Title label
    addAndMakeVisible(titleLabel);
    
    // Routing type combo box
    routingTypeCombo.addItem("Serial", 1);
    routingTypeCombo.addItem("Parallel", 2);
    routingTypeCombo.setSelectedId(isSerialGroup() ? 1 : 2);
    
    routingTypeCombo.onChange = [this]() {
        // Update routing type in EffectsChain
        auto type = routingTypeCombo.getSelectedId() == 1 ? 
                   RoutingNode::Type::Serial : RoutingNode::Type::Parallel;
        
        // TODO: Implement routing type change in EffectsChain
        
        // Update mix slider visibility
        if (mixSlider) {
            mixSlider->setVisible(!isSerialGroup());
        }
        resized();
    };
    
    addAndMakeVisible(routingTypeCombo);
    
    // Mix level slider for parallel routing
    mixSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal,
                                              juce::Slider::TextBoxRight);
    mixSlider->setRange(0.0, 1.0);
    mixSlider->setValue(1.0);
    mixSlider->setTextValueSuffix(" Mix");
    
    mixSlider->onValueChange = [this]() {
        if (!isSerialGroup()) {
            setMixLevel(static_cast<float>(mixSlider->getValue()));
        }
    };
    
    addAndMakeVisible(*mixSlider);
    mixSlider->setVisible(!isSerialGroup());
}

void RoutingGroupComponent::addChildComponent(juce::Component* component) {
    if (component) {
        childComponents.push_back(component);
        addAndMakeVisible(component);
        updateLayout();
    }
}

void RoutingGroupComponent::updateLayout() {
    // Clear existing items
    flexBox.items.clear();
    
    // Add child components to flex layout
    for (auto* child : childComponents) {
        flexBox.items.add(juce::FlexItem(*child)
                         .withMinWidth(200.0f)
                         .withMinHeight(150.0f)
                         .withMargin(4.0f));
    }
    
    // Update size based on content
    auto minWidth = 300;
    auto minHeight = 200;
    
    if (!childComponents.empty()) {
        minWidth = std::max(minWidth, 
                           static_cast<int>(208.0f * std::min(childComponents.size(), size_t(3))));
        minHeight = std::max(minHeight,
                           static_cast<int>(158.0f * ((childComponents.size() + 2) / 3)));
    }
    
    setSize(minWidth, minHeight);
    resized();
}

bool RoutingGroupComponent::isSerialGroup() const {
    if (auto* node = effectsChain.getNode(nodeId)) {
        return node->getType() == RoutingNode::Type::Serial;
    }
    return true;
}

void RoutingGroupComponent::setMixLevel(float level) {
    if (!isSerialGroup()) {
        // TODO: Implement mix level setting in EffectsChain
        if (mixSlider) {
            mixSlider->setValue(level, juce::dontSendNotification);
        }
    }
}

float RoutingGroupComponent::getMixLevel() const {
    if (!isSerialGroup() && mixSlider) {
        return static_cast<float>(mixSlider->getValue());
    }
    return 1.0f;
}

} // namespace UndergroundBeats
