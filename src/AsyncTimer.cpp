#include "AsyncTimer.h"

void AsyncTimer::setup() { srand(millis()); }

unsigned short AsyncTimer::m_newTimerInfo(void (*callback)(unsigned short),
                                          unsigned int ms, bool indefinite) {
  if (m_availableIndicesLength == 0 || m_arrayLength == m_maxArrayLength) {
    return 0;
  }
  unsigned short id = rand() + 1;
  m_availableIndicesLength--;
  unsigned short availableIndex = m_availableIndices[m_availableIndicesLength];
  m_callsArray[availableIndex].id = id;
  m_callsArray[availableIndex].callback.cbId = callback;
  m_callsArray[availableIndex].callback.cbNoId = nullptr;
  m_callsArray[availableIndex].delayByMs = ms;
  m_callsArray[availableIndex].timestamp = millis();
  m_callsArray[availableIndex].indefinite = indefinite;
  m_callsArray[availableIndex].active = true;
  m_arrayLength++;

  return id;
}

unsigned short AsyncTimer::m_newTimerInfo(void (*callback)(), unsigned int ms,
                                          bool indefinite) {
  if (m_availableIndicesLength == 0 || m_arrayLength == m_maxArrayLength) {
    return 0;
  }
  unsigned short id = rand() + 1;
  m_availableIndicesLength--;
  unsigned short availableIndex = m_availableIndices[m_availableIndicesLength];
  m_callsArray[availableIndex].id = id;
  m_callsArray[availableIndex].callback.cbNoId = callback;
  m_callsArray[availableIndex].callback.cbId = nullptr;
  m_callsArray[availableIndex].delayByMs = ms;
  m_callsArray[availableIndex].timestamp = millis();
  m_callsArray[availableIndex].indefinite = indefinite;
  m_callsArray[availableIndex].active = true;
  m_arrayLength++;

  return id;
}

unsigned short AsyncTimer::setTimeout(void (*callback)(), unsigned int ms) {
  return m_newTimerInfo(callback, ms, false);
}

unsigned short AsyncTimer::setTimeout(void (*callback)(unsigned short),
                                      unsigned int ms) {
  return m_newTimerInfo(callback, ms, false);
}

unsigned short AsyncTimer::setInterval(void (*callback)(), unsigned int ms) {
  return m_newTimerInfo(callback, ms, true);
}

unsigned short AsyncTimer::setInterval(void (*callback)(unsigned short),
                                       unsigned int ms) {
  return m_newTimerInfo(callback, ms, true);
}

void AsyncTimer::changeDelay(unsigned short id, unsigned int ms) {
  for (short i = 0; i < m_maxArrayLength; i++)
    if (m_callsArray[i].id == id)
      m_callsArray[i].delayByMs = ms;
}

void AsyncTimer::delay(unsigned short id, unsigned int ms) {
  for (short i = 0; i < m_maxArrayLength; i++)
    if (m_callsArray[i].id == id)
      m_callsArray[i].timestamp += ms;
}

void AsyncTimer::reset(unsigned short id) {
  for (short i = 0; i < m_maxArrayLength; i++)
    if (m_callsArray[i].id == id)
      m_callsArray[i].timestamp = millis();
}

void AsyncTimer::cancel(unsigned short id) {
  for (short i = 0; i < m_maxArrayLength; i++) {
    if (m_callsArray[i].id == id) {
      m_callsArray[i].active = false;
      m_arrayLength--;
      m_availableIndices[m_availableIndicesLength] = i;
      m_availableIndicesLength++;
    }
  }
}

void AsyncTimer::handle() {
  if (m_arrayLength == 0)
    return;

  for (short i = 0; i < m_maxArrayLength; i++) {
    unsigned long timestamp = millis();
    if (!m_callsArray[i].active || m_callsArray[i].timestamp > timestamp)
      continue;
    if (timestamp - m_callsArray[i].timestamp >= m_callsArray[i].delayByMs) {
      if (m_callsArray[i].indefinite) {
        m_callsArray[i].timestamp = timestamp;
        if (m_callsArray[i].callback.cbNoId == nullptr)
          m_callsArray[i].callback.cbId(m_callsArray[i].id);
        else
          m_callsArray[i].callback.cbNoId();
      } else {
        if (m_callsArray[i].callback.cbNoId == nullptr)
          m_callsArray[i].callback.cbId(m_callsArray[i].id);
        else
          m_callsArray[i].callback.cbNoId();
        m_callsArray[i].active = false;
        m_callsArray[i].callback.cbId = nullptr;
        m_callsArray[i].callback.cbNoId = nullptr;
        m_arrayLength--;
        m_availableIndices[m_availableIndicesLength] = i;
        m_availableIndicesLength++;
      }
    }
  }
}