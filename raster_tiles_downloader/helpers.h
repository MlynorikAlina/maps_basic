#ifndef HELPERS_H
#define HELPERS_H

#include "qcoreapplication.h"
#include "qmath.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPoint>
#include <QObject>
#include <QFile>
#include <QTimer>

class Downloader : public QObject{
    Q_OBJECT
private:
    QNetworkAccessManager manager;
    int connections=0;

public:

    Downloader(){
        connect(&manager, &QNetworkAccessManager::finished, this, &Downloader::finished);
    }

    void downloadTile(const QString &urlStr, const QString &savePath) {
        // qInfo() << "📥 Скачивание:" << savePath << "...";

        QUrl url(urlStr);
        QNetworkRequest request(url);
        // Добавляем User-Agent, чтобы сервера не блокировали пустые запросы
        request.setHeader(QNetworkRequest::UserAgentHeader, "QtTileDownloader/1.0");

        QNetworkReply *reply = manager.get(request);
        reply->setProperty("path",savePath);

        connections++;

    }
signals:
    void quit(int c);
public slots:
    void finished(QNetworkReply *reply){

        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "❌ Ошибка скачивания:" << reply->errorString() << "для URL:" << reply->url();

            QNetworkRequest request(reply->url());
            request.setHeader(QNetworkRequest::UserAgentHeader, "QtTileDownloader/1.0");
            QNetworkReply *nreply = manager.get(request);
            nreply->setProperty("path",reply->property("path"));

            reply->deleteLater();
            return;
        }

        QString savePath = reply->property("path").toString();

        QFile file(savePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "❌ Не удалось создать файл на диске:" << savePath;
            reply->deleteLater();
            // emit quit(1);
            qApp->exit(1);
        }

        file.write(reply->readAll());
        file.close();

        reply->deleteLater();

        connections--;

        if(connections==0) {
            qInfo() << "🎉 Процесс завершен!";
            emit quit(0);

        }

    }
};

// Функция для синхронного скачивания одного тайла через QEventLoop


#endif // HELPERS_H
