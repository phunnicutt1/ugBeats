/*
 * Underground Beats
 * EffectNodeComponent.h
 * 
 * UI component for displaying and controlling an individual effect
 */

#pragma once

#include <JuceHeader.h>
#include "../effects/Effect.h"
#include "../effects/EffectsChain.h"

namespace UndergroundBeats {

/**
 * @class EffectNodeComponent
 * @brief UI component for an individual effect in the chain
 * 
 * Displays:
 * - Effect name
 * - Bypass button
 * - Parameter controls
 * - Routing handles
 */
class EffectNodeComponent : public juce::Component,
                           public juce::DragAndDropTarget {
public:
    EffectNodeComponent(EffectsChain& chain, int nodeId);
    ~EffectNodeComponent() override = default;
    
    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    // Drag and drop interface
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragMove(const SourceDetails& dragSourceDetails) override;
    void itemDragExit(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;
    
    //==============================================================================
    int getNodeId() const { return nodeId; }
    void updateParameters();
    
private:
    void createParameterControls();
    void handleParameterChange(Effect::Parameter* param, juce::Slider* slider);
    
    EffectsChain& effectsChain;
    int nodeId;
    
    juce::Label titleLabel;
    juce::ToggleButton bypassButton;
    std::vector<std::unique_ptr<juce::Slider>> parameterControls;
    std::vector<std::unique_ptr<juce::Label>> parameterLabels;
    
    bool isMouseOverDragHandle = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectNodeComponent)
};

} // namespace UndergroundBeats
