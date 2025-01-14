
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/Fuzz.h"

//==============================================================================
FuzzerAudioProcessor::FuzzerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
    , _treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    _treeState.addParameterListener(fuzzModelID, this);
    _treeState.addParameterListener(inputID, this);
    _treeState.addParameterListener(mixID, this);
    _treeState.addParameterListener(outputID, this);
    _treeState.addParameterListener(toneID, this);
}

//Destructor
FuzzerAudioProcessor::~FuzzerAudioProcessor()
{
    _treeState.removeParameterListener(fuzzModelID, this);
    _treeState.removeParameterListener(inputID, this);
    _treeState.removeParameterListener(mixID, this);
    _treeState.removeParameterListener(outputID, this);
    _treeState.removeParameterListener(toneID, this);
}


//==============================================================================

// Create Parameter Layout
juce::AudioProcessorValueTreeState::ParameterLayout FuzzerAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;


    juce::StringArray fuzzModels = { "Hard", "Redux", "Fat"};

    juce::StringArray toneCharacters = { "Brightest", "Brighter", "Normal", "Darker", "Darkest"};


    //Fuzz Model Selector
    auto pFuzzModel = std::make_unique<juce::AudioParameterChoice>(fuzzModelID, fuzzModelName, fuzzModels, 0);

    //Input/Drive
    auto pDrive = std::make_unique<juce::AudioParameterFloat>(inputID, inputName, 0.0f, 24.0f, 0.0f);
    //The format is -> (from this value, to this value, init on this value)

    //Mix
    auto pMix = std::make_unique<juce::AudioParameterFloat>(mixID, mixName, 0.0f, 1.0f, 1.0f);

    //Output
    auto pOutput = std::make_unique<juce::AudioParameterFloat>(outputID, outputName, -20.0f, 20.0f, 0.0f);

    //Tone
    auto pToneCharacter = std::make_unique<juce::AudioParameterChoice>(toneID, toneName, toneCharacters, 2);

    params.push_back(std::move(pFuzzModel));
    params.push_back(std::move(pDrive));
    params.push_back(std::move(pMix));
    params.push_back(std::move(pOutput));
    params.push_back(std::move(pToneCharacter));

    return { params.begin(), params.end() };
}

//==============================================================================

void FuzzerAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    updateParameters(); // Update other parameters
}



//==============================================================================

void FuzzerAudioProcessor::updateParameters()
{
    auto model = static_cast<int>(_treeState.getRawParameterValue(fuzzModelID)->load());
    switch (model)
    {
    case 0: _fuzzModule.setFuzzModel(Fuzz<float>::FuzzModel::kHard);
        break;

    case 1: _fuzzModule.setFuzzModel(Fuzz<float>::FuzzModel::kRedux);
        break;

    case 2: _fuzzModule.setFuzzModel(Fuzz<float>::FuzzModel::kFat);
        break;
    }

    auto tCharacter = static_cast<int>(_treeState.getRawParameterValue(toneID)->load());
    switch (tCharacter)
    {
    case 0: _fuzzModule.setToneCharacter(Fuzz<float>::ToneCharacter::brightest);
        break;

    case 1: _fuzzModule.setToneCharacter(Fuzz<float>::ToneCharacter::brighter);
        break;

    case 2: _fuzzModule.setToneCharacter(Fuzz<float>::ToneCharacter::normal);
        break;

    case 3: _fuzzModule.setToneCharacter(Fuzz<float>::ToneCharacter::darker);
        break;

    case 4: _fuzzModule.setToneCharacter(Fuzz<float>::ToneCharacter::darkest);
        break;
    }

    _fuzzModule.setDrive(_treeState.getRawParameterValue(inputID)->load());
    _fuzzModule.setMix(_treeState.getRawParameterValue(mixID)->load());
    _fuzzModule.setOutput(_treeState.getRawParameterValue(outputID)->load());

}

//==============================================================================
const juce::String FuzzerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FuzzerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FuzzerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FuzzerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FuzzerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FuzzerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int FuzzerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FuzzerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String FuzzerAudioProcessor::getProgramName(int index)
{
    return {};
}

void FuzzerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================

// Prepare to Play
void FuzzerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    //Initialize spec for dsp modules
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    _fuzzModule.prepare(spec);

    updateParameters(); // Ensure all parameters are updated
}

void FuzzerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FuzzerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

void FuzzerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();



    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block{ buffer };

    _fuzzModule.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool FuzzerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FuzzerAudioProcessor::createEditor()
{
    return new FuzzerAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this); //It was this ONE !!!!!!!!!!!!!! For Plugin Editor (Interface) !!!!!!!!!
}

//==============================================================================
void FuzzerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FuzzerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FuzzerAudioProcessor();
}
