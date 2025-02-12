/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce::dsp
{

//==============================================================================
/**
    Utility class for logarithmically smoothed linear values.

    Logarithmically smoothed values can be more relevant than linear ones for
    specific cases such as algorithm change smoothing, using two of them in
    opposite directions.

    The gradient of the logarithmic/exponential slope can be configured by
    calling LogRampedValue::setLogParameters.

    @see SmoothedValue

    @tags{DSP}
*/
template <typename FloatType>
class LogRampedValue   : public SmoothedValueBase <LogRampedValue <FloatType>>
{
public:
    //==============================================================================
    /** Constructor. */
    LogRampedValue() = default;

    /** Constructor. */
    LogRampedValue (FloatType initialValue) noexcept
    {
        // Visual Studio can't handle base class initialisation with CRTP
        this->currentValue = initialValue;
        this->target = initialValue;
    }

    //==============================================================================
    /** Sets the behaviour of the log ramp.
        @param midPointAmplitudedB           Sets the amplitude of the mid point in
                                             decibels, with the target value at 0 dB
                                             and the initial value at -inf dB
        @param rateOfChangeShouldIncrease    If true then the ramp starts shallow
                                             and gets progressively steeper, if false
                                             then the ramp is initially steep and
                                             flattens out as you approach the target
                                             value
    */
    void setLogParameters (FloatType midPointAmplitudedB, bool rateOfChangeShouldIncrease) noexcept
    {
        jassert (midPointAmplitudedB < (FloatType) 0.0);
        B = Decibels::decibelsToGain (midPointAmplitudedB);

        increasingRateOfChange = rateOfChangeShouldIncrease;
    }

    //==============================================================================
    /** Reset to a new sample rate and ramp length.
        @param sampleRate           The sample rate
        @param rampLengthInSeconds  The duration of the ramp in seconds
    */
    void reset (double sampleRate, double rampLengthInSeconds) noexcept
    {
        jassert (sampleRate > 0 && rampLengthInSeconds >= 0);
        reset ((int) std::floor (rampLengthInSeconds * sampleRate));
    }

    /** Set a new ramp length directly in samples.
        @param numSteps                   The number of samples over which the ramp should be active
    */
    void reset (int numSteps) noexcept
    {
        stepsToTarget = numSteps;

        this->setCurrentAndTargetValue (this->target);

        updateRampParameters();
    }

    //==============================================================================
    /** Set a new target value.

        @param newValue     The new target value
    */
    void setTargetValue (FloatType newValue) noexcept
    {
        if (approximatelyEqual (newValue, this->target))
            return;

        if (stepsToTarget <= 0)
        {
            this->setCurrentAndTargetValue (newValue);
            return;
        }

        this->target = newValue;
        this->countdown = stepsToTarget;
        source = this->currentValue;

        updateRampParameters();
    }

    //==============================================================================
    /** Compute the next value.
        @returns Smoothed value
    */
    FloatType getNextValue() noexcept
    {
        if (! this->isSmoothing())
            return this->target;

        --(this->countdown);

        temp *= r; temp += d;
        this->currentValue = jmap (temp, source, this->target);

        return this->currentValue;
    }

    //==============================================================================
    /** Skip the next numSamples samples.

        This is identical to calling getNextValue numSamples times.
        @see getNextValue
    */
    FloatType skip (int numSamples) noexcept
    {
        if (numSamples >= this->countdown)
        {
            this->setCurrentAndTargetValue (this->target);
            return this->target;
        }

        this->countdown -= numSamples;

        auto rN = (FloatType) std::pow (r, numSamples);
        temp *= rN;
        temp += d * (rN - (FloatType) 1) / (r - (FloatType) 1);

        this->currentValue = jmap (temp, source, this->target);
        return this->currentValue;
    }

private:
    //==============================================================================
    void updateRampParameters()
    {
        auto D = increasingRateOfChange ? B : (FloatType) 1 - B;
        auto base = ((FloatType) 1 / D) - (FloatType) 1;
        r = std::pow (base, (FloatType) 2 / (FloatType) stepsToTarget);
        auto rN = std::pow (r, (FloatType) stepsToTarget);
        d = (r - (FloatType) 1) / (rN - (FloatType) 1);
        temp = 0;
    }

    //==============================================================================
    bool increasingRateOfChange = true;
    FloatType B = Decibels::decibelsToGain ((FloatType) -40);

    int stepsToTarget = 0;
    FloatType temp = 0, source = 0, r = 0, d = 1;
};

} // namespace juce::dsp
