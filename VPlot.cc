// +--------------------------------------------------------+
// | VPlot Qt voltage graphic plot                          |
// | Benigno Gobbo                                          |
// | (c) INFN Sezione di Trieste                            |
// | V1.1                                                   |
// | 09 October 2013                                        |
// +--------------------------------------------------------+

#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <map>
#include <QPainter>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
//#include <qwt_legend_item.h>
#include <qwt_plot_grid.h>
#include <QDateTime>
#include <QString>
#include "VPlot.h"
#include "Dvm.h"
#include "VDisplay.h"

double globYmin, globYmax;
std::fstream SaveDataFile;

// +--------------------------+
// | function getWixelChannel |
// +--------------------------+

static std::string getWixelChannel( std::string wixel ) {

  std::map< std::string, std::string  > wixelsCh;

  wixelsCh[ VPLOT::wixels[0].Id ] = VPLOT::wixels[0].Channel;
  wixelsCh[ VPLOT::wixels[1].Id ] = VPLOT::wixels[1].Channel;
  wixelsCh[ VPLOT::wixels[2].Id ] = VPLOT::wixels[2].Channel;
  wixelsCh[ VPLOT::wixels[3].Id ] = VPLOT::wixels[3].Channel;
  wixelsCh[ VPLOT::wixels[4].Id ] = VPLOT::wixels[4].Channel;
  wixelsCh[ VPLOT::wixels[5].Id ] = VPLOT::wixels[5].Channel;
  wixelsCh[ VPLOT::wixels[6].Id ] = VPLOT::wixels[6].Channel;
  wixelsCh[ VPLOT::wixels[7].Id ] = VPLOT::wixels[7].Channel;

  return wixelsCh[ wixel ];
}

// +---------------------+
// | class TimeScaleDraw |
// +---------------------+

class TimeScaleDraw : public QwtScaleDraw {
  
public:

  TimeScaleDraw(const QTime &base): _baseTime(base) {}

  virtual QwtText label(double v) const {
    QTime sinceTime = _baseTime.addSecs((int)v);
    return sinceTime.toString();
  }

private:

  QTime _baseTime;

};


// +------------------+
// | class Background |
// +------------------+

class Background : public QwtPlotItem {

public:

  Background() { setZ(0.0); }

  virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }

  virtual void draw(QPainter *painter,
		    const QwtScaleMap &, const QwtScaleMap &yMap,
		    const QRect &rect ) const {
    //QColor c(Qt::white);
    QColor c(40,40,40);
    QRect r = rect;
    
    for ( float u = globYmax; u > globYmin; u -= (globYmax-globYmin)/10 ) {
      r.setBottom(yMap.transform(u - 0.5));
      r.setTop(yMap.transform(u));
      painter->fillRect(r, c);
      //painter->setPen(Qt::white);
      //painter->drawRect(r);
      //c = c.dark(110);
      c = c.darker(90);
    } 
  }
};


// +--------------+
// | class VCurve |
// +--------------+

class VCurve : public QwtPlotCurve {

public:

  VCurve(const QString &title): QwtPlotCurve(title) {
    setRenderHint(QwtPlotItem::RenderAntialiased);
  }
  
  void setColor(const QColor &color) {
    QColor c = color;
    c.setAlpha(150);
    setPen(c);
  }
};

// +-------------+
// | constructor |
// +-------------+

VPlot::VPlot(Dvm* dvm, VDisplay* display, QWidget *parent) : QwtPlot(parent), _dvm(dvm), 
							     _display(display),
							     _dataCount(0), _first(true),
							     _ymin(-0.000001), _ymax(0.000001) {
  setAutoReplot(false);
  //setContentsMargins( 5, 5, 5, 5 );
  setMargin( 5 );
  QwtText title = QwtText( ( _dvm->getWixelId() + ", Ch: " + getWixelChannel( _dvm->getWixelId() ) ).c_str()  );
  title.setColor( QColor( 0, 255, 255 ) );
  setTitle( title );
  plotLayout()->setAlignCanvasToScales(true);

  //setAxisTitle(QwtPlot::xBottom, " Time [h:m:s]");
  setAxisTitle(QwtPlot::xBottom, "Counts");
  //QTime t = QTime::currentTime();
  //setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(t));
  //setAxisScale(QwtPlot::xBottom, 0, VPLOT::history);
  setAxisScale(QwtPlot::xBottom, -VPLOT::history, 0 );
  //setAxisLabelRotation(QwtPlot::xBottom, -50.0);
  //setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
  
  QwtScaleWidget *scaleWidget = axisWidget(QwtPlot::xBottom);
  const int fmh = QFontMetrics(scaleWidget->font()).height();
  scaleWidget->setMinBorderDist(0, fmh / 2);
  
  setAxisTitle(QwtPlot::yLeft, VPLOT::y_axis_title.c_str() );
  setAxisScale(QwtPlot::yLeft, _ymin, _ymax);

  //Background *bg = new Background();
  //bg->attach(this);

  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->enableYMin(true);
  //grid->setMajorPen(QPen(Qt::yellow, 0, Qt::DotLine));
  grid->setMajPen(QPen(Qt::yellow, 0, Qt::DotLine));
  //grid->setMinorPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->attach(this);
  
  VCurve *curve;

  QString device = (_dvm->getWixelId()).c_str();
  curve = new VCurve( device );
  //curve->setColor(Qt::red);
  //curve->setColor(QColor(20,255,255));
  QPen pen;
  pen.setWidth( 2.0 );
  pen.setColor( QColor(20,255,255) );
  curve->setPen( pen );
  curve->attach(this);
  _data.curve = curve;

  replot();

  for ( int i = 0; i < VPLOT::history; i++ ) _counts[VPLOT::history - 1 - i] = i-VPLOT::history;

  (void)startTimer(VPLOT::timer); 
  
}


// +------------+
// | timerEvent |
// +------------+

void VPlot::timerEvent(QTimerEvent *) {

  if( _display->inPause() ) return;

  for ( int i = _dataCount; i > 0; i-- ) {
    if ( i < VPLOT::history ) _data.voltage[i] = _data.voltage[i-1];
  }
  

  int status = 0;

  do {
    status = _dvm->readAdc();
  } while( status <= 0 && status != -999 );

  if( status == -999 ) _display->goToPause();

  if( status != 4*VPLOT::nReads ) return;
  
  _data.voltage[0] = _dvm->adcVoltage() * VPLOT::convert_voltage;
  _display->displayVoltage();
  _display->saveOut();

  _ymin = _data.voltage[0];
  for( int j=1; j<std::min(_dataCount,VPLOT::history); j++ ) {
    _ymin = fmin( _ymin, _data.voltage[j] );
  }
  _ymax = _data.voltage[0];
  for( int j=1; j<std::min(_dataCount,VPLOT::history); j++ ) {
    _ymax = fmax( _ymax, _data.voltage[j] );
  }
    
  globYmin = _ymin;
  globYmax = _ymax;
  _dvm->setAdcMinMax( _ymin, _ymax );
  
  if ( _dataCount < VPLOT::history ) _dataCount++;
  
  for ( int j = 0; j < VPLOT::history; j++ ) _counts[j]++;
  

  setAxisScale(QwtPlot::xBottom, _counts[VPLOT::history - 1], _counts[0]);
  
  //_data.curve->setRawSamples( _counts, _data.voltage, _dataCount);
  _data.curve->setRawData( _counts, _data.voltage, _dataCount);
  
  setAxisScale(QwtPlot::yLeft, _ymin, _ymax);

  replot();
}

