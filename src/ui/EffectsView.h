/*
 * Underground Beats
 * EffectsView.h
 * 
 * UI component for managing and controlling audio effects
 */

#pragma once

#include <JuceHeader.h>
#include "../effects/EffectsChain.h"
#include "../effects/PresetManager.h"

namespace UndergroundBeats {

/**
 * @class EffectsView
 * @brief Main UI component for effects management and control
 * 
 * Provides interface for:
 * - Adding/removing effects
 * - Creating serial/parallel routing groups
 * - Managing effect parameters
 * - Loading/saving presets
 */
class EffectsView : public juce::Component,
                    public juce::Timer {
public:
    EffectsView(EffectsChain& chain, PresetManager& presetManager);
    ~EffectsView() override;
    
    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    /**
     * @brief Draw effect chain connections
     */
    void drawConnections(juce::Graphics& g);
    
    /**
     * @brief Calculate connection path between components
     */
    juce::Path calculateConnectionPath(juce::Component* source, juce::Component* dest) const;
    
    /**
     * @brief Update connection paths for all effects
     */
    void updateConnectionPaths();
    
    /**
     * @brief Get the list of connected components in processing order
     */
    std::vector<std::pair<int, juce::Component*>> getOrderedComponents() const;

private:
    /**
     * @brief Create UI for an effect node
     */
    std::unique_ptr<EffectNodeComponent> createEffectNode(int nodeId);
    
    /**
     * @brief Create UI for a routing group
     */
    std::unique_ptr<RoutingGroupComponent> createRoutingGroup(int nodeId);
    
    /**
     * @brief Show effect creation menu
     */
    void showAddEffectMenu();
    
    /**
     * @brief Show preset management menu
     */
    void showPresetMenu();
    
    /**
     * @brief Update all UI components to reflect current chain state
     */
    void updateUI();
    
    //==============================================================================
    EffectsChain& effectsChain;
    PresetManager& presets;
    
    juce::TextButton addEffectButton;
    juce::TextButton presetButton;
    
    std::unique_ptr<juce::Viewport> viewport;
    std::unique_ptr<juce::Component> containerComponent;
    
    std::map<int, std::unique_ptr<juce::Component>> nodeComponents;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectsView)
};

} // namespace UndergroundBeats
