/*
 * Underground Beats
 * AppComponent.cpp
 * 
 * Implementation of the AppComponent class
 */

#include "AppComponent.h"

namespace UndergroundBeats {

AppComponent::AppComponent()
{
    try
    {
        juce::Logger::writeToLog("AppComponent: Creating MainComponent...");
        
        // Create main component
        mainComponent = std::make_unique<MainComponent>();
        
        juce::Logger::writeToLog("AppComponent: MainComponent created successfully.");
        
        juce::Logger::writeToLog("AppComponent: Making MainComponent visible...");
        addAndMakeVisible(mainComponent.get());
        
        // Set the size of the application window
        setSize(1024, 768);
        
        juce::Logger::writeToLog("AppComponent constructor completed successfully.");
    }
    catch (const std::exception& e)
    {
        juce::Logger::writeToLog("Exception in AppComponent constructor: " + juce::String(e.what()));
        throw;
    }
    catch (...)
    {
        juce::Logger::writeToLog("Unknown exception in AppComponent constructor");
        throw;
    }
}

AppComponent::~AppComponent()
{
    // Clean up resources
}

void AppComponent::resized()
{
    // Main component takes up the entire area
    mainComponent->setBounds(getLocalBounds());
}

void AppComponent::paint(juce::Graphics& g)
{
    // Nothing to paint directly in the app component,
    // let the main component handle painting
}

} // namespace UndergroundBeats
