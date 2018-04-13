/**
 * \file ElecClock.h
 *
 * \ingroup DetectorClocks
 * 
 * \brief Class def header for a class ElecClock
 *
 * @author Kazuhiro Terao (kterao@fnal.gov)
 */

/** \addtogroup DetectorClocks

    @{*/
#ifndef ElecClock_H
#define ElecClock_H

#include "ClockConstants.h"
#include "DetectorClocksException.h"

namespace detinfo {
  /**
   * @brief Class representing the time measured by an electronics clock.
   * 
   * This class represents the status of a running electronics clock.
   * As such, it has:
   * 
   * * _tick frequency_: how many times the clock ticks in one second
   * * _frame period_: the duration of a single frame; clock time is organised
   *   into two levels:
   *     * the time is split into frames
   *     * each frame is split in samples; the first one of a each frame is
   *       sample number 0
   * * _current time_ of the clock, with respect to the time (always "0") the
   *   clock was started at
   * 
   * Note that this object is actually able to manage any (continuous) value of
   * time, and the concepts of frame and sample are not used to store the clock
   * state, which is instead defined by its current time.
   * 
   * All these quantities are stored in real time units as opposed to clock tick
   * counts. The nominal units for all times and frequencies are microseconds
   * and megahertz, respectively, but `ElecClock` will give consistent results
   * as long as the units of frame period and time are the same, and reciprocal
   * to the frequency unit.
   * 
   * The clock can update its time directly (`SetTime()`) or through operators.
   * 
   * The clock started at time 0, with the sample 0 of the frame 0 (that is also
   * tick 0). This implies that *all times and ticks returned by the clock
   * implicitly have the same reference as the input time specified by the
   * constructors or by the last call to `SetTime()`*.
   * 
   * Some usage examples:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * // get a clock with a period of 500 ns (2 MHz) and a frame of 1.6 ms
   * detinfo::ElecClock clock(0.0, 1600.0, 2.0);
   * 
   * std::cout << "New clock:"
   *   << "\n  current time:      " << clock.Time()       // 0.0 us
   *   << "\n  samples per frame: " << clock.FrameTicks() // 3200
   *   << std::endl; 
   * 
   * clock.SetTime(1, 20); // sets the time to sample #20 of frame 1
   * std::cout << "Time set to sample #20 of frame #1:"
   *   << "\n  current time: " << clock.Time()  // 1610.0 us
   *   << "\n  current tick: " << clock.Tick()  // 3220
   *   << std::endl;
   * 
   * clock += 3.7; // add 3.7 us to the current time
   * std::cout << "Added 3.7 microseconds:"
   *   << "\n  current time:  " << clock.Time()       // 1613.7 us
   *   << "\n  current tick:  " << clock.Tick()       // 3227
   *   << "\n  discrete time: " << clock.Time(Time()) // 1613.5 us
   *   << std::endl;
   * 
   * clock += 3; // add 3 more ticks (1.5 us) to the current time
   * std::cout << "Added 3 ticks:"
   *   << "\n  current time:  " << clock.Time()       // 1615.2 us
   *   << "\n  current tick:  " << clock.Tick()       // 3230
   *   << "\n  discrete time: " << clock.Time(Time()) // 1615.0 us
   *   << std::endl;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * @note Most methods and operators have different behaviour according to
   *       whether their argument is of type `double`, in which case it is
   *       interpreted as a time, or `int`, where it is interpreted as a clock
   *       tick. Be especially careful when utilizing data types which are
   *       neither `int` not `double`, because a conversion to one of them will
   *       occur, and you need to be in control of which one.
   */
  class ElecClock{
    
  public:
    
    /**
     * @brief Default constructor.
     * 
     * A default-constructed clock is useless.
     * It is caller's responsibility to set _all_ the information of the
     * default-constructed clock by assignment from another clock.
     * 
     * Better yet, don't use this constructor and instead construct the clock
     * immediately right.
     */
    ElecClock(): ElecClock(0, kTIME_MAX, 1e9) {}

    /**
     * @brief Constructor: sets all values.
     * @param time starting time of the clock [&micro;s]
     * @param frame_period period of the clock [&micro;s]
     * @param frequency clock frequency [MHz]
     * 
     */
    ElecClock(double time, double frame_period, double frequency)
      : fTime(time),
	fFramePeriod(frame_period),
	fFrequency(frequency)
    { 
      if( fFrequency <= 0 ) throw detinfo::DetectorClocksException("Negative frequency is prohibited!"); 
    }
    
  protected:

    //-- Attribute variables --//

    double fTime;        ///< Time in microseconds.
    double fFramePeriod; ///< Frame period in microseconds.
    double fFrequency;   ///< Clock speed in MHz.
    
  public:

    //-- Setters --//

    /**
     * @brief Directly sets the current time on the clock.
     * @param time reference time (expected in microseconds)
     */
    void SetTime(double time) { fTime = time; }

