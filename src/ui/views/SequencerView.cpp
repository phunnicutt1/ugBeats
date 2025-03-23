#include "SequencerView.h"

namespace UndergroundBeats {

SequencerView::SequencerView(std::shared_ptr<Sequencer> seq)
    : sequencer(seq)
{
    // Initialize transport controls
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(recordButton);
    
    playButton.onClick = [this]() { 
        sequencer->play();
        updateTransportState();
    };
    
    stopButton.onClick = [this]() {
        sequencer->stop();
        updateTransportState();
    };
    
    // Initialize tempo controls
    addAndMakeVisible(tempoSlider);
    tempoSlider.setRange(20.0, 300.0, 1.0);
    tempoSlider.setValue(sequencer->getTempo());
    tempoSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    
    tempoSlider.onValueChange = [this]() {
        sequencer->setTempo(tempoSlider.getValue());
    };
    
    addAndMakeVisible(tempoLabel);
    tempoLabel.setText("Tempo", juce::dontSendNotification);
    tempoLabel.attachToComponent(&tempoSlider, true);
    
    // Initialize time signature selector
    addAndMakeVisible(timeSignatureCombo);
    timeSignatureCombo.addItem("4/4", 1);
    timeSignatureCombo.addItem("3/4", 2);
    timeSignatureCombo.addItem("6/8", 3);
    timeSignatureCombo.setSelectedId(1);
    
    timeSignatureCombo.onChange = [this]() {
        int id = timeSignatureCombo.getSelectedId();
        switch (id) {
            case 1: sequencer->setTimeSignature(4, 4); break;
            case 2: sequencer->setTimeSignature(3, 4); break;
            case 3: sequencer->setTimeSignature(6, 8); break;
        }
    };
    
    // Initialize pattern editing controls
    patternGrid = std::make_unique<PatternGridComponent>();
    addAndMakeVisible(*patternGrid);
    
    addAndMakeVisible(patternSelector);
    patternSelector.onChange = [this]() {
        int selectedTrack = trackList->getSelectedTrackIndex();
        if (selectedTrack >= 0) {
            if (auto track = trackList->getTrack(selectedTrack)) {
                int patternIndex = patternSelector.getSelectedItemIndex();
                if (patternIndex >= 0 && patternIndex < static_cast<int>(track->getPatternCount())) {
                    patternGrid->setPattern(track->getPattern(patternIndex));
                }
            }
        }
    };
    
    addAndMakeVisible(newPatternButton);
    newPatternButton.onClick = [this]() {
        // TODO: Create new pattern through sequencer
        patternGrid->setGridSize(16, 16); // Default size
    };
    
    addAndMakeVisible(deletePatternButton);
    deletePatternButton.onClick = []() {
        // TODO: Delete current pattern through sequencer
    };
    
    addAndMakeVisible(snapToGridButton);
    snapToGridButton.setToggleState(true, juce::dontSendNotification);
    snapToGridButton.onClick = [this]() {
        patternGrid->setSnapToGrid(snapToGridButton.getToggleState());
    };
    
    // Initialize track management
    trackList = std::make_unique<TrackListComponent>();
    addAndMakeVisible(*trackList);
    
    addAndMakeVisible(addTrackButton);
    addTrackButton.onClick = [this]() {
        auto track = std::make_shared<UndergroundBeats::Track>(
            ("Track " + juce::String(trackList->getTrackCount() + 1)).toStdString());
        trackList->addTrack(track);
        // TODO: Add track to sequencer engine
    };
    
    addAndMakeVisible(deleteTrackButton);
    deleteTrackButton.onClick = [this]() {
        int selectedTrack = trackList->getSelectedTrackIndex();
        if (selectedTrack >= 0) {
            trackList->removeTrack(selectedTrack);
            // TODO: Remove track from sequencer engine
        }
    };
    
    trackList->onTrackSelected = [this](int index) {
        if (auto track = trackList->getTrack(index)) {
            updatePatternList();
            // If track has any patterns, show the first one
            if (track->getPatternCount() > 0) {
                patternSelector.setSelectedItemIndex(0, juce::sendNotification);
            } else {
                // Create a default pattern for new tracks
                auto pattern = std::make_shared<UndergroundBeats::Pattern>(
                    std::string("Pattern 1"), 4.0); // 4 beats default length
                track->addPattern(pattern);
                updatePatternList();
                patternSelector.setSelectedItemIndex(0, juce::sendNotification);
            }
        }
    };
    
    // Connect pattern creation to selected track
    newPatternButton.onClick = [this]() {
        int selectedTrack = trackList->getSelectedTrackIndex();
        if (selectedTrack >= 0) {
            if (auto track = trackList->getTrack(selectedTrack)) {
                auto pattern = std::make_shared<UndergroundBeats::Pattern>(
                    ("Pattern " + juce::String(track->getPatternCount() + 1)).toStdString());
                track->addPattern(pattern);
                updatePatternList();
                patternSelector.setSelectedItemIndex(track->getPatternCount() - 1, juce::sendNotification);
            }
        }
    };
    
    // Connect pattern deletion to selected track
    deletePatternButton.onClick = [this]() {
        int selectedTrack = trackList->getSelectedTrackIndex();
        if (selectedTrack >= 0) {
            if (auto track = trackList->getTrack(selectedTrack)) {
                int selectedPattern = patternSelector.getSelectedItemIndex();
                if (selectedPattern >= 0 && selectedPattern < static_cast<int>(track->getPatternCount())) {
                    track->removePattern(selectedPattern);
                    updatePatternList();
                    
                    if (track->getPatternCount() > 0) {
                        // Select the previous pattern, or the first if we deleted the first
                        int newSelection = selectedPattern > 0 ? selectedPattern - 1 : 0;
                        patternSelector.setSelectedItemIndex(newSelection, juce::sendNotification);
                    } else {
                        patternGrid->setPattern(nullptr);
                    }
                }
            }
        }
    };
    
    // Start UI update timer
    startTimerHz(1000 / UPDATE_INTERVAL_MS);
}

SequencerView::~SequencerView()
{
    stopTimer();
}

void SequencerView::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SequencerView::timerCallback()
{
    updateTransportState();
    updateTempoDisplay();
}

void SequencerView::updateTransportState()
{
    bool isPlaying = sequencer->isPlaying();
    playButton.setEnabled(!isPlaying);
    stopButton.setEnabled(isPlaying);
    
    // Update transport display (position, etc) here
}

void SequencerView::updateTempoDisplay()
{
    double currentTempo = sequencer->getTempo();
    if (std::abs(currentTempo - tempoSlider.getValue()) > 0.01)
    {
        tempoSlider.setValue(currentTempo, juce::dontSendNotification);
    }
}

void SequencerView::updatePatternList()
{
    patternSelector.clear();
    
    int selectedTrack = trackList->getSelectedTrackIndex();
    if (selectedTrack >= 0) {
        if (auto track = trackList->getTrack(selectedTrack)) {
            for (size_t i = 0; i < track->getPatternCount(); ++i) {
                if (auto pattern = track->getPattern(i)) {
                    patternSelector.addItem(pattern->getName(), i + 1);
                }
            }
        }
    }
}

void SequencerView::resized()
{
    auto area = getLocalBounds();
    
    // Layout transport controls
    auto transportArea = area.removeFromTop(40);
    auto buttonWidth = 80;
    playButton.setBounds(transportArea.removeFromLeft(buttonWidth));
    stopButton.setBounds(transportArea.removeFromLeft(buttonWidth));
    recordButton.setBounds(transportArea.removeFromLeft(buttonWidth));
    
    // Layout tempo and time signature controls
    auto controlsArea = area.removeFromTop(40);
    tempoSlider.setBounds(controlsArea.removeFromLeft(200));
    timeSignatureCombo.setBounds(controlsArea.removeFromLeft(80));
    
    // Layout pattern controls
    auto controlsHeight = 30;
    auto patternControlsArea = area.removeFromTop(controlsHeight);
    patternSelector.setBounds(patternControlsArea.removeFromLeft(150));
    patternControlsArea.removeFromLeft(10); // spacing
    newPatternButton.setBounds(patternControlsArea.removeFromLeft(100));
    patternControlsArea.removeFromLeft(10); // spacing
    deletePatternButton.setBounds(patternControlsArea.removeFromLeft(100));
    patternControlsArea.removeFromLeft(10); // spacing
    snapToGridButton.setBounds(patternControlsArea.removeFromLeft(100));
    
    // Layout track management
    auto trackListWidth = 250; // Increased width for controls
    auto trackListArea = area.removeFromLeft(trackListWidth);
    
    // Track control buttons at bottom
    auto buttonHeight = 30;
    auto buttonArea = trackListArea.removeFromBottom(buttonHeight);
    addTrackButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2));
    deleteTrackButton.setBounds(buttonArea);
    
    // Track list fills remaining space
    trackList->setBounds(trackListArea);
    
    // Pattern grid takes remaining space
    patternGrid->setBounds(area);
}


} // namespace UndergroundBeats
