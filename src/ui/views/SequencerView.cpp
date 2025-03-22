#include "SequencerView.h"

SequencerView::SequencerView(std::shared_ptr<UndergroundBeats::Sequencer> seq)
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
    
    addAndMakeVisible(newPatternButton);
    newPatternButton.onClick = [this]() {
        // TODO: Create new pattern through sequencer
        patternGrid->setGridSize(16, 16); // Default size
    };
    
    addAndMakeVisible(deletePatternButton);
    deletePatternButton.onClick = [this]() {
        // TODO: Delete current pattern through sequencer
    };
    
    addAndMakeVisible(snapToGridButton);
    snapToGridButton.setToggleState(true, juce::dontSendNotification);
    snapToGridButton.onClick = [this]() {
        patternGrid->setSnapToGrid(snapToGridButton.getToggleState());
    };
    
    // Initialize track list and controls
    addAndMakeVisible(trackList);
    addAndMakeVisible(addTrackButton);
    addTrackButton.onClick = [this]() {
        // TODO: Add new track through sequencer
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
    auto controlsArea = area.removeFromTop(controlsHeight);
    newPatternButton.setBounds(controlsArea.removeFromLeft(100));
    controlsArea.removeFromLeft(10); // spacing
    deletePatternButton.setBounds(controlsArea.removeFromLeft(100));
    controlsArea.removeFromLeft(10); // spacing
    snapToGridButton.setBounds(controlsArea.removeFromLeft(100));
    
    // Layout track list and controls
    auto trackListWidth = 150;
    auto trackListArea = area.removeFromLeft(trackListWidth);
    trackList.setBounds(trackListArea.removeFromTop(area.getHeight() - controlsHeight));
    addTrackButton.setBounds(trackListArea);
    
    // Pattern grid takes remaining space
    patternGrid->setBounds(area);
}
