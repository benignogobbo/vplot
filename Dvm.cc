// +--------------------------------------------------------+
// | DVM control class via Wixel serial interface.          |
// | Benigno Gobbo                                          |
// | (c) INFN Sezione di Trieste                            |
// | V1.3                                                   |
// | 15 January 2014                                        |
// +--------------------------------------------------------+

#include <cstdio>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>
#include <cstring>
#include "Dvm.h"
#include "VNS.h"
#include <QDateTime>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <iostream> // da scancellare 

// +-------------+
// | constructor |
// +-------------+
Dvm::Dvm( std::string device ) : _device(device), _fd(0), _adcsig(0), _adcmsb(0), _adcvoltage(0),
				 _adcunderflow(false), _adcoverflow(false), _adchistomin(0), _adchistomax(0)
{
}


// +------------------------------------------------------+
// | connect to device with correct settings              |
// +------------------------------------------------------+
int Dvm::connectToSerial( void ) {
  
  if( ( _fd = open( _device.c_str(), O_RDWR | O_NOCTTY ) ) < 0 ) {
    return( -1 );
  }
    
  tcgetattr( _fd, &_oldtio );
 
  _newtio = _oldtio;
  
  _newtio.c_cflag  = (B115200);
  _newtio.c_cflag |= CS8;
  _newtio.c_cflag |= CLOCAL;
  _newtio.c_cflag |= CREAD;

  _newtio.c_iflag = IGNPAR;

  _newtio.c_cc[VTIME]     = 0;  // inter-character timer not used
  _newtio.c_cc[VMIN]      = 4;  // blocking read until 4 character arrives

  tcflush( _fd, TCIFLUSH );
  tcsetattr( _fd, TCSANOW, &_newtio );  

  // Wait a second, otherwise it does not work...
  sleep(1);

  return( 0 );

}


// +------------------------------------------------------+
// | restore serial settings to old values                |
// +------------------------------------------------------+
int Dvm::restoreSerial( void ) {

  usleep(100000);

  tcsetattr( _fd, TCSANOW, &_oldtio ); /* restore old port settings */
  return( close( _fd ) );

}


// +------------------------------------------------------+
// | read Adc                                             |
// +------------------------------------------------------+
int Dvm::readAdc( void ) {

  int totRead = 0;

  tcflush( _fd, TCIFLUSH );
  _buff[0] = 'v';
  _buff[1] = VPLOT::nReads;
  _buff[2] = 0;

  int status =  write( _fd, _buff, 2 );

  if( status == 2 ) {

    fd_set set;
    int rv = 0;
    FD_ZERO( &set );
    FD_SET( _fd, &set );

    struct timeval timeout = VPLOT::timeout;
    rv = select( _fd+1, &set, NULL, NULL, &timeout );
    if( rv == -1 ) return( -999 );
    if( rv == 0 ) return( -999 );

    totRead = 0;
    for( int i = 0; i<VPLOT::nReads; i++ ) {
      status = read( _fd, _buff+4*i, 4 );
      if( status > 0 ) totRead+= status;
    } 
    if( totRead != 4*VPLOT::nReads ) return( totRead );
    
    int j = 0;
    double voltageSum = 0.0;
    do {
      _adc = 0;
      for( int i=0; i<4; i++ ) {
	_adc = (_adc<<8) + (unsigned char) _buff[i+4*j]; 
      }
      this->computeVoltage();
      voltageSum += _adcvoltage;
      j++;
    } while( (j < VPLOT::nReads) && (!_adcunderflow) && (!_adcoverflow) ); 
    if( _adcunderflow ) {
      _adcvoltage = _adchistomin;
    }
    else if( _adcoverflow ) {
      _adcvoltage = _adchistomax;
    }
    else {
      _adcvoltage = voltageSum / VPLOT::nReads;
    }
  }
  else { return( status ); }

  return( totRead );
}

// +------------------------------------------------------+
// | get Wixel Id                                         |
// +------------------------------------------------------+
std::string Dvm::getWixelId( void ) {

  FILE *fp;
  char buff[1024];
  
  fp = popen( "/bin/ls -l /dev/serial/by-id", "r" );
  if( fp == NULL ) {
    printf( "Failed to access serial device informations" );
    return( "0" );
  }

  while( fgets( buff, sizeof( buff ) - 1, fp ) != NULL ) {
    std::string s =  buff;
    int tag = s.find( "Pololu_Corporation_Wixel" );
    std::string id;
    if( tag > 0 ) {
      id = s.substr( tag+25, 11 ); 
      std::string device = "/dev/" + s.substr( tag+51, s.size()-(tag+51+1) ); 
      if( device == _device ) {
	pclose( fp );
	return( id );
      }
    }
  }

  pclose( fp );
  return( "0" );

}

// +------------------------------------------------------+
// | Compute Voltage                                      |
// +------------------------------------------------------+
void Dvm::computeVoltage( void ) {

  _adcsig = ( _adc & 0x20000000 ) >> 29;
  _adcmsb  = ( _adc & 0x10000000 ) >> 28;
  union{ unsigned int u; int i; } adcvalue;
  adcvalue.u = ( _adc & 0x1FFFFFFF ) >> 5;
  if( _adcmsb == 1 ) adcvalue.u = adcvalue.u | 0xFF000000;
    
  _adcoverflow = false;
  _adcunderflow = false;
  _adcvoltage = double( adcvalue.i) * VPLOT::reference_voltage / VPLOT::tenTo24;
  double intercept = 0.0, slope = 1.0;
  for( int i=0; i<7; i++ ) {
    if( this->getWixelId() == VPLOT::wixels[i].Id ) {
      intercept = VPLOT::wixels[i].Intercept;
      slope = VPLOT::wixels[i].Slope;
    }
  }

  _adcvoltage = _adcvoltage * slope + intercept;

  if( _adcsig == 1 && _adcmsb == 1 ) {
    _adcoverflow = true;
  }
  else if( _adcsig == 0 && _adcmsb == 0 ) {
    _adcunderflow = true;
  }

}
