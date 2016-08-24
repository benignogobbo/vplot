#ifndef CONS_H
#define CONS_H

// +--------------------------------------------------------+
// | vplot constants                       .                |
// | Benigno Gobbo                                          |
// | (c) INFN Sezione di Trieste                            |
// | V1.3                                                   |
// | 05 January 2014                                        |
// +--------------------------------------------------------+

#include <map>
#include <string>
#include <sys/time.h>

namespace VPLOT {

  // Number of entries to be used in historical histogram 
  const int history = 50;

  // Vref from LTC6655-1.25 
  extern const double reference_voltage;

  // This is just 10^24
  extern const int tenTo24;

  // Voltage Conversion constant and histogram Y axis title
  extern const double convert_voltage;
  extern const std::string y_axis_title;

  // decimal precision of LCD display
  extern const int  v_precision;

  // histograms per row
  extern int histPerRow;

  // Timer (to be used between measurements)
  extern int timer;

  // (Eventual) output file name.
  extern std::string file_name;

  // Timeout if Wixel does not respond (in seconds)
  extern struct timeval timeout;

  // Wixels data: internal address, radio channel, intercept and slope from voltage calibration
  typedef struct wixel { std::string Id; std::string Channel; double Intercept; double Slope; } wixel;
  extern wixel wixels[8];

  // Number of voltage measurements to mediate.
  extern unsigned char nReads;

}

#endif // CONS_H
