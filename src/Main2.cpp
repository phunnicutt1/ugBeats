// src/Main.cpp - Minimal version to test build
#include <JuceHeader.h>

class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        setSize(600, 400);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        g.setFont(24.0f);
        g.drawText("Underground Beats Test", getLocalBounds(), juce::Justification::centred, true);
    }

    void resized() override
    {
    }
};

class UndergroundBeatsApplication : public juce::JUCEApplication
{
public:
    UndergroundBeatsApplication() {}

    const juce::String getApplicationName() override { return "Underground Beats Test"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name, juce::Colours::darkgrey, DocumentWindow::allButtons)
        {
            setContentOwned(new MainComponent(), true);
            setResizable(true, true);
            centreWithSize(600, 400);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(UndergroundBeatsApplication)