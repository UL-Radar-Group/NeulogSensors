#ifndef NEULOGDATAFETCHER_H
#define NEULOGDATAFETCHER_H

#include <QtWidgets>
#include <QtNetwork>
#include "qcustomplot.h"

class NeulogDataFetcher : public QObject {
    Q_OBJECT
public:
    NeulogDataFetcher(QCustomPlot* customPlot) : customPlot_(customPlot) {
        manager_ = new QNetworkAccessManager(this);
        connect(manager_, &QNetworkAccessManager::finished, this, &NeulogDataFetcher::onDataReceived);

        // Create a graphs for Pulse and Respiration data
        pulseGraph_ = customPlot_->addGraph();
        pulseGraph_->setName("Pulse");
        pulseGraph_->setPen(QPen(PulseColor));

        respirationGraph_ = customPlot_->addGraph(customPlot->xAxis, customPlot->yAxis2);
        respirationGraph_->setName("Respiration");
        respirationGraph_->setPen(QPen(RespirationColor));

        // Create a timer for periodic data fetching (adjust the interval as needed)
        dataFetchTimer_ = new QTimer(this);
        connect(dataFetchTimer_, &QTimer::timeout, this, &NeulogDataFetcher::fetchData);
        dataFetchTimer_->start(FrameRate); // Fetch data every 100 milliseconds
        startTime_ = QDateTime::currentDateTime();
    }
    const QColor PulseColor = QColor(Qt::red); // Color for pulse sensor
    const QColor RespirationColor = QColor(Qt::blue); // Color for respiration sensor
    const int xAxisRange = 30; // Visible time range in [s]
    const int FrameRate = 100; // Update time in [ms] (should be larger then 80)


public slots:
    void fetchData() {
        // Record the time before making the API request
        requestTime_ = QDateTime::currentDateTime();

        QUrl url("http://localhost:22004/NeuLogAPI?GetSensorValue:[Pulse],[1],[Respiration],[1]");
        QNetworkRequest request(url);
        manager_->get(request);
    }
private slots:
    void onDataReceived(QNetworkReply* reply) {
        if (reply->error() == QNetworkReply::NoError) {
            auto data = reply->readAll();
            qDebug() << "Data received after" << requestTime_.msecsTo(QDateTime::currentDateTime()) << "ms";

            QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
            if (!jsonDoc.isNull()) {
                QJsonObject jsonObj = jsonDoc.object();
                if (jsonObj.contains("GetSensorValue")) {
                    QJsonValue sensorData = jsonObj["GetSensorValue"];
                    if (sensorData.isArray()) {
                        QJsonArray dataArray = sensorData.toArray();
                        if (dataArray.size() == 2) {
                            int pulseValue = dataArray.at(0).toInt();
                            int respirationValue = dataArray.at(1).toInt();
                            updateChart(pulseValue, respirationValue);
                        }
                    }
                }
            }
        }
        reply->deleteLater();
    }

    void updateChart(int pulseValue, int respirationValue) {
        // Add new data points to the graph
        double time = startTime_.msecsTo(QDateTime::currentDateTime()) / 1000.;
        pulseGraph_->addData(time, pulseValue);
        respirationGraph_->addData(time, respirationValue);

        // Redraw the custom plot
        customPlot_->xAxis->setRange(time - xAxisRange, time);
        customPlot_->replot();
    }

private:
    QNetworkAccessManager* manager_;
    QCustomPlot * customPlot_;
    QCPGraph* pulseGraph_;
    QCPGraph* respirationGraph_;
    QTimer *dataFetchTimer_;
    QDateTime requestTime_;
    QDateTime startTime_;
};
#endif // NEULOGDATAFETCHER_H
