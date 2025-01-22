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

namespace juce
{

//==============================================================================
/** A relative measure of time.

    The time is stored as a number of seconds, at double-precision floating
    point accuracy, and may be positive or negative.

    If you need an absolute time, (i.e. a date + time), see the Time class.

    @tags{Core}
*/
class JUCE_API  RelativeTime
{
public:
    //==============================================================================
    /** Creates a RelativeTime.

        @param seconds  the number of seconds, which may be +ve or -ve.
        @see milliseconds, minutes, hours, days, weeks
    */
    explicit RelativeTime (double seconds = 0.0) noexcept;

    /** Copies another relative time. */
    RelativeTime (const RelativeTime& other) noexcept;

    /** Copies another relative time. */
    RelativeTime& operator= (const RelativeTime& other) noexcept;

    /** Destructor. */
    ~RelativeTime() noexcept;

    //==============================================================================
    /** Creates a new RelativeTime object representing a number of milliseconds.
        @see seconds, minutes, hours, days, weeks
    */
    static RelativeTime milliseconds (int milliseconds) noexcept;

    /** Creates a new RelativeTime object representing a number of milliseconds.
        @see seconds, minutes, hours, days, weeks
    */
    static RelativeTime milliseconds (int64 milliseconds) noexcept;

    /** Creates a new RelativeTime object representing a number of seconds.
        @see milliseconds, minutes, hours, days, weeks
    */
    static RelativeTime seconds (double seconds) noexcept;

    /** Creates a new RelativeTime object representing a number of minutes.
        @see milliseconds, hours, days, weeks
    */
    static RelativeTime minutes (double numberOfMinutes) noexcept;

    /** Creates a new RelativeTime object representing a number of hours.
        @see milliseconds, minutes, days, weeks
    */
    static RelativeTime hours (double numberOfHours) noexcept;

    /** Creates a new RelativeTime object representing a number of days.
        @see milliseconds, minutes, hours, weeks
    */
    static RelativeTime days (double numberOfDays) noexcept;

    /** Creates a new RelativeTime object representing a number of weeks.
        @see milliseconds, minutes, hours, days
    */
    static RelativeTime weeks (double numberOfWeeks) noexcept;

    //==============================================================================
    /** Returns the number of milliseconds this time represents.
        @see milliseconds, inSeconds, inMinutes, inHours, inDays, inWeeks
    */
    int64 inMilliseconds() const noexcept;

    /** Returns the number of seconds this time represents.
        @see inMilliseconds, inMinutes, inHours, inDays, inWeeks
    */
    double inSeconds() const noexcept       { return numSeconds; }

    /** Returns the number of minutes this time represents.
        @see inMilliseconds, inSeconds, inHours, inDays, inWeeks
    */
    double inMinutes() const noexcept;

    /** Returns the number of hours this time represents.
        @see inMilliseconds, inSeconds, inMinutes, inDays, inWeeks
    */
    double inHours() const noexcept;

    /** Returns the number of days this time represents.
        @see inMilliseconds, inSeconds, inMinutes, inHours, inWeeks
    */
    double inDays() const noexcept;

    /** Returns the number of weeks this time represents.
        @see inMilliseconds, inSeconds, inMinutes, inHours, inDays
    */
    double inWeeks() const noexcept;

    /** Returns a readable textual description of the time.

        The exact format of the string returned will depend on
        the magnitude of the time - e.g.

        "1 min 4 secs", "1 hr 45 mins", "2 weeks 5 days", "140 ms"

        so that only the two most significant units are printed.

        The returnValueForZeroTime value is the result that is returned if the
        length is zero. Depending on your application you might want to use this
        to return something more relevant like "empty" or "0 secs", etc.

        @see inMilliseconds, inSeconds, inMinutes, inHours, inDays, inWeeks
    */
    String getDescription (const String& returnValueForZeroTime = "0") const;

    //==============================================================================
    /** This returns a string that roughly describes how long ago this time was, which
        can be handy for showing ages of files, etc.
        This will only attempt to be accurate to within the nearest order of magnitude
        so returns strings such as "5 years", "2 weeks", "< 1 minute", "< 1 sec" etc.
    */
    String getApproximateDescription() const;

    //==============================================================================
    /** Adds another RelativeTime to this one. */
    RelativeTime operator+= (RelativeTime timeToAdd) noexcept;
    /** Subtracts another RelativeTime from this one. */
    RelativeTime operator-= (RelativeTime timeToSubtract) noexcept;

    /** Adds a number of seconds to this time. */
    RelativeTime operator+= (double secondsToAdd) noexcept;
    /** Subtracts a number of seconds from this time. */
    RelativeTime operator-= (double secondsToSubtract) noexcept;

private:
    //==============================================================================
    double numSeconds;
};

//==============================================================================
/** Compares two RelativeTimes. */
JUCE_API bool JUCE_CALLTYPE operator== (RelativeTime t1, RelativeTime t2) noexcept;
/** Compares two RelativeTimes. */
JUCE_API bool JUCE_CALLTYPE operator!= (RelativeTime t1, RelativeTime t2) noexcept;
/** Compares two RelativeTimes. */
JUCE_API bool JUCE_CALLTYPE operator>  (RelativeTime t1, RelativeTime t2) noexcept;
/** Compares two RelativeTimes. */
JUCE_API bool JUCE_CALLTYPE operator<  (RelativeTime t1, RelativeTime t2) noexcept;
/** Compares two RelativeTimes. */
JUCE_API bool JUCE_CALLTYPE operator>= (RelativeTime t1, RelativeTime t2) noexcept;
/** Compares two RelativeTimes. */
JUCE_API bool JUCE_CALLTYPE operator<= (RelativeTime t1, RelativeTime t2) noexcept;

//==============================================================================
/** Adds two RelativeTimes together. */
JUCE_API RelativeTime  JUCE_CALLTYPE operator+  (RelativeTime t1, RelativeTime t2) noexcept;
/** Subtracts two RelativeTimes. */
JUCE_API RelativeTime  JUCE_CALLTYPE operator-  (RelativeTime t1, RelativeTime t2) noexcept;

} // namespace juce
