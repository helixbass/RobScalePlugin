/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class RobScalePluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    RobScalePluginAudioProcessor();
    ~RobScalePluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float speed;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RobScalePluginAudioProcessor)
    int getNextNote();
    int getInitialNote();
    int getNextInterval(int currentNote);
    int getNextNoteFromInterval(int currentNote, int interval);
    int getScaleIndex(int note);
    int getWeightedRandom(int random);

    float rate;
    int time;
    int lastNote;
    int baseNote;
    int lastInterval;
    int lowestNote;
    int highestNote;
    enum {scaleLength = 5};
    int offsets[scaleLength];
};
