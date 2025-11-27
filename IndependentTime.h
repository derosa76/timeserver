#ifndef INDEPENDENT_TIME_H
#define INDEPENDENT_TIME_H

#include <Arduino.h>
#include <time.h>

class IndependentTime {
  private:
    unsigned long _epoch_seconds;
    unsigned long _last_update_millis;
    unsigned int _microseconds;
    
  public:
    IndependentTime() : _epoch_seconds(0), _last_update_millis(0), _microseconds(0) {}
    
    void setTime(unsigned long epoch_seconds, unsigned int microseconds = 0) {
      _epoch_seconds = epoch_seconds;
      _microseconds = microseconds;
      _last_update_millis = millis();
    }
    
    void setTime(int sec, int min, int hour, int day, int month, int year, unsigned int microseconds = 0) {
      struct tm t = {0};
      t.tm_year = year - 1900;
      t.tm_mon = month - 1;
      t.tm_mday = day;
      t.tm_hour = hour;
      t.tm_min = min;
      t.tm_sec = sec;
      _epoch_seconds = mktime(&t);
      _microseconds = microseconds;
      _last_update_millis = millis();
    }
    
    unsigned long getEpoch() {
      unsigned long elapsed_ms = millis() - _last_update_millis;
      return _epoch_seconds + (elapsed_ms / 1000);
    }
    
    unsigned int getMillis() {
      unsigned long original_ms = _microseconds / 1000;
      unsigned long elapsed_ms = millis() - _last_update_millis;
      return (unsigned int)((original_ms + elapsed_ms) % 1000);
    }
    
    struct tm getTimeStruct() {
      time_t t = (time_t)getEpoch();
      struct tm timeinfo;
      gmtime_r(&t, &timeinfo);
      return timeinfo;
    }
    
    int getYear()   { return getTimeStruct().tm_year + 1900; }
    int getMonth()  { return getTimeStruct().tm_mon; }
    int getDay()    { return getTimeStruct().tm_mday; }
    int getHour(bool format24 = true) { return getTimeStruct().tm_hour; }
    int getMinute() { return getTimeStruct().tm_min; }
    int getSecond() { return getTimeStruct().tm_sec; }
    
    String getTime(const char* format = "%H:%M:%S") {
      struct tm timeinfo = getTimeStruct();
      char buffer[64];
      strftime(buffer, sizeof(buffer), format, &timeinfo);
      return String(buffer);
    }
};

#endif