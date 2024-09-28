#include "clock.h"
#include <PCM.h>
#include "sounds.h"

// Replace these with the PIN numbers of your dev board.
const uint8_t CLK_PIN = 2;
const uint8_t DIO_PIN = 3;
const uint8_t NUM_DIGITS = 4;

// Many TM1637 LED modules contain 10 nF capacitors on their DIO and CLK lines
// which are unreasonably high. This forces a 100 microsecond delay between
// bit transitions. If you remove those capacitors, you can set this as low as
// 1-5 micros.
const uint8_t DELAY_MICROS = 100;

using TmiInterface = SimpleTmi1637Interface;
TmiInterface tmiInterface(DIO_PIN, CLK_PIN, DELAY_MICROS);
Tm1637Module<TmiInterface, NUM_DIGITS> ledModule(tmiInterface);

// LED segment patterns.
const uint8_t NUM_PATTERNS = 10;
const uint8_t PATTERNS[NUM_PATTERNS] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111, // 9
};

Clock::Clock(short seconds)
{
  length = 1000 * (long)seconds;
  remaining = length;
  lastDisplay = -1;
  lastSound = -1;
  strikes = 0;

  tmiInterface.begin();
  ledModule.begin();

  ledModule.setPatternAt(0, PATTERNS[0]);
  ledModule.setPatternAt(1, PATTERNS[0]);
  ledModule.setPatternAt(2, PATTERNS[0]);
  ledModule.setPatternAt(3, PATTERNS[0]);

  ledModule.setBrightness(5);

  ledModule.flush();
  refresh();
}

void Clock::start()
{
  startTime = millis();
}

TimeChanges Clock::refresh()
{
  long realElapsed = millis() - startTime;
  long adjustedElapsed = realElapsed;

  if (strikes == 1)
  {
    adjustedElapsed = strike1time + ((realElapsed - strike1time) * 5 / 4);
  }
  else if (strikes == 2)
  {
    adjustedElapsed = strike1time + ((strike2time - strike1time) * 5 / 4) + ((realElapsed - strike2time) * 3 / 2);
  }

  remaining = length - adjustedElapsed;
  int minutes = remaining / 1000 / 60;
  int seconds = remaining / 1000 % 60;

  if (minutes > 0) {
    if (minutes < 10) {
      digits[0] = 0;
      digits[1] = minutes;
    } else {
      digits[0] = minutes / 10;
      digits[1] = minutes % 1;
    }

    if (seconds < 10) {
      digits[2] = 0;
      digits[3] = seconds;
    } else {
      digits[2] = seconds / 10;
      digits[3] = seconds % 10;
    }

    if (seconds != lastDisplay)
    {
      lastDisplay = seconds;

      tick();

      ledModule.setDecimalPointAt(1);
      ledModule.flush();

      ledModule.setPatternAt(0, PATTERNS[digits[0]]);
      ledModule.setPatternAt(1, PATTERNS[digits[1]]);
      ledModule.setPatternAt(2, PATTERNS[digits[2]]);
      ledModule.setPatternAt(3, PATTERNS[digits[3]]);

      ledModule.flush();
    }

  } else {
    if (seconds < 10) {
      digits[0] = 0;
      digits[1] = seconds;
    } else {
      digits[0] = seconds / 10;
      digits[1] = seconds % 10;
    }

    int thousandths = remaining % 1000;
    int hundredths = thousandths / 10;
    int tenths = hundredths / 10;

    if (hundredths < 10) {
      digits[2] = 0;
      digits[3] = hundredths;
    } else {
      digits[2] = tenths;
      digits[3] = hundredths % 10;
    }

    if (hundredths != lastDisplay)
    {
      lastDisplay = hundredths;

      if (lastSound != tenths && (tenths == 0 || tenths == 5))
      {
        lastSound = tenths;
        tick();
      }      

      ledModule.setDecimalPointAt(1);
      ledModule.flush();

      ledModule.setPatternAt(0, PATTERNS[digits[0]]);
      ledModule.setPatternAt(1, PATTERNS[digits[1]]);
      ledModule.setPatternAt(2, PATTERNS[digits[2]]);
      ledModule.setPatternAt(3, PATTERNS[digits[3]]);

      ledModule.flush();
    }

    // When timer is under 1 minute, the display moves the digits around for effect.
    // Here we remap digits back to original positions before they are sent to the other modules
    digits[3] = digits[1];
    digits[2] = digits[0];
    digits[1] = 0;
    digits[0] = 0;
  }

  TimeChanges tc;
  for (short i = 0; i < 4; i++)
  {
    if (digits[i] != prevDigits[i])
    {
      tc.digits[i] = digits[i];
    }
    else
    {
      tc.digits[i] = -1;
    }
    prevDigits[i] = digits[i];
  }
  return tc;
}

bool Clock::isExpired()
{
  return remaining <= 0;
}

void Clock::showColon(bool flag)
{
  ledModule.setDecimalPointAt(2);
}

void Clock::strike()
{
  strikes++;
  if (strikes == 1)
  {
    strike1time = millis();
  }
  else if (strikes == 2)
  {
    strike2time = millis();
  }
}
