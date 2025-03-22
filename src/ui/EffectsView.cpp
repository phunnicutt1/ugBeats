/*
 * Underground Beats
 * EffectsView.cpp
 */

#include "EffectsView.h"
#include "EffectNodeComponent.h"
#include "RoutingGroupComponent.h"
#include "../effects/Delay.h"
#include "../effects/Reverb.h"
namespace UndergroundBeats {

EffectsView::EffectsView(EffectsChain& chain, PresetManager& presetManager)
    : effectsChain(chain)
    , presets(presetManager)
    , addEffectButton("Add Effect")
    , presetButton("Presets")
{
    // Create scrollable container
    viewport = std::make_unique<juce::Viewport>();
    addAndMakeVisible(*viewport);
    
    containerComponent = std::make_unique<juce::Component>();
    viewport->setViewedComponent(containerComponent.get(), false);
    
    // Add buttons
    addAndMakeVisible(addEffectButton);
    addAndMakeVisible(presetButton);
    
    addEffectButton.onClick = [this]() { showAddEffectMenu(); };
    presetButton.onClick = [this]() { showPresetMenu(); };
    
    // Start timer for UI updates
    startTimerHz(30);
    
    updateUI();
}

EffectsView::~EffectsView() {
    stopTimer();
}

void EffectsView::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    // Draw connections between effects
    drawConnections(g);
}

void EffectsView::resized() {
    auto bounds = getLocalBounds();
    
    // Layout buttons
    auto topArea = bounds.removeFromTop(30);
    addEffectButton.setBounds(topArea.removeFromLeft(100).reduced(2));
    presetButton.setBounds(topArea.removeFromLeft(100).reduced(2));
    
    // Layout viewport
    viewport->setBounds(bounds);
    
    // Update container size based on content
    int minHeight = std::max(viewport->getHeight(), 300); // Temporary fixed height
    containerComponent->setSize(viewport->getWidth() - viewport->getScrollBarThickness(),
                              minHeight);
}

void EffectsView::timerCallback() {
    // Update parameter values in UI
    for (auto& [id, component] : nodeComponents) {
        if (auto* effectComp = dynamic_cast<EffectNodeComponent*>(component.get())) {
            effectComp->updateParameters();
        } else if (auto* groupComp = dynamic_cast<RoutingGroupComponent*>(component.get())) {
            if (auto* node = effectsChain.getNode(id)) {
                if (node->getType() == RoutingNode::Type::Parallel) {
                    groupComp->setMixLevel(node->getMixLevel());
                }
            }
        }
    }
}

void EffectsView::showAddEffectMenu() {
    juce::PopupMenu menu;
    
    menu.addItem(1, "Add Delay");
    menu.addItem(2, "Add Reverb");
    menu.addSeparator();
    menu.addItem(3, "Add Serial Group");
    menu.addItem(4, "Add Parallel Group");
    
    menu.showMenuAsync(juce::PopupMenu::Options()
                        .withTargetComponent(addEffectButton),
                      [this](int result) {
        if (result == 0) return;
        
        switch (result) {
            case 1: // Delay
                effectsChain.addEffect(std::make_unique<Delay>());
                break;
            case 2: // Reverb
                effectsChain.addEffect(std::make_unique<Reverb>());
                break;
            case 3: // Serial Group
                effectsChain.createGroup(RoutingNode::Type::Serial);
                break;
            case 4: // Parallel Group
                effectsChain.createGroup(RoutingNode::Type::Parallel);
                break;
        }
        
        updateUI();
    });
}

