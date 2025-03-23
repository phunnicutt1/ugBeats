#include "HelpSystem.h"

namespace UndergroundBeats {

HelpSystem::HelpSystem()
{
    initializeDefaultContent();
}

void HelpSystem::addTopic(const std::string& id, const HelpTopic& topic)
{
    topics[id] = topic;
}

const HelpTopic* HelpSystem::getTopic(const std::string& id) const
{
    auto it = topics.find(id);
    return it != topics.end() ? &it->second : nullptr;
}

std::vector<std::string> HelpSystem::getTopicsByCategory(const std::string& category) const
{
    std::vector<std::string> result;
    for (const auto& [id, topic] : topics) {
        if (topic.category == category) {
            result.push_back(id);
        }
    }
    return result;
}

std::vector<std::string> HelpSystem::getCategories() const
{
    std::vector<std::string> categories;
    for (const auto& [id, topic] : topics) {
        if (std::find(categories.begin(), categories.end(), topic.category) == categories.end()) {
            categories.push_back(topic.category);
        }
    }
    return categories;
}

std::vector<std::string> HelpSystem::searchTopics(const std::string& query) const
{
    std::vector<std::string> results;
    for (const auto& [id, topic] : topics) {
        // Simple case-insensitive search in title and content
        auto lowercaseQuery = juce::String(query).toLowerCase().toStdString();
        auto lowercaseTitle = juce::String(topic.title).toLowerCase().toStdString();
        auto lowercaseContent = juce::String(topic.content).toLowerCase().toStdString();
        
        if (lowercaseTitle.find(lowercaseQuery) != std::string::npos ||
            lowercaseContent.find(lowercaseQuery) != std::string::npos) {
            results.push_back(id);
        }
    }
    return results;
}

std::string HelpSystem::getContextHelp(const juce::Component* component) const
{
    if (!component) return "";
    
    auto it = componentHelp.find(component->getName().toStdString());
    return it != componentHelp.end() ? it->second : "";
}

void HelpSystem::initializeDefaultContent()
{
    // Add basic application help topics
    addTopic("getting_started", {
        "Getting Started",
        "Welcome to Underground Beats! This guide will help you get started with making music...",
        "Basics"
    });
    
    addTopic("mixer_basics", {
        "Mixer Basics",
        "The mixer view allows you to control levels, panning, and routing for all tracks...",
        "Mixer"
    });
    
    addTopic("effects_basics", {
        "Effects Basics",
        "Learn how to add and configure audio effects to enhance your sound...",
        "Effects"
    });
    
    addTopic("sequencer_basics", {
        "Sequencer Basics",
        "Create patterns and arrange your music using the sequencer view...",
        "Sequencer"
    });
    
    // Add component-specific help
    componentHelp["MixerView"] = "Control track levels, panning, and routing. Double-click faders to reset.";
    componentHelp["EffectsView"] = "Add and arrange effects. Drag effects to reorder them.";
    componentHelp["SequencerView"] = "Create and edit patterns. Use grid snap for precise timing.";
}

} // namespace UndergroundBeats
