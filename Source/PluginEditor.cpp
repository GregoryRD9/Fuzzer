
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FuzzerAudioProcessorEditor::FuzzerAudioProcessorEditor(FuzzerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    //inputSlider
    inputSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor._treeState, "input", inputSlider);

    inputSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    inputSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 32);
    inputSlider.setRange(0.0, 24.0, 0.1);
    inputSlider.setDoubleClickReturnValue(true, 0.0);
    inputSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke.withAlpha(0.5f));
    inputSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(inputSlider);

    inputSlider.onValueChange = [this]()
        {
            DBG(inputSlider.getValue()); //Debug
        };

    //inputLabel
    addAndMakeVisible(inputLabel);
    inputLabel.setText("Input", juce::dontSendNotification);
    inputLabel.attachToComponent(&inputSlider, false);
    inputLabel.setJustificationType(juce::Justification::centred);



    //outputSlider
    outputSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor._treeState, "output", outputSlider);

    outputSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outputSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 32);
    outputSlider.setRange(-20.0, 20.0, 0.1);
    outputSlider.setDoubleClickReturnValue(true, 0.0);
    outputSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke.withAlpha(0.5f));
    outputSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(outputSlider);

    //outputLabel
    addAndMakeVisible(outputLabel);
    outputLabel.setText("Output", juce::dontSendNotification);
    outputLabel.attachToComponent(&outputSlider, false);
    outputLabel.setJustificationType(juce::Justification::centred);
    


    //mixSlider
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor._treeState, "mix", mixSlider);

    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 32);
    mixSlider.setRange(0.0f, 1.0f, 0.01f);
    mixSlider.setDoubleClickReturnValue(true, 0.0f);
    //mixSlider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::transparentBlack); //rotarySliderFillColourId 
    addAndMakeVisible(mixSlider);

    //mixLabel
    addAndMakeVisible(mixLabel);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    mixLabel.setJustificationType(juce::Justification::centred);




    //menu
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor._treeState, "fuzzModel", menu);

    addAndMakeVisible(menu);
    menu.setTextWhenNothingSelected("Hard Clipping"); //"Hard Clipping" as default, otherwise "Fuzzes"
    menu.addItem("Hard Clipping", 1);
    menu.addItem("Redux", 2);
    menu.addItem("Fat", 3);

    // Add a listener to ensure the menu updates correctly
    menu.onChange = [this]() { syncMenuWithParameter(menu, "fuzzModel"); };

    //menuLabel
    addAndMakeVisible(menuLabel);
    menuLabel.setText("Fuzzes", juce::dontSendNotification);
    menuLabel.attachToComponent(&menu, false);
    menuLabel.setJustificationType(juce::Justification::centred);




    //tone
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor._treeState, "tone", tone);

    addAndMakeVisible(tone);
    tone.setTextWhenNothingSelected("Normal"); //"Normal" as default, otherwise "Tone"
    tone.addItem("Brightest", 1);
    tone.addItem("Brighter", 2);
    tone.addItem("Normal", 3);
    tone.addItem("Darker", 4);
    tone.addItem("Darkest", 5);

    // Add a listener to ensure the menu updates correctly
    tone.onChange = [this]() { syncMenuWithParameter(tone, "tone"); };

    //toneLabel
    addAndMakeVisible(toneLabel);
    toneLabel.setText("Tone", juce::dontSendNotification);
    toneLabel.attachToComponent(&tone, false);
    toneLabel.setJustificationType(juce::Justification::centred);





    //button
    //addAndMakeVisible(button);
    //button.setButtonText("Button");
    //button.setClickingTogglesState(true);
    //button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::lightgoldenrodyellow);
    //button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::dimgrey);
    //button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);
    //button.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);

    //button.onClick = [this]()
    //    {
    //        DBG(static_cast<int>(button.getToggleState())); //Debug
    //    };

    //toggle
    //addAndMakeVisible(toggle);
    //toggle.setButtonText("Power");



    //border
    //addAndMakeVisible(border);
    //border.setText("Utility");


    // Sync ComboBoxes
    syncMenuWithParameter(menu, "fuzzModel");
    syncMenuWithParameter(tone, "tone");

    initWindow();

    setSize(700, 500);
}

FuzzerAudioProcessorEditor::~FuzzerAudioProcessorEditor()
{
}

//==============================================================================
void FuzzerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.brighter(0.05)); 

    g.setColour(juce::Colours::whitesmoke.withAlpha(0.8f));
    g.setFont(juce::FontOptions(36.0f));
    g.drawFittedText("Fuzzer", getLocalBounds(), juce::Justification::centredTop, 1);
}

void FuzzerAudioProcessorEditor::resized()
{
    auto leftMargin = getWidth() * 0.02;
    auto topMargin  = getHeight() * 0.15;
    auto dialSize   = getWidth() * 0.32;

    auto buttonWidth = dialSize * 0.35;
    auto buttonHeight = buttonWidth * 0.5;

    auto toggleSize = getWidth() * 0.3;

    inputSlider.setBounds(leftMargin, topMargin, dialSize, dialSize); 
    outputSlider.setBounds(leftMargin * 32.0f, topMargin, dialSize, dialSize);
    mixSlider.setBounds(leftMargin * 16.0f, topMargin * 3.0f, dialSize, dialSize / 2.0f);

    menu.setBounds(inputSlider.getX() + inputSlider.getWidth() + ((inputSlider.getX() + inputSlider.getWidth()) / 45.0f),
        topMargin - 1.0f, buttonWidth * 2.5f, buttonHeight * 1.2f);
    tone.setBounds(inputSlider.getX() + inputSlider.getWidth() - ((inputSlider.getX() + inputSlider.getWidth()) / 1.7f),
        topMargin * 5.6f, buttonWidth * 6.5f, buttonHeight * 2.0f);

    //button.setBounds(inputSlider.getX() + inputSlider.getWidth() * 0.33, inputSlider.getY() + inputSlider.getHeight(), 
        //buttonWidth, buttonHeight);
    //toggle.setBounds(button.getX(), button.getY() + button.getHeight() + 12, toggleSize, toggleSize);
    //border.setBounds(leftMargin, topMargin * 0.5, dialSize, inputSlider.getY() + inputSlider.getHeight() + button.getHeight());
}


// Sync ComboBox with Parameter
void FuzzerAudioProcessorEditor::syncMenuWithParameter(juce::ComboBox& comboBox, const juce::String& parameterID)
{
    auto* paramValue = audioProcessor._treeState.getRawParameterValue(parameterID);

    if (paramValue != nullptr)
    {
        int currentValue = static_cast<int>(paramValue->load());
        comboBox.setSelectedId(currentValue + 1, juce::dontSendNotification);
    }
    else
    {
        DBG("Parameter " + parameterID + " not found!");
    }
}