    /**
     * @brief Sets the current time on the clock from frame and sample number.
     * @param sample the number of the sample within the specified frame
     * @param frame the frame containing the time to be sets
     * 
     * The current clock time is set to the instant of the start of the
     * specified sample.
     */
    void SetTime(int sample,
		 int frame)
    { fTime = Time(sample, frame); }

    /**
     * @brief Sets the current time on the clock from frame and sample number.
     * @param sample the number of the sample within the specified frame
     * @param frame the frame containing the time to be sets
     * 
     * The current clock time is set to the instant of the start of the
     * specified sample.
     */
    void SetTime(unsigned int sample,
		 unsigned int frame)
    { SetTime(int(sample),int(frame)); }

    /**
     * @brief Sets the current time on the clock from tick number.
     * @param ticks the number of tick to set the clock to
     * 
     * The current clock time is set to the instant of the start of the
     * specified tick.
     */
    void SetTime(int ticks)
    { fTime = Time(ticks,0); }

    /**
     * @brief Sets the current time on the clock from tick number.
     * @param ticks the number of tick to set the clock to
     * 
     * The current clock time is set to the instant of the start of the
     * specified tick.
     */
    void SetTime(unsigned int ticks)
    { SetTime(int(ticks)); }

    //-- Getters --//

    /**
     * @brief Current time (as stored) in microseconds.
     * 
     * Note that this is different than `Time(Time())`, which is discretized.
     */
    double Time() const { return fTime; }

    /**
     * @brief Returns the absolute time of the start of the specified sample.
     * @param sample sample number within the specified frame
     * @param frame number of the frame the specified sample is in
     * @return time [us] corresponding to the start of the specified sample
     * 
     * The sample number is not checked to be actually within the specified
     * frame.
     * 
     * The returned time is not related to the current time of the clock.
     */
    double Time(int sample, int frame) const { return (sample / fFrequency + frame * fFramePeriod); }

    /**
     * @brief Returns the discretized value of the specified time.
     * @param time a clock time [&micro;s]
     * @return discretized time [&micro;s] (as a floating point number)
     * 
     * The returned time is the start time of the sample `time` falls in.
     * 
     * It is not related to the current time of the clock.
     */
    double Time(double time) const { return Time(Sample(time),Frame(time)); }

    /**
     * @brief Returns the absolute start time of the specified tick.
     * @param ticks a clock time [&micro;s]
     * @return discretized time [&micro;s] (as a floating point number)
     * 
     * The returned time is the start time of the tick which `time` falls in.
     * 
     * It is not related to the current time of the clock.
     */
    double Time(int ticks) const {return ticks/fFrequency; }

    /// Frequency in MHz.
    double Frequency() const { return fFrequency; }

    /// A single frame period in microseconds.
    double FramePeriod() const { return fFramePeriod; }

    /// Current clock tick (that is, the number of tick `Time()` falls in).
    int Ticks() const { return Ticks(fTime); }

    /**
     * @brief Returns the number of tick the specified time falls in.
     * @param time time to be converted in ticks [&micro;s]
     * @return the number of the tick containing the specified time
     * 
     * The tick number 0 starts at time 0.0 &micro;s.
     * 
     * @note The returned value (number of tick) can wrap if the real number of
     *       the tick is beyond the range of the returned data type (`int`).
     */
    int Ticks(double time) const { return (int)(time * fFrequency); }

    /**
     * @brief Returns the number of tick the specified sample falls in.
     * @param sample number of sample in the specified frame
     * @param frame number of frame the specified sample is in
     * @return the number of the tick containing the specified sample
     * 
     * The sample 0 of frame 0 is the tick number 0.
     * 
     * @note The returned value (number of tick) can wrap if the real number of
     *       the tick is beyond the range of the returned data type (`int`).
     */
    int Ticks(int sample, int frame) const { return sample + frame * FrameTicks(); }

    /**
     * @brief Returns number of the sample containing the clock current time.
     * @see `Sample(double)`
     * 
     * The returned value is the number of the sample within a frame, which the
     * current clock time falls in. The number of the frame is not returned.
     * To univocally define the sample, the number of the frame must also be
     * obtained, e.g. via `Frame()`.
     */
    int Sample() const { return Sample(fTime); }

    /**
     * @brief Returns the number of the sample containing the specified time.
     * @param time a clock time [&micro;s]
     * @return number of the sample containing the specified time
     * @see `Frame(double)`
     * 
     * The returned value is the number of the sample within a frame, which the
     * specified time falls in. The number of the frame is not returned.
     * To univocally define the sample, the number of the frame must also be
     * obtained, e.g. via `Frame(double)`.
     * 
     * The result is not related to the current time of the clock.
     */
    int Sample(double time) const { return (int)((time - Frame(time) * fFramePeriod) * fFrequency); }

