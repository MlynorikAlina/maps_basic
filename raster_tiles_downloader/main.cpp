#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QNetworkAccessManager>
#include "helpers.h"

// Функция перевода широты/долготы в координаты тайла Web Mercator (XYZ)
QPoint latLonToTile(double lat, double lon, int zoom) {
    double n = qPow(2.0, zoom);

    int x = static_cast<int>(qFloor((lon + 180.0) / 360.0 * n));

    double latRad = qDegreesToRadians(lat);
    int y = static_cast<int>(qFloor((1.0 - qLn(qTan(latRad) + 1.0 / qCos(latRad)) / M_PI) * n / 2.0));

    return QPoint(x, y);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Настройка парсера аргументов командной строки
    QCommandLineParser parser;
    parser.setApplicationDescription("Утилита для оффлайн скачивания тайлов спутника/карт в формате Web Mercator");
    parser.addHelpOption();

    QCommandLineOption dirOption("dir", "Целевая директория для сохранения тайлов", "path");
    QCommandLineOption urlOption("url", "Шаблон URL c плейсхолдерами {z}, {x}, {y}", "\"http://mt1.google.com/vt/lyrs=m&x={x}&y={y}&z={z}\"");
    QCommandLineOption boundsOption("bounds", "Границы в WGS84 в формате: min_lon,min_lat,max_lon,max_lat", "coords");


    parser.addOption(dirOption);
    parser.addOption(urlOption);
    parser.addOption(boundsOption);

    parser.process(a);

    // Проверка обязательных параметров
    if (!parser.isSet(dirOption) || !parser.isSet(urlOption) || !parser.isSet(boundsOption)) {
        qCritical() << "Ошибка: Все аргументы (--dir, --url, --bounds) обязательны.";
        return 1;
    }

    QString outputDirStr = parser.value(dirOption);
    QString urlTemplate = parser.value(urlOption);

    qInfo()<<outputDirStr<<" "<<urlTemplate<<" "<<parser.value(boundsOption);

    // Парсим границы (Bounding Box)
    QStringList boundsList = parser.value(boundsOption).split(',');
    if (boundsList.size() != 4) {
        qCritical() << "Ошибка: Неверный формат --bounds. Ожидается: min_lon,min_lat,max_lon,max_lat";
        return 1;
    }

    double minLon = boundsList[0].toDouble();
    double minLat = boundsList[1].toDouble();
    double maxLon = boundsList[2].toDouble();
    double maxLat = boundsList[3].toDouble();

    // Инициализируем сетевой менеджер Qt
    Downloader downloader;
    QObject::connect(&downloader, &Downloader::quit, &a, &QCoreApplication::exit);

    qInfo() << "Старт парсинга и закачки тайлов...";
    qInfo() << "Директория:" << outputDirStr;

    bool need_download = false;

    // Основной цикл по уровням зума
    for (int z = 13; z <= 14; ++z) {
        qInfo() << "--- Обработка Zoom Level:" << z << "---";

        // Рассчитываем индексы тайлов для угловых точек
        // Важно: в Web Mercator Y идет сверху вниз, поэтому minLat дает максимальный Y, а maxLat — минимальный Y.
        QPoint topLeftTile = latLonToTile(maxLat, minLon, z);
        QPoint bottomRightTile = latLonToTile(minLat, maxLon, z);

        int minX = topLeftTile.x();
        int maxX = bottomRightTile.x();
        int minY = topLeftTile.y();
        int maxY = bottomRightTile.y();

        // Проверяем и корректируем границы сетки для данного зума
        int maxTileIndex = qPow(2, z) - 1;
        minX = qMax(0, minX); maxX = qMin(maxTileIndex, maxX);
        minY = qMax(0, minY); maxY = qMin(maxTileIndex, maxY);

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {

                // Формируем структуру папок структуры: output_dir/Z/X/Y.ext
                // Извлекаем расширение файла из шаблона URL (обычно .png, .jpg или .webp)
                QString ext = "jpeg";

                QString relativePath = QString("%1/%2/%3.%4").arg(z).arg(x).arg(y).arg(ext);
                QString absoluteSavePath = QDir(outputDirStr).absoluteFilePath(relativePath);
                QFileInfo fileInfo(absoluteSavePath);

                // 🔍 Проверяем, существует ли файл локально
                if (fileInfo.exists() && fileInfo.size() > 0) {
                    // qDebug() << "⏭️ Файл уже существует, пропускаем:" << relativePath;
                    continue;
                }

                need_download = true;

                // Создаем вложенные директории (Z/X), если их нет
                QDir().mkpath(fileInfo.absolutePath());

                // Формируем URL тайла, подставляя Z, X, Y
                QString tileUrl = urlTemplate;
                tileUrl.replace("{z}", QString::number(z))
                    .replace("{x}", QString::number(x))
                    .replace("{y}", QString::number(y));

                // Скачиваем тайл

                downloader.downloadTile(tileUrl, absoluteSavePath);
            }
        }
    }
    if(need_download)
        return a.exec();
    else return 0;
}
