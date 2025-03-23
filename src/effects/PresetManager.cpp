/*
 * Underground Beats
 * PresetManager.cpp
 */

#include "PresetManager.h"
#include <algorithm>

namespace UndergroundBeats {

PresetManager::PresetManager()
{
    // Create preset directories if they don't exist
    getPresetDirectory(false).createDirectory();
    getPresetDirectory(true).createDirectory();
}

PresetManager::~PresetManager() = default;

bool PresetManager::savePreset(EffectsChain& chain, const PresetInfo& info)
{
    // Create the preset XML
    auto presetXml = std::make_unique<juce::XmlElement>("EffectChainPreset");
    
    // Add metadata
    presetXml->addChildElement(createPresetInfoXml(info).release());
    
    // Add chain state
    if (auto chainState = chain.createStateXml()) {
        presetXml->addChildElement(chainState.release());
    }
    
    // Save to file
    auto presetFile = getPresetFile(info.name);
    return presetXml->writeTo(presetFile);
}

bool PresetManager::loadPreset(EffectsChain& chain, const std::string& presetName)
{
    auto presetFile = getPresetFile(presetName);
    if (!presetFile.existsAsFile()) {
        return false;
    }
    
    // Load and parse XML
    auto presetXml = juce::XmlDocument::parse(presetFile);
    if (!presetXml || presetXml->getTagName() != "EffectChainPreset") {
        return false;
    }
    
    // Find chain state element
    if (auto* chainState = presetXml->getChildByName("EffectChainState")) {
        return chain.restoreStateFromXml(chainState);
    }
    
    return false;
}

std::vector<PresetManager::PresetInfo> PresetManager::getPresetList(const std::string& category) const
{
    std::vector<PresetInfo> presets;
    
    // Helper to load presets from a directory
    auto loadFromDirectory = [&](const juce::File& directory) {
        for (auto& file : directory.findChildFiles(juce::File::findFiles, false, "*.xml")) {
            if (auto xml = juce::XmlDocument::parse(file)) {
                if (auto* infoXml = xml->getChildByName("PresetInfo")) {
                    auto info = loadPresetInfo(*infoXml);
                    if (category.empty() || info.category == category) {
                        presets.push_back(std::move(info));
                    }
                }
            }
        }
    };
    
    // Load factory presets
    loadFromDirectory(getPresetDirectory(true));
    
    // Load user presets
    loadFromDirectory(getPresetDirectory(false));
    
    // Sort by name
    std::sort(presets.begin(), presets.end(),
              [](const PresetInfo& a, const PresetInfo& b) {
                  return a.name < b.name;
              });
    
    return presets;
}

std::vector<std::string> PresetManager::getCategories() const
{
    std::vector<std::string> categories;
    
    for (const auto& preset : getPresetList()) {
        if (std::find(categories.begin(), categories.end(), preset.category) == categories.end()) {
            categories.push_back(preset.category);
        }
    }
    
    std::sort(categories.begin(), categories.end());
    return categories;
}

bool PresetManager::deletePreset(const std::string& presetName)
{
    auto info = getPresetInfo(presetName);
    if (!info || info->isFactory) {
        return false; // Can't delete factory presets
    }
    
    auto presetFile = getPresetFile(presetName);
    return presetFile.deleteFile();
}

std::optional<PresetManager::PresetInfo> PresetManager::getPresetInfo(const std::string& presetName) const
{
    auto presetFile = getPresetFile(presetName);
    if (!presetFile.existsAsFile()) {
        return std::nullopt;
    }
    
    if (auto xml = juce::XmlDocument::parse(presetFile)) {
        if (auto* infoXml = xml->getChildByName("PresetInfo")) {
            return loadPresetInfo(*infoXml);
        }
    }
    
    return std::nullopt;
}

void PresetManager::initializeFactoryPresets()
{
    // Create some basic effect chain presets
    
    // Simple reverb chain
    {
        EffectsChain chain;
        auto info = PresetInfo("Simple Reverb", "Basic", "Basic reverb effect", true);
        chain.addEffect(std::make_unique<Reverb>("Reverb"));
        savePreset(chain, info);
    }
    
    // Delay + Reverb
    {
        EffectsChain chain;
        auto info = PresetInfo("Delay + Reverb", "Basic", "Classic delay into reverb", true);
        chain.addEffect(std::make_unique<Delay>("Delay"));
        chain.addEffect(std::make_unique<Reverb>("Reverb"));
        savePreset(chain, info);
    }
    
    // Parallel Delays
    {
        EffectsChain chain;
        auto info = PresetInfo("Dual Delay", "Creative", "Two parallel delays with different times", true);
        
        auto parallelId = chain.createGroup(RoutingNode::Type::Parallel);
        
        // Add two delays with different settings
        auto delay1 = std::make_unique<Delay>("Delay 1");
        delay1->getParameter("time")->setValue(0.25f);
        chain.addEffect(std::move(delay1), parallelId);
        
        auto delay2 = std::make_unique<Delay>("Delay 2");
        delay2->getParameter("time")->setValue(0.375f);
        chain.addEffect(std::move(delay2), parallelId);
        
        chain.setGroupMix(parallelId, 0.7f);
        savePreset(chain, info);
    }
}

juce::File PresetManager::getPresetFile(const std::string& presetName) const
{
    // First check user presets
    auto userFile = getPresetDirectory(false).getChildFile(presetName + ".xml");
    if (userFile.existsAsFile()) {
        return userFile;
    }
    
    // Then check factory presets
    return getPresetDirectory(true).getChildFile(presetName + ".xml");
}

juce::File PresetManager::getPresetDirectory(bool factory) const
{
    auto appDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                    .getChildFile("UndergroundBeats");
    
    return factory ? appDir.getChildFile("FactoryPresets")
                  : appDir.getChildFile("UserPresets");
}

PresetManager::PresetInfo PresetManager::loadPresetInfo(const juce::XmlElement& xml) const
{
    PresetInfo info;
    info.name = xml.getStringAttribute("name", "");
    info.category = xml.getStringAttribute("category", "");
    info.description = xml.getStringAttribute("description", "");
    info.isFactory = xml.getBoolAttribute("factory", false);
    info.created = juce::Time(xml.getStringAttribute("created", ""));
    info.modified = juce::Time(xml.getStringAttribute("modified", ""));
    return info;
}

std::unique_ptr<juce::XmlElement> PresetManager::createPresetInfoXml(const PresetInfo& info) const
{
    auto xml = std::make_unique<juce::XmlElement>("PresetInfo");
    xml->setAttribute("name", info.name);
    xml->setAttribute("category", info.category);
    xml->setAttribute("description", info.description);
    xml->setAttribute("factory", info.isFactory);
    xml->setAttribute("created", info.created.toString(true, true));
    xml->setAttribute("modified", info.modified.toString(true, true));
    return xml;
}

} // namespace UndergroundBeats
