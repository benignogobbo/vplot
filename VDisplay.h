#ifndef VDISPLAY_H
#define VDISPLAY_H

// +--------------------------------------------------------+
// | VDisplay voltage display and store                     |
// | Benigno Gobbo                                          |
// | (c) INFN Sezione di Trieste                            |
// | V1.1                                                   |
// | 08 October 2013                                        |
// +--------------------------------------------------------+

#include <QLCDNumber>
#include <fstream>

class Dvm;
class QPushButton;

#endif // VDISPLAY_H

class VDisplay : public QWidget { 

Q_OBJECT
 
 public:

  VDisplay( unsigned int ndigis, Dvm* dvm );
 
   void displayVoltage( void );
   void saveOut( void );
   bool inPause() { return( _pauseFlag ); }
   void goToPause( void );

 private slots:

   void setOut( void );
   void pause( void );

 private:  
   
   Dvm* _dvm;
   QLCDNumber* _lcdnumber;
   QPushButton* _saveButton;
   QPushButton* _pauseButton;
   bool _saveFlag;
   bool _pauseFlag;
   QString _variableName;
   std::fstream _saveDataFile;
   
};
