/*
 * Underground Beats
 * PresetManager.h
 * 
 * Manages presets for effect chains
 */

#pragma once

#include "EffectsChain.h"
#include <JuceHeader.h>
#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace UndergroundBeats {

/**
 * @class PresetManager
 * @brief Manages saving and loading of effect chain presets
 * 
 * Handles:
 * - Saving effect chain configurations including routing and automation
 * - Loading presets with proper recreation of the effect chain
 * - Preset categories and metadata
 * - Factory presets and user presets
 */
class PresetManager {
public:
    /**
     * @brief Preset metadata structure
     */
    struct PresetInfo {
        std::string name;
        std::string category;
        std::string description;
        bool isFactory;
        juce::Time created;
        juce::Time modified;
        
        PresetInfo(const std::string& name = "",
                  const std::string& category = "",
                  const std::string& description = "",
                  bool isFactory = false)
            : name(name), category(category), description(description)
            , isFactory(isFactory), created(juce::Time::getCurrentTime())
            , modified(created) {}
    };
    
    PresetManager();
    ~PresetManager();
    
    /**
     * @brief Save the current effect chain as a preset
     * 
     * @param chain The effect chain to save
     * @param info Preset metadata
     * @return true if successful
     */
    bool savePreset(EffectsChain& chain, const PresetInfo& info);
    
    /**
     * @brief Load a preset into an effect chain
     * 
     * @param chain The effect chain to load into
     * @param presetName Name of the preset to load
     * @return true if successful
     */
    bool loadPreset(EffectsChain& chain, const std::string& presetName);
    
    /**
     * @brief Get list of available presets
     * 
     * @param category Optional category filter
     * @return Vector of preset info
     */
    std::vector<PresetInfo> getPresetList(const std::string& category = "") const;
    
    /**
     * @brief Get categories of available presets
     * 
     * @return Vector of category names
     */
    std::vector<std::string> getCategories() const;
    
    /**
     * @brief Delete a preset
     * 
     * @param presetName Name of preset to delete
     * @return true if successful
     */
    bool deletePreset(const std::string& presetName);
    
    /**
     * @brief Get information about a specific preset
     * 
     * @param presetName Name of preset
     * @return PresetInfo if found, nullopt if not
     */
    std::optional<PresetInfo> getPresetInfo(const std::string& presetName) const;
    
    /**
     * @brief Initialize factory presets
     * 
     * Creates default presets that ship with the application
     */
    void initializeFactoryPresets();

private:
    /**
     * @brief Get the file path for a preset
     * 
     * @param presetName Name of preset
     * @return File path
     */
    juce::File getPresetFile(const std::string& presetName) const;
    
    /**
     * @brief Get the directory for storing presets
     * 
     * @param factory True for factory presets, false for user presets
     * @return Directory path
     */
    juce::File getPresetDirectory(bool factory = false) const;
    
    /**
     * @brief Load preset metadata from XML
     * 
     * @param xml XML element containing metadata
     * @return PresetInfo structure
     */
    PresetInfo loadPresetInfo(const juce::XmlElement& xml) const;
    
    /**
     * @brief Save preset metadata to XML
     * 
     * @param info Preset metadata to save
     * @return XML element containing metadata
     */
    std::unique_ptr<juce::XmlElement> createPresetInfoXml(const PresetInfo& info) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};

} // namespace UndergroundBeats
