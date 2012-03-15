/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 
/*LICENSE_END*/


#include "TimeCourseDialog.h"
#include "ui_TimeCourseDialog.h"

//qwt includes
#include <qapplication.h>
#include "qlayout.h"
#include "qwt_plot.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_curve.h"
#include "qwt_legend.h"
#include "qwt_series_data.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_magnifier.h"
#include "qwt_text.h"
#include "qwt_math.h"
#include "math.h"
#include "PlotPanner.h"

using namespace caret;

TimeCourseDialog::TimeCourseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimeCourseDialog)
{
    ui->setupUi(this);
#ifdef CARET_OS_WINDOWS
    this->setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
#endif
    this->plot = new caret::PlotTC();
    isEnabled = true;

    ui->verticalLayout_4->setContentsMargins(0,0,0,0);
    ui->verticalLayout_4->insertWidget(0,plot,100);    
}



TimeCourseDialog::~TimeCourseDialog()
{
    delete ui;
}

void TimeCourseDialog::addTimeLine(TimeLine &tl)
{    
    tlV.push_back(tl);
    //readjust x values to account for timestep
    for(int j = 0;j<tlV.last().x.size();j++)
    {
        tlV.last().x[j] = startOffset + tlV.last().timeStep*(double)j;
    }
}

void TimeCourseDialog::addTimeLines(QList <TimeLine> &tlVIn)
{
    for(int i = 0;i<tlVIn.size();i++)
    {
        this->addTimeLine(tlV[i]);
    }
}

void TimeCourseDialog::updateDialog(bool forceUpdate)
{
    if(tlV.isEmpty() && !forceUpdate) return;
    if(!this->isEnabled) return;
    if(!(this->filename.length())) 
    {   
        this->filename = tlV[0].filename;
        AString temp = filename;
        if(temp.length() > 80) temp = "..." + temp.right(80);
        this->setWindowTitle("Time Course " + AString::number(tlV[0].clmID) + AString(" - ") + temp);
    }
    plot->detachItems();
    plot->populate(tlV);    
    this->setVisible(true);
    this->show();
    this->activateWindow();
    plot->update();
    plot->replot();
    plot->setFocus();
    this->setAttribute(Qt::WA_NoMousePropagation,true);
}

void TimeCourseDialog::setAnimationStartTime(const double &time)
{
    startOffset = time;
    if(tlV.isEmpty()) return;
    
    for(int i =0;i<tlV.size();i++)
    {        
        for(int j = 0;j<tlV[i].x.size();j++)
        {
            tlV[i].x[j] = startOffset + tlV[i].timeStep*(double)j;
        }
    }
    plot->detachItems();
    plot->populate(tlV);
    plot->update();
    plot->replot();    
}




void TimeCourseDialog::on_TDClose_clicked()
{
    this->hide();
}

void TimeCourseDialog::on_TDClearChart_clicked()
{
    this->plot->clear(tlV);
    tlV.clear();
    this->updateDialog(true);
}

void TimeCourseDialog::on_TDMinTime_valueChanged(double arg1)
{

}

void TimeCourseDialog::on_TDMaxTime_valueChanged(double arg1)
{

}

void TimeCourseDialog::on_TDMinActivity_valueChanged(double arg1)
{

}

void TimeCourseDialog::on_TDMaxActivity_valueChanged(double arg1)
{

}

void TimeCourseDialog::on_TDShowAverage_toggled(bool checked)
{
    this->plot->setDisplayAverage(checked);
    this->updateDialog();
}

void TimeCourseDialog::setTimeSeriesGraphEnabled(bool enabled)
{
    this->isEnabled = enabled;
}







PlotTC::PlotTC(QWidget *parent):
    QwtPlot( parent )
{
    ctrlKeyDown = false;
    shiftKeyDown = false;
    max = 5;
    lineWidth = 1;
    autoScaleEnabled = true;
    // panning with the left mouse button
    (void) new PlotPanner( canvas() );

    // zoom in/out with the wheel
    magnifier = new PlotMagnifier( canvas() );    
    magnifier->setAxisEnabled(QwtPlot::yLeft,false);
    magnifier->setAxisEnabled(QwtPlot::yRight,false);    

    setAutoFillBackground( true );
    setPalette( QPalette( QColor( 165, 193, 228 ) ) );    

    setTitle("Time Course");
    insertLegend(new QwtLegend(), QwtPlot::RightLegend);

    // canvas
    canvas()->setLineWidth( 1 );
    canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas()->setBorderRadius( 1 );//15

    QPalette canvasPalette( Qt::white );
    canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
    canvas()->setPalette( canvasPalette );
    this->displayAverage = false;
}

void PlotTC::clear(QList<TimeLine> &tlV)
{
    for(int i =0;i<tlV.size();)
    {
        colors.removeColor(tlV.at(0).colorID);
        tlV.pop_front();
    }
}

