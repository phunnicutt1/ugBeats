#pragma once

#include <JuceHeader.h>
#include "Pattern.h"
#include <string>
#include <vector>
#include <memory>

namespace UndergroundBeats {

/**
 * @class Track
 * @brief Represents a single track in the sequencer
 * 
 * A Track contains patterns, MIDI routing information, and mixing parameters.
 * It manages pattern playback and handles track-specific settings like mute,
 * solo, and volume.
 */
class Track {
public:
    Track(const std::string& name = "New Track");
    ~Track();
    
    // Track properties
    void setName(const std::string& name);
    std::string getName() const;
    
    void setMuted(bool shouldBeMuted);
    bool isMuted() const;
    
    void setSolo(bool shouldBeSolo);
    bool isSolo() const;
    
    void setVolume(float newVolume);
    float getVolume() const;
    
    void setPan(float newPan);
    float getPan() const;
    
    // Pattern management
    void addPattern(std::shared_ptr<Pattern> pattern);
    void removePattern(size_t index);
    std::shared_ptr<Pattern> getPattern(size_t index) const;
    size_t getPatternCount() const;
    
    // MIDI routing
    void setMidiChannel(int channel);
    int getMidiChannel() const;
    
    void setMidiProgram(int program);
    int getMidiProgram() const;
    
    // Serialization
    void saveToXml(juce::XmlElement& xml) const;
    void loadFromXml(const juce::XmlElement& xml);

private:
    std::string name;
    bool muted { false };
    bool solo { false };
    float volume { 1.0f };
    float pan { 0.0f };
    
    int midiChannel { 1 };
    int midiProgram { 0 };
    
    std::vector<std::shared_ptr<Pattern>> patterns;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};

} // namespace UndergroundBeats
