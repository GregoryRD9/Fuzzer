
#pragma once
#include <JuceHeader.h>
#include <cmath>

template <typename SampleType>
class Fuzz
{
public:

    Fuzz();

    void prepare(juce::dsp::ProcessSpec& spec);

    void reset();

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        for (size_t n = 0; n < numSamples; ++n)
        {
            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                outputBlock.getChannelPointer(ch)[n] = _dcFilter.processSample(ch, inputBlock.getChannelPointer(ch)[n]); //DC Filter
                outputBlock.getChannelPointer(ch)[n] = processSample(inputBlock.getChannelPointer(ch)[n], ch);
            }
        }
    }


    SampleType processSample(SampleType inputSample, int channel) noexcept
    {
        //Fuzz Models
        switch (_model)
        {
        case FuzzModel::kHard:
        {
            return processHardClipper(inputSample);
            break;
        }

        case FuzzModel::kRedux:
        {
            return processRedux(inputSample);
            break;
        }

        case FuzzModel::kFat:
        {
            return processFat(inputSample);
            break;
        }

        }
    }



    SampleType processHardClipper(SampleType inputSample)
    {
        //Drive (Drives the Signal)
        auto wetSignal = pow((inputSample * juce::Decibels::decibelsToGain(_input.getNextValue())), 2)
            + (inputSample * juce::Decibels::decibelsToGain(_input.getNextValue()));

        if (std::abs(wetSignal) > 0.99) //If the absolute value of the signal is >1, it distorts
            //I do it in 0.99 to introduce some "headroom" before full clipping occurs
        {
            wetSignal *= 0.99 / std::abs(wetSignal); //Distortion function
            //The bigger the abs(wetSignal) is, the smaller it becomes
           //Signals that go beyond ±0.99 are "hard clipped" to this range
        }

        // Lower the Output Volume cause its too loud
        wetSignal *= juce::Decibels::decibelsToGain(-6.0f);

  
        // Apply Filtering
        wetSignal = _toneLowPassFilter.processSample(0, wetSignal);
        wetSignal = _toneHighPassFilter.processSample(0, wetSignal);
    

        //auto mix = (1.0 - _mix.getNextValue()) * inputSample + wetSignal * _mix.getNextValue(); // Mix Dry/Wet OLD

        // Dry/Wet mix calculation
        auto mixValue = _mix.getNextValue();
        auto dryWeight = std::cos(mixValue * juce::MathConstants<float>::halfPi);
        auto wetWeight = std::pow(std::sin(mixValue * juce::MathConstants<float>::halfPi), 1.5); // Non-linear scaling for wet signal
        auto mix = dryWeight * inputSample + wetWeight * wetSignal;

        return mix * juce::Decibels::decibelsToGain(_output.getNextValue());
    }




    SampleType processRedux(SampleType inputSample)
    {
        //Drive (Drives the Signal)
        auto wetSignal = pow((inputSample * juce::Decibels::decibelsToGain(_input.getNextValue())), 3)
            + (inputSample * juce::Decibels::decibelsToGain(_input.getNextValue()));

        if (std::abs(wetSignal) > 0.99) //If the absolute value of the signal is >1, it distorts
            //I do it in 0.99 to introduce some "headroom" before full clipping occurs
        {
            wetSignal *= 0.99 / std::abs(wetSignal) * 3; //Distortion function
            //The bigger the abs(wetSignal) is, the smaller it becomes
           //Signals that go beyond ±0.99 are "hard clipped" to this range
        }

        // Lower the Output Volume cause its too loud
        wetSignal *= juce::Decibels::decibelsToGain(-20.0f);


        // Apply Filtering
        wetSignal = _toneLowPassFilter.processSample(0, wetSignal);
        wetSignal = _toneHighPassFilter.processSample(0, wetSignal);


        //auto mix = (1.0 - _mix.getNextValue()) * inputSample + wetSignal * _mix.getNextValue(); // Mix Dry/Wet OLD

        // Dry/Wet mix calculation
        auto mixValue = _mix.getNextValue();
        auto dryWeight = std::cos(mixValue * juce::MathConstants<float>::halfPi);
        auto wetWeight = std::pow(std::sin(mixValue * juce::MathConstants<float>::halfPi), 1.5); // Non-linear scaling for wet signal
        auto mix = dryWeight * inputSample + wetWeight * wetSignal;

        return mix * juce::Decibels::decibelsToGain(_output.getNextValue());
    }


    SampleType processFat(SampleType inputSample)
    {
        //Drive (Drives the Signal)
        auto wetSignal = pow((inputSample * juce::Decibels::decibelsToGain(_input.getNextValue())), 3)
            + (inputSample * juce::Decibels::decibelsToGain(_input.getNextValue()));

        if (std::abs(wetSignal) > 0.99) //If the absolute value of the signal is >1, it distorts
            //I do it in 0.99 to introduce some "headroom" before full clipping occurs
        {
            wetSignal *= 0.99 / std::abs(wetSignal) * 0.7; //Distortion function
            //The bigger the abs(wetSignal) is, the smaller it becomes
           //Signals that go beyond ±0.99 are "hard clipped" to this range
        }

        // Lower the Output Volume cause its too loud
        wetSignal *= juce::Decibels::decibelsToGain(-8.0f);

       
        // Apply Filtering
        wetSignal = _toneLowPassFilter.processSample(0, wetSignal);
        wetSignal = _toneHighPassFilter.processSample(0, wetSignal);
       

        //auto mix = (1.0 - _mix.getNextValue()) * inputSample + wetSignal * _mix.getNextValue(); // Mix Dry/Wet OLD

        // Dry/Wet mix calculation
        auto mixValue = _mix.getNextValue();
        auto dryWeight = std::cos(mixValue * juce::MathConstants<float>::halfPi);
        auto wetWeight = std::pow(std::sin(mixValue * juce::MathConstants<float>::halfPi), 1.5); // Non-linear scaling for wet signal
        auto mix = dryWeight * inputSample + wetWeight * wetSignal;

        return mix * juce::Decibels::decibelsToGain(_output.getNextValue());
    }


    enum class FuzzModel
    {
        kHard,
        kRedux,
        kFat
    };

    enum class ToneCharacter
    {
        darkest,
        darker,
        normal,
        brighter,
        brightest
    };

    void setDrive(SampleType newDrive);
    void setMix(SampleType newMix);
    void setOutput(SampleType newOutput);
    void setFuzzModel(FuzzModel newModel);
    void setToneCharacter(ToneCharacter newToneChar);



private:


    juce::SmoothedValue<float> _input;
    juce::SmoothedValue<float> _mix;
    juce::SmoothedValue<float> _output;

    juce::dsp::LinkwitzRileyFilter<float> _dcFilter;

    juce::dsp::StateVariableTPTFilter<SampleType> _lowPassFilter; // Low-pass filter
    float _lowPassCutoff = 1100.0f; //Cutoff frequency 1.1 kHz


    //Tone
    //Low Pass
    juce::dsp::StateVariableTPTFilter<SampleType> _toneLowPassFilter; // Low-pass filter

    //High Pass
    juce::dsp::StateVariableTPTFilter<SampleType> _toneHighPassFilter; // High-pass filter
    //TONE


    float _piDivisor = 2.0 / juce::MathConstants<float>::pi;

    float _sampleRate = 44100.0f;

    FuzzModel _model = FuzzModel::kHard;
    
    ToneCharacter _toneChar = ToneCharacter::normal;

    std::array<SampleType, 2> previousSample = { 0.0f, 0.0f }; // For stereo channels (Not used currently)
};
