// +--------------------------------------------------------+
// | VDisplay voltage display and store                     |
// | Benigno Gobbo                                          |
// | (c) INFN Sezione di Trieste                            |
// | V1.1                                                   |
// | 08 October 2013                                        |
// +--------------------------------------------------------+

#include "VDisplay.h"
#include "Dvm.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <QPushButton>
#include <QInputDialog>
#include <QGridLayout>
#include <QDate>
#include "VNS.h"
#include <iostream>

// +-------------+
// | constructor |
// +-------------+

VDisplay::VDisplay( unsigned int ndigit, Dvm *dvm ) :
  _dvm( dvm ), _saveFlag(false), _pauseFlag(false), _variableName(QString::null) {

  _lcdnumber = new QLCDNumber( ndigit, this );

  _lcdnumber->setSmallDecimalPoint( false );
  _lcdnumber->setStyleSheet( "color:rgb(160,255,160);background-color:black" );
  _lcdnumber->setSegmentStyle( QLCDNumber::Flat );    
  _lcdnumber->setFixedSize( QSize( 180, 60 ) );

  _saveButton = new QPushButton( "Save", this );
  _saveButton->setStyleSheet( "color:rgb(0,0,0);background-color:rgb(100,255,100)" );
  _saveButton->setFixedSize( QSize( 70, 35 ) );
  connect( _saveButton, SIGNAL(clicked()), this, SLOT( setOut() ));

  _pauseButton = new QPushButton( "Pause", this );
  _pauseButton->setStyleSheet( "color:rgb(0,0,0);background-color:rgb(100,255,100)" );
  _pauseButton->setFixedSize( QSize( 70, 35 ) );
  connect( _pauseButton, SIGNAL(clicked()), this, SLOT( pause() ));

  QGridLayout* grid = new QGridLayout( this );
  grid->setSpacing( 0 );
  grid->addWidget( _lcdnumber, 0, 0 );
  grid->addWidget( _saveButton, 0, 1 );
  grid->addWidget( _pauseButton, 0, 2 );
} 


// +----------------+
// | displayVoltage |
// +----------------+

void VDisplay::displayVoltage( void ) {

  std::stringstream s;
  if( _dvm->adcOverflow() ) {
    s << "H" << std::endl;
  }
  else if( _dvm->adcUnderflow() ) {
    s << "L" << std::endl;
  }
  else {
    //s << std::fixed << std::setprecision(8) << _dvm->adcVoltage() * VPLOT::convert_voltage << std::endl;
    s << std::setw(VPLOT::v_precision+8) << std::fixed << std::setprecision(VPLOT::v_precision) << _dvm->adcVoltage() * VPLOT::convert_voltage << std::endl;
  }
  QString text = s.str().c_str();
  _lcdnumber->display( text );

}


// +--------+
// | setOut |
// +--------+

void VDisplay::setOut( void ) {

  bool ok = false;
  if( _variableName == QString::null ) {
    _variableName = QInputDialog::getText( this, tr("Enter variable"), tr("Variable name: "), QLineEdit::Normal,
					QString::null, &ok );
  }

  if( ok || _variableName != QString::null ) {
    if( _saveFlag ) {
      _saveFlag = false; 
      _saveButton->setText( "Save" );
      _saveButton->setStyleSheet( "color:rgb(0,0,0);background-color:rgb(100,255,100)" );
    }
    else {
      _saveFlag = true;
      _saveButton->setText( "Stop save" );
      _saveButton->setPalette( QPalette( Qt::yellow ) );
      _saveButton->setStyleSheet( "color:rgb(0,0,0);background-color:rgb(255,127,80)" );
    }
 
    if( !_saveDataFile.is_open() ) {
      _saveDataFile.open( VPLOT::file_name.c_str(), std::ios::app| std::ios::out );
    } 
  }

}

// +-------+
// | pause |
// +-------+

void VDisplay::pause( void ) {

  if( _pauseFlag ) {
    _pauseFlag = false; 
    _pauseButton->setText( "Pause" );
    _pauseButton->setStyleSheet( "color:rgb(0,0,0);background-color:rgb(100,255,100)" );
  }
  else {
    _pauseFlag = true;
    _pauseButton->setText( "Restore" );
    _pauseButton->setPalette( QPalette( Qt::yellow ) );
    _pauseButton->setStyleSheet( "color:rgb(0,0,0);background-color:rgb(255,127,80)" );
  }

}

// +-------+
// | goToPause |
// +-------+

void VDisplay::goToPause( void ) {

  _pauseFlag = true;
  _pauseButton->setText( "No Sign." );
  _pauseButton->setPalette( QPalette( Qt::yellow ) );
  _pauseButton->setStyleSheet( "color:rgb(0,0,0);background-color:rgb(255,0,0)" );

}

// +---------+
// | saveOut |
// +---------+

void VDisplay::saveOut( void ) {
  if( _saveFlag ) {
    _saveDataFile << _variableName.toStdString()
		 << " " << QDate::currentDate().toString().toStdString() 
		 << " " << QTime::currentTime().toString().toStdString() 
		 << " " << _dvm->adcVoltage() * VPLOT::convert_voltage
		 << " " << _dvm->adcUnderflow()
		 << " " << _dvm->adcOverflow()
		 << std::endl;
    }
}
