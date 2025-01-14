
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FuzzerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    FuzzerAudioProcessorEditor(FuzzerAudioProcessor&);
    ~FuzzerAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FuzzerAudioProcessor& audioProcessor;

    void initWindow();

    juce::Slider inputSlider;
    juce::Slider outputSlider;
    juce::Slider mixSlider;

    juce::Label inputLabel;
    juce::Label outputLabel;
    juce::Label mixLabel;

    juce::ComboBox menu;
    juce::ComboBox tone;

    juce::Label menuLabel;
    juce::Label toneLabel;

    //juce::TextButton button;

    //juce::ToggleButton toggle;

    //juce::GroupComponent border;
    

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> menuAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> toneAttachment;


    void FuzzerAudioProcessorEditor::syncMenuWithParameter(juce::ComboBox& comboBox, const juce::String& parameterID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FuzzerAudioProcessorEditor)
};
