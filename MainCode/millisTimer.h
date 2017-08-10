/*
   Simple Counter class
*/

class MillisTimer {

  public:
    MillisTimer();
    int getTime();
    void resetTo(int resetValue);

  private:
    void countDown();

    int m_resetTime;
    int m_cntValue;
};

/*
   Simple Counter class
*/
//#include <MillisTimer.h>
#include <Arduino.h>

MillisTimer::MillisTimer()
  : m_resetTime(0), m_cntValue(0)
{
}

void MillisTimer::resetTo(int resetValue) {
  m_cntValue = resetValue;

  // get current time
#ifdef USE_SAVEPOWER
  m_resetTime = Narcoleptic.millis() + millis();
#else
  m_resetTime = millis();
#endif
}

int MillisTimer::getTime() {

  countDown();

  return m_cntValue;
}

void MillisTimer::countDown() {
  if (m_cntValue == 0)
    return;

  // get current time
#ifdef USE_SAVEPOWER
  long timeNow = Narcoleptic.millis() + millis();
#else
  long timeNow = millis();
#endif

  // diff passed time since last check
  m_cntValue -= (timeNow - m_resetTime);

  // set time for next check
  m_resetTime = timeNow;

  if (m_cntValue < 0)
    m_cntValue = 0;
}