    /**
     * @brief Returns the number of the sample containing the specified tick.
     * @param tick a clock tick number
     * @return number of the sample containing the specified tick
     * @see `Frame(int)`
     * 
     * The returned value is the number of the sample within a frame, of the
     * specified tick. The number of the frame is not returned.
     * To univocally define the sample, the number of the frame must also be
     * obtained, e.g. via `Frame(int)`.
     * 
     * The result is not related to the current time of the clock.
     */
    int Sample(int tick) const { return (tick % (int)(FrameTicks())); }

    /**
     * @brief Returns the number of the frame containing the clock current time.
     * @see `Frame(double)`
     * 
     * The returned value is the number of the frame which the current clock
     * time falls in.
     */
    int Frame() const { return Frame(fTime); }

    /**
     * @brief Returns the number of the frame containing the specified time.
     * @param time a clock time [&micro;s]
     * @return number of the frame containing the specified time
     * @see `Sample(double)`
     * 
     * The returned value is the number of the frame which the specified time
     * falls in.
     * 
     * The result is not related to the current time of the clock.
     */
    int Frame(double time) const { return (int)(time / fFramePeriod); }

    /**
     * @brief Returns the number of the frame containing the specified tick.
     * @param tick a clock tick number
     * @return number of the frame containing the specified tick
     * @see `Frame(int)`
     * 
     * The returned value is the number of the frame the specified tick belongs
     * to.
     * 
     * The result is not related to the current time of the clock.
     */
    int Frame(int tick) const { return (tick / (int)(FrameTicks())); }

    /// Number ticks in a frame.
    unsigned int FrameTicks() const { return (int)(fFramePeriod * fFrequency);}

    /// A single tick period in microseconds.
    double TickPeriod() const { return 1./fFrequency; }

    
    //-- operators --//

    /// @{
    /// @name Operators to modify clock time.
    
    /// Increases the current clock time by a full tick time.
    ElecClock& operator++()    { fTime += 1./fFrequency; return *this;}
    
    /// Increases the current clock time by a full tick time.
    ElecClock  operator++(int) {ElecClock tmp(*this); operator++(); return tmp;}
    
    /// Decreases the current clock time by a full tick time.
    ElecClock& operator--()    { fTime -= 1./fFrequency; return *this;}
    
    /// Decreases the current clock time by a full tick time.
    ElecClock  operator--(int) {ElecClock tmp(*this); operator--(); return tmp;}
    
    /// Increases the current clock time by the specified time in microseconds.
    ElecClock& operator+=(const double &rhs) { fTime += rhs; return *this;}
    /// Decreases the current clock time by the specified time in microseconds.
    ElecClock& operator-=(const double &rhs) { fTime -= rhs; return *this;}
    
    /// Increases the current clock time by the specified time in microseconds.
    ElecClock& operator+=(const float  &rhs) { fTime += (double)rhs; return *this;}
    /// Decreases the current clock time by the specified time in microseconds.
    ElecClock& operator-=(const float  &rhs) { fTime -= (double)rhs; return *this;}
    
    /// Increases the current clock time by the specified number of ticks.
    ElecClock& operator+=(const int &rhs) { fTime += Time(rhs); return *this;}
    /// Decreases the current clock time by the specified number of ticks.
    ElecClock& operator-=(const int &rhs) { fTime -= Time(rhs); return *this;}
    
    /// Increases the current clock time by the specified number of ticks.
    ElecClock& operator+=(const unsigned int &rhs) { fTime += Time((int)rhs); return *this;}
    /// Decreases the current clock time by the specified number of ticks.
    ElecClock& operator-=(const unsigned int &rhs) { fTime -= Time((int)rhs); return *this;}
    
    /// Increases the current clock time by the current time of another clock.
    ElecClock& operator+=(const ElecClock& rhs) { fTime += rhs.Time(); return *this;}
    
    /// Decreases the current clock time by the current time of another clock.
    ElecClock& operator-=(const ElecClock& rhs) { fTime -= rhs.Time(); return *this;}

    /// Returns a clock like this one, with its current time increased by
    /// the current time of `rhs`.
    inline ElecClock operator+(const ElecClock& rhs)
    { return ElecClock(fTime + rhs.Time(),fFramePeriod,fFrequency); }

    /// Returns a clock like this one, with its current time decreased by
    /// the current time of `rhs`.
    inline ElecClock operator-(const ElecClock& rhs)
    { return ElecClock(fTime - rhs.Time(),fFramePeriod,fFrequency); }
    
    /// @}
    
    /// Compares the current time of this clock with the one of `rhs`.
    inline bool operator<  (const ElecClock& rhs) const { return fTime <  rhs.Time(); }
    /// Compares the current time of this clock with the one of `rhs`.
    inline bool operator>  (const ElecClock& rhs) const { return fTime >  rhs.Time(); }
    /// Compares the current time of this clock with the one of `rhs`.
    inline bool operator<= (const ElecClock& rhs) const { return fTime <= rhs.Time(); }
    /// Compares the current time of this clock with the one of `rhs`.
    inline bool operator>= (const ElecClock& rhs) const { return fTime >= rhs.Time(); }

  }; // class ElecClock

}
#endif
/** @} */ // end of doxygen group 

