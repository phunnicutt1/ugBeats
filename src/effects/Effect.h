/*
 * Underground Beats
 * Effect.h
 * 
 * Base class for all audio effects
 */

#pragma once

#include <JuceHeader.h>
#include <string>
#include <memory>
#include "ParameterAutomation.h"
#include <map>

namespace UndergroundBeats {

/**
 * @class Effect
 * @brief Base class for all audio effects
 * 
 * The Effect class provides a common interface for all audio effects
 * in the Underground Beats application. It defines methods for processing
 * audio, parameter control, and state management.
 */
class Effect {
public:
    Effect(const std::string& name);
    virtual ~Effect();

    class Parameter {
    public:
        Parameter(const std::string& name, float defaultValue, float minValue, float maxValue)
            : name(name), value(defaultValue), defaultValue(defaultValue)
            , minValue(minValue), maxValue(maxValue) {}
        
        void setValue(float newValue) {
            value = juce::jlimit(minValue, maxValue, newValue);
        }
        
        float getValue() const { return value; }
        float getDefaultValue() const { return defaultValue; }
        float getMinValue() const { return minValue; }
        float getMaxValue() const { return maxValue; }
        const std::string& getName() const { return name; }
        
        void setAutomation(std::unique_ptr<ParameterAutomation> newAutomation) {
            automation = std::move(newAutomation);
            if (automation) {
                automation->addPoint(0.0, value); // Initialize with current value
            }
        }
        
        ParameterAutomation* getAutomation() const { return automation.get(); }
        
        void updateFromAutomation(double time) {
            if (automation) {
                setValue(automation->getValueAtTime(time));
            }
        }
        
    private:
        std::string name;
        float value;
        float defaultValue;
        float minValue;
        float maxValue;
        std::unique_ptr<ParameterAutomation> automation;
    };

protected:
    /**
     * @brief Get a parameter's current value
     * 
     * @param name Parameter name
     * @param defaultValue Value to return if parameter not found
     * @return Current parameter value or defaultValue if not found
     */
    float getParameterValue(const std::string& name, float defaultValue = 0.0f) const {
        if (const auto* param = getParameter(name)) {
            return param->getValue();
        }
        return defaultValue;
    }

    // Storage for effect parameters
    std::map<std::string, std::unique_ptr<Parameter>> parameters;
    
    /**
     * @brief Get the effect name
     * 
     * @return The effect name
     */
    std::string getName() const;

    /**
     * @brief Add a parameter to the effect
     * 
     * @param name Parameter name
     * @param defaultValue Default value
     * @param minValue Minimum value
     * @param maxValue Maximum value
     * @return Parameter* Pointer to the created parameter
     */
    Parameter* addParameter(const std::string& name, float defaultValue, float minValue, float maxValue);

    /**
     * @brief Get a parameter by name
     * 
     * @param name Parameter name
     * @return Parameter* Pointer to the parameter, or nullptr if not found
     */
    Parameter* getParameter(const std::string& name) const;

    /**
     * @brief Update all automated parameters for the current time
     * 
     * @param currentTime Current time in seconds
     */
    void updateAutomation(double currentTime);
    
    /**
     * @brief Set whether the effect is enabled
     * 
     * @param enabled true to enable, false to bypass
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if the effect is enabled
     * 
     * @return true if enabled, false if bypassed
     */
    bool isEnabled() const;
    
    /**
     * @brief Set the effect's wet/dry mix
     * 
     * @param mix The wet/dry mix (0 = dry, 1 = wet)
     */
    void setMix(float mix);
    
    /**
     * @brief Get the current wet/dry mix
     * 
     * @return The current wet/dry mix
     */
    float getMix() const;
    
    /**
     * @brief Process a mono buffer of samples
     * 
     * @param buffer Buffer containing samples to process
     * @param numSamples Number of samples to process
     */
    void process(float* buffer, int numSamples);
    
    /**
     * @brief Process a stereo buffer of samples
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    void processStereo(float* leftBuffer, float* rightBuffer, int numSamples);
    
    /**
     * @brief Prepare the effect for processing
     * 
     * @param sampleRate The sample rate in Hz
     * @param blockSize The maximum block size in samples
     */
    virtual void prepare(double sampleRate, int blockSize);
    
    /**
     * @brief Reset the effect state
     */
    virtual void reset();
    
    /**
     * @brief Create an XML element containing the effect's state
     * 
     * @return XML element containing effect state
     */
    virtual std::unique_ptr<juce::XmlElement> createStateXml() const;
    
    /**
     * @brief Restore effect state from an XML element
     * 
     * @param xml XML element containing effect state
     * @return true if state was successfully restored
     */
    virtual bool restoreStateFromXml(const juce::XmlElement* xml);
    
protected:
    /**
     * @brief Process a single sample (mono)
     * 
     * Derived classes must implement this method to process
     * a single sample.
     * 
     * @param sample The input sample
     * @return The processed sample
     */
    virtual float processSample(float sample) = 0;
    
    /**
     * @brief Process a single sample (stereo)
     * 
     * Derived classes must implement this method to process
     * a single stereo sample.
     * 
     * @param leftSample The left channel input sample
     * @param rightSample The right channel input sample
     * @param leftOutput Pointer to store left channel output
     * @param rightOutput Pointer to store right channel output
     */
    virtual void processSampleStereo(float leftSample, float rightSample, float* leftOutput, float* rightOutput) = 0;
    
    /**
     * @brief Process a buffer of samples with custom implementation
     * 
     * Derived classes can optionally override this method to
     * implement more efficient buffer processing.
     * 
     * @param buffer Buffer containing samples to process
     * @param numSamples Number of samples to process
     */
    virtual void processBuffer(float* buffer, int numSamples);
    
    /**
     * @brief Process a stereo buffer with custom implementation
     * 
     * Derived classes can optionally override this method to
     * implement more efficient stereo buffer processing.
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    virtual void processBufferStereo(float* leftBuffer, float* rightBuffer, int numSamples);
    
    std::string effectName;
    bool enabled;
    float mixLevel;
    double currentSampleRate;
    int currentBlockSize;
    
    // Temporary buffer for wet/dry mixing
    juce::AudioBuffer<float> tempBuffer;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Effect)
};

} // namespace UndergroundBeats