void EffectsView::showPresetMenu() {
    juce::PopupMenu menu;
    
    // Add factory presets submenu
    juce::PopupMenu factoryMenu;
    int itemId = 1;
    
    for (const auto& category : presets.getCategories()) {
        if (presets.isFactoryCategory(category)) {
            juce::PopupMenu categoryMenu;
            for (const auto& preset : presets.getPresetList(category)) {
                categoryMenu.addItem(itemId++, preset.name);
            }
            factoryMenu.addSubMenu(category, categoryMenu);
        }
    }
    menu.addSubMenu("Factory Presets", factoryMenu);
    
    // Add user presets submenu
    juce::PopupMenu userMenu;
    for (const auto& category : presets.getCategories()) {
        if (!presets.isFactoryCategory(category)) {
            juce::PopupMenu categoryMenu;
            for (const auto& preset : presets.getPresetList(category)) {
                categoryMenu.addItem(itemId++, preset.name);
            }
            userMenu.addSubMenu(category, categoryMenu);
        }
    }
    menu.addSubMenu("User Presets", userMenu);
    
    menu.addSeparator();
    menu.addItem(-1, "Save Current Preset...");
    
    // Store the total number of presets for later reference
    const int totalPresets = itemId - 1;
    
    menu.showMenuAsync(juce::PopupMenu::Options()
                        .withTargetComponent(presetButton),
                      [this](int result) {
        if (result == 0) return;
        
        if (result == -1) {
            // Show save preset dialog
            auto* dialog = new juce::AlertWindow("Save Preset",
                "Enter preset information:",
                juce::AlertWindow::NoIcon);
                
            dialog->addTextEditor("name", "New Preset", "Preset Name:");
            dialog->addTextEditor("category", "User", "Category:");
            dialog->addTextEditor("description", "", "Description:");
            
            dialog->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
            dialog->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
            
            dialog->enterModalState(true,
                juce::ModalCallbackFunction::create(
                    [this, dialog](int result) {
                        if (result == 1) {
                            PresetInfo info(
                                dialog->getTextEditorContents("name"),
                                dialog->getTextEditorContents("category"),
                                dialog->getTextEditorContents("description"),
                                false // Not a factory preset
                            );
                            
                            if (presets.savePreset(effectsChain, info)) {
                                juce::AlertWindow::showMessageBoxAsync(
                                    juce::AlertWindow::InfoIcon,
                                    "Success",
                                    "Preset saved successfully."
                                );
                            } else {
                                juce::AlertWindow::showMessageBoxAsync(
                                    juce::AlertWindow::WarningIcon,
                                    "Error",
                                    "Failed to save preset."
                                );
                            }
                        }
                        delete dialog;
                    }
                )
            );
        } else {
            // Load the selected preset
            const auto& categories = presets.getCategories();
            for (const auto& category : categories) {
                auto presetList = presets.getPresetList(category);
                if (result <= static_cast<int>(presetList.size())) {
                    const auto& preset = presetList[result - 1];
                    if (presets.loadPreset(effectsChain, preset.name)) {
                        updateUI();
                    } else {
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::WarningIcon,
                            "Error",
                            "Failed to load preset: " + preset.name
                        );
                    }
                    break;
                }
                result -= presetList.size();
            }
        }
    });
}

void EffectsView::updateUI() {
    // Clear existing components
    nodeComponents.clear();
    containerComponent->removeAllChildren();
    
    // Create components for the node hierarchy starting from root
    std::function<void(int parentId, int& xPos, int& yPos)> createNodeComponents;
    createNodeComponents = [this, &createNodeComponents](int parentId, int& xPos, int& yPos) {
        if (auto* node = effectsChain.getNode(parentId)) {
            std::unique_ptr<juce::Component> component;
            
            if (node->getType() == RoutingNode::Type::Effect) {
                // Create effect node component
                auto effectComp = std::make_unique<EffectNodeComponent>(effectsChain, parentId);
                component = std::move(effectComp);
            } else {
                // Create routing group component
                auto groupComp = std::make_unique<RoutingGroupComponent>(effectsChain, parentId);
                
                // Process children
                int childX = 0;
                int childY = 0;
                for (const auto& child : node->getChildren()) {
                    createNodeComponents(child->getNodeId(), childX, childY);
                }
                
                component = std::move(groupComp);
            }
            
            if (component) {
                component->setTopLeftPosition(xPos, yPos);
                nodeComponents[parentId] = std::move(component);
                containerComponent->addAndMakeVisible(*nodeComponents[parentId]);
                
                // Update position for next component
                xPos += component->getWidth() + 10;
                yPos = std::max(yPos, component->getBottom() + 10);
            }
        }
    };
    
    // Start creating components from root node
    int xPos = 10;
    int yPos = 10;
    createNodeComponents(effectsChain.getRootNodeId(), xPos, yPos);
    
    resized();
}

} // namespace UndergroundBeats
