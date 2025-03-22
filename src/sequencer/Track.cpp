#include "Track.h"

namespace UndergroundBeats {

Track::Track(const std::string& name)
    : name(name)
{
}

Track::~Track()
{
}

void Track::setName(const std::string& newName)
{
    name = newName;
}

std::string Track::getName() const
{
    return name;
}

void Track::setMuted(bool shouldBeMuted)
{
    muted = shouldBeMuted;
}

bool Track::isMuted() const
{
    return muted;
}

void Track::setSolo(bool shouldBeSolo)
{
    solo = shouldBeSolo;
}

bool Track::isSolo() const
{
    return solo;
}

void Track::setVolume(float newVolume)
{
    volume = juce::jlimit(0.0f, 1.0f, newVolume);
}

float Track::getVolume() const
{
    return volume;
}

void Track::setPan(float newPan)
{
    pan = juce::jlimit(-1.0f, 1.0f, newPan);
}

float Track::getPan() const
{
    return pan;
}

void Track::addPattern(std::shared_ptr<Pattern> pattern)
{
    if (pattern)
        patterns.push_back(pattern);
}

void Track::removePattern(size_t index)
{
    if (index < patterns.size())
        patterns.erase(patterns.begin() + index);
}

std::shared_ptr<Pattern> Track::getPattern(size_t index) const
{
    if (index < patterns.size())
        return patterns[index];
    return nullptr;
}

size_t Track::getPatternCount() const
{
    return patterns.size();
}

void Track::setMidiChannel(int channel)
{
    midiChannel = juce::jlimit(1, 16, channel);
}

int Track::getMidiChannel() const
{
    return midiChannel;
}

void Track::setMidiProgram(int program)
{
    midiProgram = juce::jlimit(0, 127, program);
}

int Track::getMidiProgram() const
{
    return midiProgram;
}

void Track::saveToXml(juce::XmlElement& xml) const
{
    xml.setAttribute("name", name);
    xml.setAttribute("muted", muted);
    xml.setAttribute("solo", solo);
    xml.setAttribute("volume", volume);
    xml.setAttribute("pan", pan);
    xml.setAttribute("midiChannel", midiChannel);
    xml.setAttribute("midiProgram", midiProgram);
    
    auto* patternsXml = xml.createNewChildElement("patterns");
    for (const auto& pattern : patterns)
    {
        if (pattern)
        {
            auto* patternXml = patternsXml->createNewChildElement("pattern");
            // TODO: Implement pattern serialization
            patternXml->setAttribute("name", pattern->getName());
        }
    }
}

void Track::loadFromXml(const juce::XmlElement& xml)
{
    name = xml.getStringAttribute("name", "New Track");
    muted = xml.getBoolAttribute("muted", false);
    solo = xml.getBoolAttribute("solo", false);
    volume = xml.getDoubleAttribute("volume", 1.0);
    pan = xml.getDoubleAttribute("pan", 0.0);
    midiChannel = xml.getIntAttribute("midiChannel", 1);
    midiProgram = xml.getIntAttribute("midiProgram", 0);
    
    patterns.clear();
    if (auto* patternsXml = xml.getChildByName("patterns"))
    {
        for (auto* patternXml : patternsXml->getChildWithTagNameIterator("pattern"))
        {
            // TODO: Implement pattern deserialization
            auto pattern = std::make_shared<Pattern>(
                patternXml->getStringAttribute("name", "Untitled Pattern")
            );
            patterns.push_back(pattern);
        }
    }
}

} // namespace UndergroundBeats
