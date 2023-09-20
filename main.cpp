#include <QApplication>
#include "NeulogDataFetcher.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    QWidget* centralWidget = new QWidget(&mainWindow);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    QChartView chartView;
    QPushButton fetchDataButton("Fetch Data");
    layout->addWidget(&chartView);
    layout->addWidget(&fetchDataButton);

    NeulogDataFetcher dataFetcher(&chartView);

    QObject::connect(&fetchDataButton, &QPushButton::clicked, &dataFetcher, &NeulogDataFetcher::fetchData);

    mainWindow.setCentralWidget(centralWidget);
    mainWindow.show();

    return app.exec();
}

