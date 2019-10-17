#include "network.h"

Network::Network(QObject *parent) : QObject(parent)
{
    connect(&manager, &QNetworkAccessManager::finished, this, &Network::readData);
}

Network::~Network(){}

void Network::getRequest(QUrl url)
{
    manager.get(QNetworkRequest(url));
}

void Network::readData(QNetworkReply *reply)
{
    data = QString(reply->readAll());
    reply->deleteLater();
    emit dataRead(reply->url());
}

QString Network::getData() const
{
    return data;
}
