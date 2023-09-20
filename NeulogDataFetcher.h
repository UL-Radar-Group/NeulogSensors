#ifndef NEULOGDATAFETCHER_H
#define NEULOGDATAFETCHER_H

#include <QtWidgets>
#include <QtNetwork>
#include <QtCharts>

class NeulogDataFetcher : public QObject {
    Q_OBJECT
public:
    NeulogDataFetcher(QChartView* chartView) : chartView_(chartView) {
        manager_ = new QNetworkAccessManager(this);
        connect(manager_, &QNetworkAccessManager::finished, this, &NeulogDataFetcher::onDataReceived);

        // Create a series for Pulse and Respiration data
        pulseSeries_ = new QLineSeries();
        pulseSeries_->setName("Pulse");
        respirationSeries_ = new QLineSeries();
        respirationSeries_->setName("Respiration");

        // Create a chart and add the series
        chart_ = new QChart();
        chart_->addSeries(pulseSeries_);
        chart_->addSeries(respirationSeries_);
        chart_->setTitle("Sensor Data");
        chart_->createDefaultAxes();

        // Assuming you have X and Y axes defined in your chart
        chart_->axes(Qt::Horizontal).first()->setRange(0, maxDataPoints);
        chart_->axes(Qt::Vertical).first()->setRange(0, 600); // Customize the Y-axis range as needed

        // Set the chart to the chart view
        chartView_->setChart(chart_);

        // Create a timer for periodic data fetching (adjust the interval as needed)
        dataFetchTimer_ = new QTimer(this);
        connect(dataFetchTimer_, &QTimer::timeout, this, &NeulogDataFetcher::fetchData);
        dataFetchTimer_->start(100); // Fetch data every 1000 milliseconds (1 second)
    }


public slots:
    void fetchData() {
        // Record the time before making the API request
        requestTime = QDateTime::currentDateTime();

        QUrl url("http://localhost:22004/NeuLogAPI?GetSensorValue:[Pulse],[1],[Respiration],[1]");
        QNetworkRequest request(url);
        manager_->get(request);
    }
private slots:
    void onDataReceived(QNetworkReply* reply) {
        if (reply->error() == QNetworkReply::NoError) {
            auto data = reply->readAll();
            qDebug() << "Data received after" << requestTime.msecsTo(QDateTime::currentDateTime()) << "ms";

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
        // Add new data points to the series
        pulseSeries_->append(xValue, pulseValue);
        respirationSeries_->append(xValue, respirationValue);
        xValue++;

        // Limit the number of data points displayed (e.g., show only the last 20 points)
        if (pulseSeries_->count() > maxDataPoints) {
            pulseSeries_->remove(0);
            respirationSeries_->remove(0);
            chart_->axes(Qt::Horizontal).first()->setRange(xValue - maxDataPoints, xValue);
        }
    }

private:
    QNetworkAccessManager* manager_;
    QChartView* chartView_;
    QChart* chart_;
    QLineSeries* pulseSeries_;
    QLineSeries* respirationSeries_;
    QTimer *dataFetchTimer_;
    QDateTime requestTime;

    const int maxDataPoints = 30;
    int xValue = 0;
};
#endif // NEULOGDATAFETCHER_H
