#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QObject>

class Network : public QObject
{
    Q_OBJECT

    public:
        explicit Network(QObject *parent = 0);
        virtual ~Network();
        void getRequest(QUrl url);
        QString getData() const;

    signals:
        void dataRead(QUrl url);

    private slots:
        void readData(QNetworkReply *reply);

    private:
        QNetworkAccessManager manager;
        QString data;
};

#endif
