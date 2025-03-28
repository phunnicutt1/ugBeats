#pragma once

#include <JuceHeader.h>
#include "../../sequencer/Sequencer.h"

class SequencerView : public juce::Component,
                     private juce::Timer
{
public:
    SequencerView(std::shared_ptr<UndergroundBeats::Sequencer> sequencer);
    ~SequencerView() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateTransportState();
    void updateTempoDisplay();
    
    // Engine reference
    std::shared_ptr<UndergroundBeats::Sequencer> sequencer;
    
    // Transport controls
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton recordButton { "Record" };
    
    // Timeline controls
    juce::Slider tempoSlider;
    juce::Label tempoLabel;
    juce::ComboBox timeSignatureCombo;
    
    // Pattern editing
    std::unique_ptr<PatternGridComponent> patternGrid;
    juce::ComboBox patternSelector;
    juce::TextButton newPatternButton { "New Pattern" };
    juce::TextButton deletePatternButton { "Delete Pattern" };
    juce::ToggleButton snapToGridButton { "Snap to Grid" };
    
    void updatePatternList();
    
    // Track management
    std::unique_ptr<TrackListComponent> trackList;
    juce::TextButton addTrackButton { "Add Track" };
    juce::TextButton deleteTrackButton { "Delete Track" };
    
    // Sequencer engine reference
    std::shared_ptr<UndergroundBeats::Sequencer> sequencer;
    
    // UI update interval in milliseconds
    static constexpr int UPDATE_INTERVAL_MS = 50;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequencerView)
};
