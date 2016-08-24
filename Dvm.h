#ifndef DVM_H
#define DVM_H

// +--------------------------------------------------------+
// | DVM control via Wixel serial interface.                |
// | Benigno Gobbo                                          |
// | (c) INFN Sezione di Trieste                            |
// | V1.2                                                   |
// | 10 October 2013                                        |
// +--------------------------------------------------------+

#include <string>
#include <vector>
#include <fstream>
#include <termios.h>

class Dvm {
  
 public:

  Dvm( std::string device );

  ~Dvm() {}


  int connectToSerial( void );

  int restoreSerial( void ); 

  int readAdc( void );

  inline double adcVoltage( void )   { return( _adcvoltage ); }
  inline bool   adcOverflow( void )  { return( _adcoverflow ); }
  inline bool   adcUnderflow( void ) { return( _adcunderflow ); }
  inline void   setAdcMinMax( double min, double max ) { _adchistomin = min; _adchistomax = max; } 
  
  std::string getWixelId( void );

 private:
  
  std::string       _device;
  int               _fd;
  struct termios    _oldtio;
  struct termios    _newtio;
  char              _buff[512];
  unsigned int      _adc;
  int               _adcsig;
  int               _adcmsb;
  double            _adcvoltage;
  bool              _adcunderflow;
  bool              _adcoverflow;
  double            _adchistomin;
  double            _adchistomax;
  
  void              computeVoltage( void );
};

#endif // DVM_H
