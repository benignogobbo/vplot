// +--------------------------------------------------------+
// | Digital Voltmeter Control Program                      |
// | Benigno Gobbo                                          |
// | (c) INFN Sezione di Trieste                            |
// | V1.1                                                   |
// | 21 January 2014                                        |
// +--------------------------------------------------------+

#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include "Dvm.h"
#include "VPlot.h"
#include "VDisplay.h"
#include "VNS.h"

// +---------------------+
// | class MainWindow |
// +---------------------+

class MainWindow : public QMainWindow {

public:
  MainWindow( std::vector<Dvm*> dvms, QWidget * = NULL );

public slots:
  void endRun( void );

private:
  QWidget* _vBox;
};

MainWindow::MainWindow( std::vector<Dvm*> dvms, QWidget *parent ) : QMainWindow(parent) {

  _vBox = new QWidget();
  _vBox->setWindowTitle("D.V.M.");
  setCentralWidget( _vBox );
  _vBox->setStyleSheet( "color:rgb(160,255,160);background-color:rgb(0,0,0)" );
      

  QToolBar* toolBar = new QToolBar(this);

  QToolButton* quitButton = new QToolButton( toolBar );

  quitButton->setText( "Quit" );
  quitButton->setStyleSheet( "color:white;background:red" );
  toolBar->addWidget( quitButton );

  addToolBar( toolBar );

  connect( quitButton, SIGNAL(clicked( void )), qApp, SLOT( quit( void ) ) );

  std::vector<VPlot*> plots;
  std::vector<VDisplay*> displays;

  for( unsigned i = 0; i < dvms.size(); i++ ) { 
    VDisplay *display = new VDisplay( VPLOT::v_precision+8, dvms[i] );
    displays.push_back( display );
    VPlot *plot = new VPlot(dvms[i], display, _vBox );
    plots.push_back( plot );
  }

  int nplots = plots.size();

  _vBox->resize( 350*( (nplots>2) ? VPLOT::histPerRow : nplots ), 50+300*((nplots-1)/VPLOT::histPerRow+1) );

  QGridLayout *layout = new QGridLayout( _vBox );
  layout->setSpacing(2);

  for( int i=0; i<nplots; i++ ) {
    layout->addWidget(plots[i], 2*(i/VPLOT::histPerRow), i%VPLOT::histPerRow );
    layout->addWidget(displays[i], 2*(i/VPLOT::histPerRow)+1, i%VPLOT::histPerRow );
  }

}


// +--------------+
// | class getdir |
// +--------------+

int getdir( std::string dir, std::vector<std::string> &files )  {

  DIR *dp;
  struct dirent *dirp;
  if(( dp  = opendir( dir.c_str() )) == NULL ) {
    std::cout << "Error(" << errno << ") opening " << dir << std::endl;
    return errno;
  }

  while(( dirp = readdir(dp) ) != NULL ) {
    files.push_back( std::string( dirp->d_name ));
  }
  closedir( dp );
  return 0;
}

// +------------------------------------------------------+
// | get Wixel Id                                         |
// +------------------------------------------------------+
std::string getWixelId( std::string device ) {
  
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
      std::string thisdevice = "/dev/" + s.substr( tag+51, s.size()-(tag+51+1) ); 
      if( thisdevice == device ) {
	pclose( fp );
	return( id );
      }
    }
  }

  pclose( fp );
  return( "0" );
  
}

// +-------------+
// |    usage    |
// +-------------+

void usage( char* cp ) {

  std::cout << 
"\n\
usage: " << cp << " [-h] [-c channel1 channel2 ...] [-n reads]\n\
                    [-s timeout seconds] [-u timeout microseconds]\n\
                    [-t milliseconds] [-o filename]\n\n\
     h             : print this help message \n\
     c ch1 ... chn : select wixels by channel\n\
     n reads       : number of adc reads per measurement\n\
     o filename    : output file name\n\
     r hist./row   : histograms per row\n\
     s sec         : timeout seconds\n\
     u usec        : timeout microseconds\n\
     t msec        : time between measurements (milliseconds)\n\
\n";
}

