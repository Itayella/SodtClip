/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SodtClipAudioProcessor::SodtClipAudioProcessor()

: myValueTreeState(*this, nullptr, juce::Identifier("myParameter"),
        {
    std::make_unique<juce::AudioParameterFloat>("Input", "Input", -24.0f, 24.f, 0.f),
    std::make_unique<juce::AudioParameterFloat>("Tone", "Tone", juce::NormalisableRange<float>(0.01f, 0.9f, 0.01f, 0.28f), 0.1f),
    //replace Tone param with AudioParameterChoise. with Dark/Mid/highMid/Bright options
    
    
    std::make_unique<juce::AudioParameterFloat>("Freq", "Freq", 20.f, 18000.0f, 60.f),
    std::make_unique<juce::AudioParameterFloat>("Mix", "Mix", 0.0f, 1.0f, 1.f),
    std::make_unique<juce::AudioParameterFloat>("Output", "Output", -48.0f, 6.0f, 0.f),
    std::make_unique<juce::AudioParameterBool>("Bypass", "Bypass", false)
    
    
})
{
    inputPtr = dynamic_cast<juce::AudioParameterFloat*>(myValueTreeState.getParameter("Input"));
    tonePtr = dynamic_cast<juce::AudioParameterFloat*>(myValueTreeState.getParameter("Tone"));
    lpfPtr = dynamic_cast<juce::AudioParameterFloat*>(myValueTreeState.getParameter("Freq"));
    mixPtr = dynamic_cast<juce::AudioParameterFloat*>(myValueTreeState.getParameter("Mix"));
    outputPtr = dynamic_cast<juce::AudioParameterFloat*>(myValueTreeState.getParameter("Output"));
    bypassPtr = dynamic_cast<juce::AudioParameterBool*>(myValueTreeState.getParameter("bypassPtr"));
        
}



SodtClipAudioProcessor::~SodtClipAudioProcessor()
{
}

//==============================================================================
const juce::String SodtClipAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SodtClipAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SodtClipAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SodtClipAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SodtClipAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SodtClipAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SodtClipAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SodtClipAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SodtClipAudioProcessor::getProgramName (int index)
{
    return {};
}

void SodtClipAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SodtClipAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mySpec.sampleRate = sampleRate;
    mySpec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    mySpec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
   // auto inputDB = inputPtr->get();
    //auto drive = drivePtr->get();
    //auto mix = mixPtr->get();
    //auto output = outputPtr->get();
    const auto filterCutoff = lpfPtr->get();
    
    
    
  /*   for (int i = 0; i < getTotalNumInputChannels(); ++i) // Assuming stereo signal
        myFilter[i].setCoefficients(juce::IIRCoefficients::makePeakFilter(sampleRate, filterCutoff, 1.f, 12.f));
    */
}

void SodtClipAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SodtClipAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SodtClipAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    /*
    
  auto drive = drivePtr->get();
  auto mix = mixPtr->get();
  auto output = outputPtr->get();
   */
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const float inputValue = inputPtr->get();
    const float toneValue = tonePtr->get();
    const float mixValue = mixPtr->get();
    const float outputValue = outputPtr->get();

    
    


    juce::dsp::AudioBlock<float> audioBlock {buffer};
    
    //input = (input - 1/3) * ((input)^3)
    
    for (int sample = 0; sample < audioBlock.getNumSamples(); sample++)
    {
        for (int channel = 0; channel < audioBlock.getNumChannels(); channel++)
        {
            auto* data = audioBlock.getChannelPointer(channel);
            
            const auto input = data[sample] * juce::Decibels::decibelsToGain(inputValue);
            
            const auto processA = input - 0.333;
           //const auto filtered = myFilter[channel].processSingleSampleRaw(data[sample]);

            const auto processB = std::pow(toneValue, data[sample]);
            
            auto clipProcess = processA * processB;
            
            auto blend = input * (1. - mixValue) + clipProcess * mixValue;
            
            blend *= juce::Decibels::decibelsToGain(outputValue);
            
            data[sample] = blend;
            
        }
    }

  
    
    
    }

//==============================================================================
bool SodtClipAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SodtClipAudioProcessor::createEditor()
{
    //return new SodtClipAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void SodtClipAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SodtClipAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SodtClipAudioProcessor();
}
