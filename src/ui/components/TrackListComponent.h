#pragma once

#include <JuceHeader.h>
#include "../../sequencer/Track.h"
#include <memory>
#include <vector>

class TrackListComponent : public juce::Component,
                         public juce::ListBoxModel
{
public:
    TrackListComponent();
    ~TrackListComponent() override;

    // ListBoxModel overrides
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent& event) override;
    
    // Track management
    void addTrack(std::shared_ptr<UndergroundBeats::Track> track);
    void removeTrack(int index);
    std::shared_ptr<UndergroundBeats::Track> getTrack(int index) const;
    int getSelectedTrackIndex() const;
    void setSelectedTrack(int index);
    
    // Track list state
    void clear();
    int getTrackCount() const;
    
    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Callbacks for track changes
    std::function<void(int)> onTrackSelected;
    std::function<void(int)> onTrackMuted;
    std::function<void(int)> onTrackSoloed;

private:
    void createTrackControls(int index);
    void updateTrackControls();
    
    juce::ListBox listBox;
    std::vector<std::shared_ptr<UndergroundBeats::Track>> tracks;
    
    struct TrackControls {
        std::unique_ptr<juce::TextButton> muteButton;
        std::unique_ptr<juce::TextButton> soloButton;
        std::unique_ptr<juce::Slider> volumeSlider;
        std::unique_ptr<juce::Slider> panSlider;
    };
    std::vector<TrackControls> trackControls;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListComponent)
};