void PlotTC::populate(QList<TimeLine> &tlV)
{
    if(tlV.isEmpty()) return;
    //Delete oldest time lines first if we are displaying more than max number of time lines
    if(tlV.size()>max)
    {
        for(int i =0;i<(tlV.size()-max);)
        {
            colors.removeColor(tlV.at(0).colorID);
            tlV.pop_front();
        }
    }
    // Make sure all timelines have a valid color assigned to them
    for(int i=0;i<tlV.size();i++)
    {
        if(tlV.at(i).colorID == -1)
        {
            tlV[i].colorID = colors.getNewColor();
        }
    }
    // Insert new curves
    //setAxisScale(xBottom,0,x.size()-1);
    //setAxisScale(yLeft,0,y.size()-1);

    //Plot Time Lines
    if(this->getAutoScale())
    {
        setAxisAutoScale(QwtPlot::xBottom);
        setAxisAutoScale(QwtPlot::yLeft);
    }
    else
    {
        setAxisAutoScale(QwtPlot::xBottom,false);
        setAxisAutoScale(QwtPlot::yLeft,false);
    }
    
    for(int i = 0;i<tlV.size();i++)
    {
        drawTimeLine(tlV[i]);
    }
    if(this->displayAverage && tlV.size()) calculateAndDisplayAverage(tlV);
    
}

void PlotTC::drawTimeLine(TimeLine &tl, QPen *pen)
{
    QPen *myPen;
    if(pen == NULL)

    {
        myPen = new QPen(colors.getColor(tl.colorID));
        myPen->setWidth(this->lineWidth);
    }
    else
    {
        myPen = pen;
    }
    QwtPlotCurve *tc = new QwtPlotCurve();
    tc->setRenderHint(QwtPlotItem::RenderAntialiased);
    //tc->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
    tc->setPen(*myPen);

    tc->setSamples(tl.x,tl.y);
    tc->attach(this);
    if(pen == NULL) delete myPen;
}


void PlotTC::calculateAndDisplayAverage(QList<TimeLine> &tlV)
{
    int64_t xmax =0;
    for(int i =0;i<tlV.size();i++)
    {
        xmax = std::max<int64_t>(tlV[i].x.size(),xmax);
    }
    averageTimeLine.x.clear();
    averageTimeLine.y.clear();
    for(int i =0;i<xmax;i++)
    {
        averageTimeLine.x.push_back(0.0);
        averageTimeLine.y.push_back(0.0);
        uint64_t divisor = 0.0;

        for(int j = 0;j<tlV.size();j++)
        {
            if(i>=tlV[j].x.size()) continue;
            divisor++;
            averageTimeLine.x[i] += tlV[j].x[i];
            averageTimeLine.y[i] += tlV[j].y[i];

        }
        averageTimeLine.x[i] /= divisor;
        averageTimeLine.y[i] /= divisor;
    }
    QPen pen(colors.getColor(averageTimeLine.colorID));
    pen.setDashOffset(4);
    pen.setWidth(2);
    drawTimeLine(averageTimeLine,&pen);
}

void PlotTC::setDisplayAverage(bool checked)
{
    displayAverage = checked;
    if(checked)
    {
        averageTimeLine.colorID = colors.getNewColor();
    }
    else
    {
        colors.removeColor(averageTimeLine.colorID);
    }
}

void PlotTC::resizeEvent( QResizeEvent *event )
{
    QwtPlot::resizeEvent( event );
}

void TimeCourseDialog::on_TDKeepLast_valueChanged(int arg1)
{
    this->plot->setMaxTimeLines(arg1);
    this->updateDialog();
}

void TimeCourseDialog::on_zoomXCheckBox_toggled(bool checked)
{
    this->plot->magnifier->setAxisEnabled(QwtPlot::xBottom,checked);
    this->plot->magnifier->setAxisEnabled(QwtPlot::xTop,checked);
}

void TimeCourseDialog::on_zoomYCheckBox_toggled(bool checked)
{
    this->plot->magnifier->setAxisEnabled(QwtPlot::yLeft,checked);
    this->plot->magnifier->setAxisEnabled(QwtPlot::yRight,checked);
}

void TimeCourseDialog::on_lineWidthSpinBox_valueChanged(int arg1)
{
    this->plot->setTimeLineWidth(arg1);
    this->updateDialog(true);
}

void PlotTC::setTimeLineWidth(int width)
{
    this->lineWidth = width;    
}

void PlotTC::setAutoScaleEnabled(bool checked)
{
    this->autoScaleEnabled = checked;
}

bool PlotTC::getAutoScale()
{
    if(autoScaleOnce || autoScaleEnabled)
    {
        autoScaleOnce = false;
        return true;
    }
    else
        return false;
}

void PlotTC::resetView()
{
    autoScaleOnce = true;
}

void TimeCourseDialog::on_autoFitTimeLinesCheckBox_toggled(bool checked)
{
    this->plot->setAutoScaleEnabled(checked);
    this->updateDialog(true);
}

void TimeCourseDialog::on_resetViewButton_clicked()
{
    this->plot->resetView();
    this->updateDialog(true);
}
