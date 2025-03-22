/*
 * Underground Beats
 * RoutingGroupComponent.h
 * 
 * UI component for managing groups of effects in serial or parallel routing
 */

#pragma once

#include <JuceHeader.h>
#include "../effects/EffectsChain.h"
#include "../effects/RoutingNode.h"

namespace UndergroundBeats {

/**
 * @class RoutingGroupComponent
 * @brief UI component for managing effect routing groups
 * 
 * Handles:
 * - Serial/parallel routing configuration
 * - Mix level control for parallel groups
 * - Adding/removing effects in the group
 * - Visual organization of child effects
 */
class RoutingGroupComponent : public juce::Component,
                             public juce::DragAndDropContainer {
public:
    RoutingGroupComponent(EffectsChain& chain, int nodeId);
    ~RoutingGroupComponent() override = default;
    
    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    int getNodeId() const { return nodeId; }
    void updateLayout();
    void addChildComponent(juce::Component* component);
    
    //==============================================================================
    bool isSerialGroup() const;
    void setMixLevel(float level);
    float getMixLevel() const;
    
private:
    void createHeaderControls();
    void layoutChildComponents();
    
    EffectsChain& effectsChain;
    int nodeId;
    
    juce::Label titleLabel;
    juce::ComboBox routingTypeCombo;
    std::unique_ptr<juce::Slider> mixSlider;
    
    juce::FlexBox flexBox;
    std::vector<juce::Component*> childComponents;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoutingGroupComponent)
};

} // namespace UndergroundBeats
