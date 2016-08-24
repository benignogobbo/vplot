#ifndef VPLOT_h
#define VPLOT_h

// +--------------------------------------------------------+
// | VPlot Qt voltage graphic plot                          |
// | Benigno Gobbo                                          |
// | (c) INFN Sezione di Trieste                            |
// | V1.1                                                   |
// | 09 October 2013                                        |
// +--------------------------------------------------------+

#include <qwt_plot.h>
#include <QPushButton>
#include <fstream>
#include "VNS.h"

class QwtPlotCurve;
class Dvm;
class VDisplay;

class VPlot : public QwtPlot { Q_OBJECT

public:

  VPlot(Dvm* dvm, VDisplay* display, QWidget* parent = 0 );

  const QwtPlotCurve *vCurve( void ) const { return _data.curve; }

protected:
  void timerEvent(QTimerEvent *e);

private:

  Dvm *_dvm;
  VDisplay* _display;

  struct {
    QwtPlotCurve *curve;
    double voltage[VPLOT::history];
  } _data;
  
  double _counts[VPLOT::history];
  int _dataCount;

  bool _first;

  double _ymin;
  double _ymax;

};

#endif // VPLOT_h
