#ifndef DATA_PLOTTER_H
#define DATA_PLOTTER_H

#include <QObject>
#include "qcustomplot.h"

class DataPlotter : public QObject {
    Q_OBJECT
public:
    const QColor pulseColor_ = QColor(Qt::red); // Color for pulse sensor
    const QColor respirationColor_ = QColor(Qt::blue); // Color for respiration sensor
    const int xAxisRange_ = 30; // Visible time range in [s]

    QVector<double> save;
    bool wrote=false;
    DataPlotter(QCustomPlot* customPlot) : customPlot_(customPlot) {
        // Create a QCPGraph for Pulse and Respiration data
        pulseGraph_ = customPlot_->addGraph();
        pulseGraph_->setName("Pulse");
        respirationGraph_ = customPlot_->addGraph();
        respirationGraph_->setName("Respiration");

        // Customize plot appearance as needed
        customPlot->xAxis->setLabel("Time [s] ");
        customPlot->yAxis->setLabel("Pulse");
        customPlot->yAxis->setRange(QCPRange(0, 100));
        customPlot->yAxis->setLabelColor(pulseColor_);
        customPlot->yAxis->setTickLabelColor(pulseColor_);
        customPlot->yAxis->setBasePen(QPen(pulseColor_));
        customPlot->yAxis->setTickPen(QPen(pulseColor_));
        customPlot->yAxis->setSubTickPen(QPen(pulseColor_));

        customPlot->yAxis2->setLabel("Respiration");
        customPlot->yAxis2->setRange(QCPRange(0, 2000));
        customPlot->yAxis2->setLabelColor(respirationColor_);
        customPlot->yAxis2->setTickLabelColor(respirationColor_);
        customPlot->yAxis2->setBasePen(QPen(respirationColor_));
        customPlot->yAxis2->setTickPen(QPen(respirationColor_));
        customPlot->yAxis2->setSubTickPen(QPen(respirationColor_));
        customPlot->yAxis2->setVisible(true);

        customPlot->setInteractions(QCP::iRangeZoom);

        // Create a graphs for Pulse and Respiration data
        pulseGraph_ = customPlot_->addGraph();
        pulseGraph_->setName("Pulse");
        pulseGraph_->setPen(QPen(pulseColor_));

        respirationGraph_ = customPlot_->addGraph(customPlot->xAxis, customPlot->yAxis2);
        respirationGraph_->setName("Respiration");
        respirationGraph_->setPen(QPen(respirationColor_));
        startTime_ = QDateTime::currentDateTime();
    }

public slots:
    void updatePlot(int pulseValue, int respirationValue) {
        // Add new data points to the graph
        double time = startTime_.msecsTo(QDateTime::currentDateTime()) / 1000.;
        pulseGraph_->addData(time, pulseValue);

        save.append(time);
        save.append(respirationValue);
        if(wrote==false){
            if(save.length()>300*2){

                QFile file("log.bin");
                if(file.open(QFile::WriteOnly)){
                    file.write((char*)save.data(),save.length()*8);
                    wrote=true;
                    file.flush();
                    file.close();
                }
            }
        }
        respirationGraph_->addData(time, respirationValue);

        // Redraw the custom plot
        customPlot_->xAxis->setRange(time - xAxisRange_, time);
        customPlot_->replot();
    }

private:
    QCustomPlot* customPlot_;
    QCPGraph* pulseGraph_;
    QCPGraph* respirationGraph_;
    QDateTime startTime_;

};

#endif // DATA_PLOTTER_H
