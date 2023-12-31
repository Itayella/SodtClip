/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SodtClipAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SodtClipAudioProcessor();
    ~SodtClipAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState& getPluginState();
    juce::AudioProcessorValueTreeState myValueTreeState;
    
    
    
private:
    juce::dsp::ProcessSpec mySpec;
    
    juce::dsp::IIR::Filter<float> myFilter;
   // juce::dsp::IIR::Filter<float>::CoefficientsPtr coefficient;
    
    /*
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    
    MonoChain leftChannel, righChannel;
    juce::dsp::StateVariableTPTFilter<float> myFilter;
    //using MonoChain = 
    
    
    
    //myFilter[2];
    
    //juce::IIRFilter::setCoefficients(<#const IIRCoefficients &newCoefficients#>)
    */
    juce::AudioParameterFloat* inputPtr;
    juce::AudioParameterFloat* tonePtr;
    juce::AudioParameterFloat* lpfPtr;
    juce::AudioParameterFloat* mixPtr;
    juce::AudioParameterFloat* outputPtr;
    juce::AudioParameterBool* bypassPtr;
    
    
    
    
    
    
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SodtClipAudioProcessor)
};
