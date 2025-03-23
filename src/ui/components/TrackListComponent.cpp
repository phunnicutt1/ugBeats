#include "TrackListComponent.h"

TrackListComponent::TrackListComponent()
{
    addAndMakeVisible(listBox);
    listBox.setModel(this);
    listBox.setMultipleSelectionEnabled(false);
    listBox.setRowHeight(80); // Height for track name + controls
}

TrackListComponent::~TrackListComponent()
{
    listBox.setModel(nullptr);
}

void TrackListComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackListComponent::resized()
{
    listBox.setBounds(getLocalBounds());
    updateTrackControls();
}

int TrackListComponent::getNumRows()
{
    return static_cast<int>(tracks.size());
}

void TrackListComponent::paintListBoxItem(int rowNumber, juce::Graphics& g, 
                                        int width, int height, bool rowIsSelected)
{
    if (rowNumber >= 0 && rowNumber < static_cast<int>(tracks.size()))
    {
        auto track = tracks[rowNumber];
        
        // Draw selection background
        if (rowIsSelected)
            g.fillAll(getLookAndFeel().findColour(juce::TextEditor::highlightColourId));
            
        // Draw track name
        g.setColour(getLookAndFeel().findColour(juce::ListBox::textColourId));
        g.setFont(16.0f);
        g.drawText(track->getName(), 10, 5, width - 20, 25, juce::Justification::left);
        
        // Controls are managed by trackControls vector
    }
}

void TrackListComponent::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    setSelectedTrack(row);
}

void TrackListComponent::addTrack(std::shared_ptr<UndergroundBeats::Track> track)
{
    if (track)
    {
        tracks.push_back(track);
        createTrackControls(tracks.size() - 1);
        listBox.updateContent();
        updateTrackControls();
    }
}

void TrackListComponent::removeTrack(int index)
{
    if (index >= 0 && index < static_cast<int>(tracks.size()))
    {
        tracks.erase(tracks.begin() + index);
        trackControls.erase(trackControls.begin() + index);
        listBox.updateContent();
        updateTrackControls();
    }
}

std::shared_ptr<UndergroundBeats::Track> TrackListComponent::getTrack(int index) const
{
    if (index >= 0 && index < static_cast<int>(tracks.size()))
        return tracks[index];
    return nullptr;
}

int TrackListComponent::getSelectedTrackIndex() const
{
    return listBox.getSelectedRow();
}

void TrackListComponent::setSelectedTrack(int index)
{
    listBox.selectRow(index);
    if (onTrackSelected)
        onTrackSelected(index);
}

void TrackListComponent::clear()
{
    tracks.clear();
    trackControls.clear();
    listBox.updateContent();
}

int TrackListComponent::getTrackCount() const
{
    return static_cast<int>(tracks.size());
}

void TrackListComponent::createTrackControls(int index)
{
    TrackControls controls;
    
    // Create mute button
    controls.muteButton = std::make_unique<juce::TextButton>("M");
    controls.muteButton->setClickingTogglesState(true);
    controls.muteButton->onClick = [this, index, &controls]() {
        if (auto track = getTrack(index))
        {
            track->setMuted(controls.muteButton->getToggleState());
            if (onTrackMuted)
                onTrackMuted(index);
        }
    };
    
    // Create solo button
    controls.soloButton = std::make_unique<juce::TextButton>("S");
    controls.soloButton->setClickingTogglesState(true);
    controls.soloButton->onClick = [this, index, &controls]() {
        if (auto track = getTrack(index))
        {
            track->setSolo(controls.soloButton->getToggleState());
            if (onTrackSoloed)
                onTrackSoloed(index);
        }
    };
    
    // Create volume slider
    controls.volumeSlider = std::make_unique<juce::Slider>();
    controls.volumeSlider->setSliderStyle(juce::Slider::LinearVertical);
    controls.volumeSlider->setRange(0.0, 1.0);
    controls.volumeSlider->setValue(1.0);
    controls.volumeSlider->onValueChange = [this, index, &controls]() {
        if (auto track = getTrack(index))
            track->setVolume(static_cast<float>(controls.volumeSlider->getValue()));
    };
    
    // Create pan slider
    controls.panSlider = std::make_unique<juce::Slider>();
    controls.panSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    controls.panSlider->setRange(-1.0, 1.0);
    controls.panSlider->setValue(0.0);
    controls.panSlider->onValueChange = [this, index, &controls]() {
        if (auto track = getTrack(index))
            track->setPan(static_cast<float>(controls.panSlider->getValue()));
    };
    
    addAndMakeVisible(*controls.muteButton);
    addAndMakeVisible(*controls.soloButton);
    addAndMakeVisible(*controls.volumeSlider);
    addAndMakeVisible(*controls.panSlider);
    
    trackControls.push_back(std::move(controls));
}

void TrackListComponent::updateTrackControls()
{
    auto rowHeight = listBox.getRowHeight();
    auto viewport = listBox.getViewport();
    if (!viewport)
        return;
        
    auto viewportPosition = viewport->getViewPosition();
    
    for (int i = 0; i < static_cast<int>(trackControls.size()); ++i)
    {
        auto& controls = trackControls[i];
        auto rowBounds = listBox.getRowPosition(i, true);
        
        // Adjust bounds relative to viewport position
        rowBounds.translate(0, -viewportPosition.getY());
        
        // Layout controls in the row
        auto controlArea = rowBounds.removeFromBottom(rowHeight - 30);
        auto buttonWidth = 30;
        auto sliderWidth = 80;
        
        controls.muteButton->setBounds(controlArea.removeFromLeft(buttonWidth));
        controlArea.removeFromLeft(5); // spacing
        controls.soloButton->setBounds(controlArea.removeFromLeft(buttonWidth));
        controlArea.removeFromLeft(10); // spacing
        controls.volumeSlider->setBounds(controlArea.removeFromLeft(sliderWidth));
        controlArea.removeFromLeft(10); // spacing
        controls.panSlider->setBounds(controlArea.removeFromLeft(sliderWidth));
    }
}
