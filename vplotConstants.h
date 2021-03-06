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

// Vref from LTC6655-1.25 
static const double VPLOT_reference_voltage = 1.25;

// This is just 10^24
static const int VPLOT_tenTo24 = 16777216;

// Number of entries to be used in historical histogram 
static const int VPLOT_history = 50;

// Voltage Conversion constant and histogram Y axis title
static const double VPLOT_convert_voltage = 100000.; // assume amplifier impedence of 10 Mohm 
static const std::string VPLOT_y_axis_title = "Current [pA]";
//static const double VPLOT_convert_voltage = 1000.; // millivolt
//static const std::string VPLOT_y_axis_title = "Voltage [mV]";

// decimal precision of LCD display
static const int  VPLOT_v_precision = 4;

// Timer (to be used between measurements)
//static const int VPLOT_timer = 1000; // ms
//static const int VPLOT_timer = 50; // ms
static int VPLOT_timer = 10; // ms

// (Eventual) output file name.
static std::string VPLOT_file_name = "DVM_save_data.dat";

// Timeout if Wixel does not respond (in seconds)
static struct timeval VPLOT_timeout = { 5, 0 };

// Wixels data: internal address, radio channel, intercept and slope from voltage calibration
typedef struct wixel { std::string Id; std::string Channel; double Intercept; double Slope; } wixel;
//
//static wixel VPLOT_wixels[] = {{ "08-16-39-E3", "20",  -0.0554011, 0.998019 },
//			 { "CC-77-51-BE", "40",   0.82809,   0.997216 }, 
//			 { "74-C6-AB-89", "60",  -0.54026,   0.981588 },
//			 { "60-B3-36-A1", "80",  -2.07683,   1.02748  },
//			 { "BA-1B-70-69", "100", -7.86956,   1.06608  }, 
//			 { "90-8D-05-7A", "120", -0.213785,  0.995827 },
//			 { "51-FC-D6-C2", "140", -0.816662,  1.002    },    
//			 { "F8-50-BF-18", "160",  0.0,       1.0      }};
static wixel VPLOT_wixels[] = {{ "9C-81-9B-FA", "20",   0.0, 1.0 },
			 { "75-AA-BB-25", "40",   0.0, 1.0 }, 
			 { "2D-4F-54-DE", "60",   0.0, 1.0 },
			 { "BD-43-CB-A0", "80",   0.0, 1.0 },
			 { "19-8F-0F-9E", "100",  0.0, 1.0 }, 
			 { "E6-3E-68-FA", "120",  0.0, 1.0 },
			 { "0F-BA-CA-04", "140",  0.0, 1.0 },    
			 { "F8-50-BF-18", "160",  0.0, 1.0 }};

// Number of voltage measurements to mediate.
static unsigned char VPLOT_nReads = 1;

#endif // CONS_H
