#ifndef CLOCK_H
#define CLOCK_H

#include <Adafruit_GFX.h>
#include <AceTMI.h> // SimpleTmi1637Interface
#include <AceSegment.h> // Tm1637Module

using ace_tmi::SimpleTmi1637Interface;
using ace_segment::Tm1637Module;

struct TimeChanges
{
  short digits[4];
};

class Clock
{
  private:
    long length;
    long startTime;
    long remaining;
    short lastDisplay;
    short lastSound;
    short strikes;
    long strike1time = -1;
    long strike2time = -1;
    short digits[4];
    short prevDigits[4];

  public:
    Clock(short seconds);
    void start();
    TimeChanges refresh();
    bool isExpired();
    void showColon(bool flag);
    void strike();
};
#endif