// +------------+
// |    main    |
// +------------+

int main(int argc, char **argv) {

  // Manage arguments...

  int         rc;
  char        *cp;
  std::string ch = "";

  if(( cp = (char*) strrchr( argv[0], '/' )) != NULL ) {
    ++cp;
  }
  else {
    cp = argv[0];
  }

  if( argc > 1 ) {
    while(( rc = getopt( argc, argv, "hc:n:o:r:s:t:u:" )) != EOF ) {
      switch( rc ) {
      case 'h':
        usage(cp);
        return(0);
        break;
      case 'c':
        ch = ch + " " + optarg;
        break;
      case 'n':
        VPLOT::nReads = atoi( optarg );
        break;
      case 'o':
        VPLOT::file_name = optarg;
        break;
      case 'r':
        VPLOT::histPerRow = atoi( optarg );
      case 's':
        VPLOT::timeout.tv_sec = atoi( optarg );
        break;
      case 'u':
        VPLOT::timeout.tv_usec = atoi( optarg );
        break;
      case 't':
        VPLOT::timer = atoi( optarg );
        break;
      case '?':
        return(-1);
        break;
      default:
        break;
      }
    }  
  }
  //else {
  //  usage( cp );
  //  return (-1);
  //}
  
  //if( optind < argc ) {
  //  var = argv[optind];
  //}
  //else {
  //  std::cerr << "..." << std::endl;
  //  return (-1);
  //}

  std::cout << "Preset values" << std::endl
	    << "-------------" << std::endl
            << "Historical histogram width: " << VPLOT::history << std::endl
	    << "Volt conversion factor: " << VPLOT::convert_voltage << std::endl
	    << "Decimal precision of LCDs: " << VPLOT::v_precision << std::endl
	    << "Histograms per row: " << VPLOT::histPerRow << std::endl 
	    << "Time between measurements (ms): " << VPLOT::timer << std::endl
	    << "Output file name: " << VPLOT::file_name << std::endl
	    << "Timeout (s, us): " << VPLOT::timeout.tv_sec 
	    << ", " << VPLOT::timeout.tv_usec << std::endl
	    << "Voltage reads to mediate: " << int( VPLOT::nReads ) << std::endl
	    << "-------------" << std::endl;

  std::vector<Dvm*> dvms;
  std::string dir = "/dev";
  std::vector<std::string> files;
  getdir( dir, files );
  std::string s = "ttyACM";
  for( unsigned int i = 0; i<files.size(); i++ ) {
    if( files[i].substr(0,6) == s ) {
      std::string device = dir+"/"+files[i]; 
      if( ch.empty() ) {
	Dvm* dvm = new Dvm( device );
	dvm->connectToSerial();
	if( dvm->getWixelId() != "0" ) {
	  std::cout << "Found a wixel on device  " << device << " with id " << dvm->getWixelId() << std::endl; 
	  dvms.push_back( dvm );
	}
      }
      else {
	std::stringstream ss;
	ss << ch;
	std::string thech;
	do{
	  ss >> thech;
	  std::string id = getWixelId( device );
	  if( id != "0" ) {
	    for( int i=0; i<7; i++ ) {
	      if( id == VPLOT::wixels[i].Id ) {
		if( thech == VPLOT::wixels[i].Channel ) {
		  Dvm* dvm = new Dvm( device );
		  dvm->connectToSerial();
		  std::cout << "Found a wixel on device  " << device << " with id " << dvm->getWixelId() << std::endl; 
		  dvms.push_back( dvm );
		}
	      }
	    }
	  }
	} while( ! ss.eof() );
      }
    }
  }

  QApplication a(argc, argv); 
    
  MainWindow mainWindow( dvms );
  mainWindow.show();
  
  return a.exec();  
}

