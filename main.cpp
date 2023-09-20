#include <QApplication>
#include "NeulogDataFetcher.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    QWidget* centralWidget = new QWidget(&mainWindow);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QCustomPlot customPlot;
    layout->addWidget(&customPlot);
    NeulogDataFetcher dataFetcher(&customPlot);

    customPlot.xAxis->setLabel("Time [s] ");
    customPlot.yAxis->setLabel("Pulse");
    customPlot.yAxis->setRange(QCPRange(50, 200));
    customPlot.yAxis->setLabelColor(dataFetcher.PulseColor);
    customPlot.yAxis->setTickLabelColor(dataFetcher.PulseColor);
    customPlot.yAxis->setBasePen(QPen(dataFetcher.PulseColor));
    customPlot.yAxis->setTickPen(QPen(dataFetcher.PulseColor));
    customPlot.yAxis->setSubTickPen(QPen(dataFetcher.PulseColor));

    customPlot.yAxis2->setLabel("Respiration");
    customPlot.yAxis2->setRange(QCPRange(0, 2000));
    customPlot.yAxis2->setLabelColor(dataFetcher.RespirationColor);
    customPlot.yAxis2->setTickLabelColor(dataFetcher.RespirationColor);
    customPlot.yAxis2->setBasePen(QPen(dataFetcher.RespirationColor));
    customPlot.yAxis2->setTickPen(QPen(dataFetcher.RespirationColor));
    customPlot.yAxis2->setSubTickPen(QPen(dataFetcher.RespirationColor));
    customPlot.yAxis2->setVisible(true);

    customPlot.setInteractions(QCP::iRangeZoom | QCP::iSelectAxes);

    mainWindow.setMinimumSize(800, 600);  // Set the minimum size to 800x600 pixels
    mainWindow.setCentralWidget(centralWidget);
    mainWindow.show();

    return app.exec();
}

