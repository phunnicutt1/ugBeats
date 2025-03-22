#pragma once

#include <JuceHeader.h>
#include <string>
#include <map>
#include <memory>

namespace UndergroundBeats {

/**
 * @struct HelpTopic
 * @brief Represents a single help topic with content and related topics
 */
struct HelpTopic {
    std::string title;
    std::string content;
    std::vector<std::string> relatedTopics;
    std::string category;
    
    HelpTopic(const std::string& t = "", const std::string& c = "", 
              const std::string& cat = "General") 
        : title(t), content(c), category(cat) {}
};

/**
 * @class HelpSystem
 * @brief Manages the application's help content and documentation
 * 
 * Provides:
 * - Topic-based help organization
 * - Context-sensitive help
 * - Search functionality
 * - Quick tips and tooltips
 */
class HelpSystem {
public:
    HelpSystem();
    ~HelpSystem() = default;
    
    /**
     * @brief Add a help topic
     */
    void addTopic(const std::string& id, const HelpTopic& topic);
    
    /**
     * @brief Get a help topic by ID
     */
    const HelpTopic* getTopic(const std::string& id) const;
    
    /**
     * @brief Get topics by category
     */
    std::vector<std::string> getTopicsByCategory(const std::string& category) const;
    
    /**
     * @brief Get all categories
     */
    std::vector<std::string> getCategories() const;
    
    /**
     * @brief Search help topics
     */
    std::vector<std::string> searchTopics(const std::string& query) const;
    
    /**
     * @brief Get context-sensitive help for a component
     */
    std::string getContextHelp(const juce::Component* component) const;
    
    /**
     * @brief Initialize default help content
     */
    void initializeDefaultContent();

private:
    std::map<std::string, HelpTopic> topics;
    std::map<std::string, std::string> componentHelp;
    
    /**
     * @brief Load help content from resource files
     */
    void loadHelpContent();
};

} // namespace UndergroundBeats
