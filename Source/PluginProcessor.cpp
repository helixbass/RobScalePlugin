/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RobScalePluginAudioProcessor::RobScalePluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
  DBG("abc");
}

RobScalePluginAudioProcessor::~RobScalePluginAudioProcessor()
{
}

//==============================================================================
const String RobScalePluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RobScalePluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RobScalePluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RobScalePluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RobScalePluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RobScalePluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RobScalePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RobScalePluginAudioProcessor::setCurrentProgram (int index)
{
}

const String RobScalePluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void RobScalePluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void RobScalePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    DBG("prepare");

    rate = static_cast<float>(sampleRate);
    speed = 0.6f;
    time = 0.0;
    lastNote = -1;
    baseNote = 7;
    lastInterval = 1;
    /* lowestNote = 60; */
    /* highestNote = 100; */
    lowestNote = 33;
    highestNote = 98;
    // offsets = {0, 4, 5, 7, 9};
    offsets[0] = 0;
    offsets[1] = 4;
    offsets[2] = 5;
    offsets[3] = 7;
    offsets[4] = 10;
}

void RobScalePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RobScalePluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

int RobScalePluginAudioProcessor::getInitialNote() {
  return baseNote + 12 * 4;
}

int RobScalePluginAudioProcessor::getWeightedRandom(int random) {
  if (random < -10) return -3;
  if (random < -5) return -2;
  if (random < 0) return -1;
  if (random > 10) return 3;
  if (random > 5) return 2;
  if (random > 0) return 1;
  return 0;
}

int RobScalePluginAudioProcessor::getNextInterval(int currentNote) {
  if (currentNote >= highestNote) {
    std::cout << "too high\n";
    return getWeightedRandom(Random::getSystemRandom().nextInt(Range<int>(-15, 0)));
  }

  if (currentNote <= lowestNote) {
    std::cout << "too low\n";
    return getWeightedRandom(Random::getSystemRandom().nextInt(Range<int>(0, 16)));
  }

  return getWeightedRandom(Random::getSystemRandom().nextInt(Range<int>(-15, 16)));
}

int RobScalePluginAudioProcessor::getScaleIndex(int note) {
  auto offsetInScale = (note - baseNote) % 12;
  for (int offsetIndex = 0; offsetIndex < scaleLength; offsetIndex++) {
    if (offsets[offsetIndex] == offsetInScale) {
      return offsetIndex;
    }
  }
  return -1;
}

int RobScalePluginAudioProcessor::getNextNoteFromInterval(int currentNote, int interval) {
  auto currentScaleIndex = getScaleIndex(currentNote);
  std::cout << "currentScaleIndex " << currentScaleIndex << "\n";

  int octaves = 0;

  if (currentScaleIndex + interval < 0) {
    octaves = -1;
  } else if (currentScaleIndex + interval >= scaleLength) {
    octaves = 1;
  }

  std::cout << "octaves " << octaves << "\n";

  auto scaleIndex = (currentScaleIndex + interval + scaleLength) % scaleLength;

  std::cout << "scaleIndex " << scaleIndex << "\n";
  std::cout << "currentNote " << currentNote << "\n";
  std::cout << "scale diff " << offsets[currentScaleIndex] - offsets[scaleIndex] << "\n";

  auto nextNote = currentNote + (offsets[scaleIndex] - offsets[currentScaleIndex]) + octaves * 12;
  std::cout << "nextNote " << nextNote << "\n";

  return nextNote;
}

int RobScalePluginAudioProcessor::getNextNote() {
  if (lastNote < 0) {
    return getInitialNote();
  }

  lastInterval = getNextInterval(lastNote);
  std::cout << "interval: " << lastInterval << "\n";

  return getNextNoteFromInterval(lastNote, lastInterval);
}

void RobScalePluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }

    auto numSamples = buffer.getNumSamples();

    auto noteDuration = static_cast<int>(std::ceil(rate * 0.25f * (0.1f + (1.0f - speed))));

    midiMessages.clear();

    if ((time + numSamples) >= noteDuration) {
      auto offset = jmax(0, jmin((int) (noteDuration - time), numSamples - 1));

      if (lastNote > 0) {
        midiMessages.addEvent(MidiMessage::noteOff(1, lastNote), offset);
      }

      lastNote = getNextNote();
      midiMessages.addEvent(MidiMessage::noteOn(1, lastNote, (uint8) 85), offset);
    }

    time = (time + numSamples) % noteDuration;
}

//==============================================================================
bool RobScalePluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* RobScalePluginAudioProcessor::createEditor()
{
    return new RobScalePluginAudioProcessorEditor (*this);
}

//==============================================================================
void RobScalePluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RobScalePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RobScalePluginAudioProcessor();
}
