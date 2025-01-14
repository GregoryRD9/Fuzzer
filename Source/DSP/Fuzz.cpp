
#include "Fuzz.h"
#include <JuceHeader.h>
#include <cmath>

template <typename SampleType>
Fuzz<SampleType>::Fuzz()
{
}


template <typename SampleType>
void Fuzz<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{

    _sampleRate = spec.sampleRate;

    //dc offset highpass filter
    _dcFilter.prepare(spec);
    _dcFilter.setCutoffFrequency(10.0);
    _dcFilter.setType(juce::dsp::LinkwitzRileyFilter<float>::Type::highpass);

    // Prepare low-pass filter
    _lowPassFilter.prepare(spec);
    _lowPassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    _lowPassFilter.setCutoffFrequency(_lowPassCutoff); // Default cutoff frequency


    //Tone
    // Prepare low-pass filter
    _toneLowPassFilter.prepare(spec);
    _toneLowPassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    // Prepare high-pass filter for tone control (high frequencies)
    _toneHighPassFilter.prepare(spec);
    _toneHighPassFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    //TONE

    reset();
}

template <typename SampleType>
void Fuzz<SampleType>::reset()
{
    if (_sampleRate <= 0) return;

    _input.reset(_sampleRate, 0.02); //20 milliseconds => (rampLengthIn Seconds)
    _input.setTargetValue(0.0);

    _mix.reset(_sampleRate, 0.02);
    _mix.setTargetValue(1.0);

    _output.reset(_sampleRate, 0.02);
    _output.setTargetValue(0.0);

    // Reset dcFilter
    _dcFilter.reset();

    // Reset low pass filter
    _lowPassFilter.reset();
}
template <typename SampleType>
void Fuzz<SampleType>::setDrive(SampleType newDrive)
{
    _input.setTargetValue(newDrive);
}

template <typename SampleType>
void Fuzz<SampleType>::setMix(SampleType newMix)
{
    _mix.setTargetValue(newMix);
}

template <typename SampleType>
void Fuzz<SampleType>::setOutput(SampleType newOutput)
{
    _output.setTargetValue(newOutput);
}

template <typename SampleType>
void Fuzz<SampleType>::setFuzzModel(FuzzModel newModel)
{
    switch (newModel)
    {
    case FuzzModel::kHard:
    {
        _model = newModel;
        break;
    }

    case FuzzModel::kRedux:
    {
        _model = newModel;
        break;
    }

    case FuzzModel::kFat:
    {
        _model = newModel;
        break;
    }

    }
}

template <typename SampleType>
void Fuzz<SampleType>::setToneCharacter(ToneCharacter newToneChar)
{
    float darkerFreq = 2500.0f; //First LowPass Stage
    float darkestFreq = 900.0f; //Second LowPass Stage
    float brighterFreq = 200.0f; //First HighPass Stage
    float brightestFreq = 400.0f; //Second HighPass Stage

    switch (newToneChar)
    {
    case ToneCharacter::darkest:
    {
        _toneChar = newToneChar;

        _toneLowPassFilter.setCutoffFrequency(darkestFreq);
        _toneHighPassFilter.setCutoffFrequency(10.0f);
        break;
    }

    case ToneCharacter::darker:
    {
        _toneChar = newToneChar;

        _toneLowPassFilter.setCutoffFrequency(darkerFreq);
        _toneHighPassFilter.setCutoffFrequency(10.0f);
        break;
    }

    case ToneCharacter::normal:
    {
        _toneChar = newToneChar;
        
        _toneLowPassFilter.setCutoffFrequency(13000.0f);
        _toneHighPassFilter.setCutoffFrequency(10.0f); 
        break;
    }

    case ToneCharacter::brighter:
    {
        _toneChar = newToneChar;

        _toneHighPassFilter.setCutoffFrequency(brighterFreq);
        _toneLowPassFilter.setCutoffFrequency(20000.0f);
        break;
    }

    case ToneCharacter::brightest:
    {
        _toneChar = newToneChar;

        _toneHighPassFilter.setCutoffFrequency(brightestFreq);
        _toneLowPassFilter.setCutoffFrequency(20000.0f);
        break;
    }
    }
}

template class Fuzz<float>;
template class Fuzz<double>;


