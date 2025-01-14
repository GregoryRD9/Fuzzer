/*
  ==============================================================================

    InitWindow.cpp
    Created: 10 Jan 2025 1:33:54pm
    Author:  grdip

  ==============================================================================
*/

#include "../PluginEditor.h"


void FuzzerAudioProcessorEditor::initWindow()
{
    setResizable(true, true);
    setResizeLimits(380, 380, 520, 520);
    getConstrainer()->setFixedAspectRatio(1.0f);
}