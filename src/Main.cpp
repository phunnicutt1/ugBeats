#include <JuceHeader.h>
#include "ui/AppComponent.h"

//==============================================================================
class UndergroundBeatsApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    UndergroundBeatsApplication() {}

    const juce::String getApplicationName() override       { return "Underground Beats"; }
    const juce::String getApplicationVersion() override    { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& /*commandLine*/) override
    {
        try
        {
            // Print debug message
            juce::Logger::writeToLog("Initializing Underground Beats...");
            
            // Initialize the main window
            mainWindow.reset (new MainWindow (getApplicationName()));
            
            juce::Logger::writeToLog("Application initialized successfully.");
        }
        catch (const std::exception& e)
        {
            juce::Logger::writeToLog("Exception during initialization: " + juce::String(e.what()));
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                                 "Initialization Error",
                                                 "Failed to initialize application: " + juce::String(e.what()));
            quit();
        }
        catch (...)
        {
            juce::Logger::writeToLog("Unknown exception during initialization");
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                                 "Initialization Error",
                                                 "Failed to initialize application due to an unknown error.");
            quit();
        }
    }

    void shutdown() override
    {
        // Clean up resources
        mainWindow = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String& /*commandLine*/) override
    {
        // Handle another instance starting - currently ignored
    }

    //==============================================================================
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            juce::Logger::writeToLog("Creating MainWindow...");
            
            try
            {
                setUsingNativeTitleBar (true);
                juce::Logger::writeToLog("Creating AppComponent...");
                setContentOwned (new UndergroundBeats::AppComponent(), true);
                juce::Logger::writeToLog("AppComponent created successfully.");

               #if JUCE_IOS || JUCE_ANDROID
                setFullScreen (true);
               #else
                setResizable (true, true);
                centreWithSize (1024, 768);
               #endif

                setVisible (true);
                juce::Logger::writeToLog("MainWindow created successfully.");
            }
            catch (const std::exception& e)
            {
                juce::Logger::writeToLog("Exception in MainWindow constructor: " + juce::String(e.what()));
                throw;
            }
            catch (...)
            {
                juce::Logger::writeToLog("Unknown exception in MainWindow constructor");
                throw;
            }
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (UndergroundBeatsApplication)
