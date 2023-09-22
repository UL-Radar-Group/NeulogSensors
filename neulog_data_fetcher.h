#ifndef NEULOG_DATA_FETCHER_H
#define NEULOG_DATA_FETCHER_H

#include <QtNetwork>

class NeulogDataFetcher : public QObject {
    Q_OBJECT
public:
    const int updateRate = 100; // Time period to receive data in [ms]

    NeulogDataFetcher(QObject* parent = nullptr) : QObject(parent), manager_(new QNetworkAccessManager(this)) {
        connect(manager_, &QNetworkAccessManager::finished, this, &NeulogDataFetcher::onDataReceived);

        // Create a timer for periodic data fetching (adjust the interval as needed)
        dataFetchTimer_ = new QTimer(this);
        connect(dataFetchTimer_, &QTimer::timeout, this, &NeulogDataFetcher::fetchData);
        dataFetchTimer_->start(updateRate); // Fetch data every 100 milliseconds
    }

signals:
    void dataReceived(int pulseValue, int respirationValue);

private slots:
    void fetchData() {
        // Record the time before making the API request
        requestTime_ = QDateTime::currentDateTime();

        QUrl url("http://localhost:22004/NeuLogAPI?GetSensorValue:[Pulse],[1],[Respiration],[1]");
        QNetworkRequest request(url);
        manager_->get(request);
    }

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
                            emit dataReceived(dataArray.at(0).toInt(), dataArray.at(1).toInt());
                        }
                    }
                }
            }
        }
        reply->deleteLater();
    }

private:
    QNetworkAccessManager* manager_;
    QTimer* dataFetchTimer_;
    QDateTime requestTime_;
};
#endif // NEULOG_DATA_FETCHER_H
