#include "GeoInfo.h"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>

int main(int argc, char *argv[])
{
    QCommandLineParser parser;
    parser.addHelpOption();

    QString proj_root = "./../../";

    QCommandLineOption m("m","Program mode(required):\ng - generate tiles (expected --pbf and -d options set),\n"
                              "\tc - configure styles(expected -d, -i, -f options set),\n"
                              "\tr - render maps (expected -s, -r, --lat, --lon options; additional options: -W, -H, -o)","mode","g");
    parser.addOption(m);

    QCommandLineOption pbf("pbf","Osm.pbf file with data","pbf",proj_root + "data/belarus.osm.pbf");
    parser.addOption(pbf);
    QCommandLineOption d("d","Vector tiles directory","tiles_dir",proj_root + "data/tiles/");
    parser.addOption(d);

    QCommandLineOption i("i","Sprite location","sprite",proj_root+"styles/data/sprite/");
    parser.addOption(i);
    QCommandLineOption f("f","Font directory","font_dir",proj_root+"styles/data/openmaptiles-fonts/fonts/");
    parser.addOption(f);

    QCommandLineOption s("s","style.json","style",proj_root+"styles/osm-bright-style/style.json");
    parser.addOption(s);
    QCommandLineOption r("r","Distance radius(km) list","radius","1,2,3");
    parser.addOption(r);
    QCommandLineOption olat("lat","Sets center lattitude","lattitude","53.935");
    parser.addOption(olat);
    QCommandLineOption olon("lon","Sets center longitude","longitude","27.635");
    parser.addOption(olon);
    QCommandLineOption w("W","Sets image width","width","1024");
    parser.addOption(w);
    QCommandLineOption h("H","Sets image height","height","1024");
    parser.addOption(h);
    QCommandLineOption o("o","Sets out file name","output","./map.png");
    parser.addOption(o);

    QStringList l;
    for(int i=0;i<argc;i++)
        l<<argv[i];

    parser.process(l);

    if(!parser.isSet(m))
        throw std::invalid_argument("No mode(-m) argument provided.");

    QString mode = parser.value(m);

    QString program;
    QString working_dir;
    QStringList arguments;

    if(mode == "g"){
        qInfo()<<"Generating tiles...";

        if(!parser.isSet(pbf))
            throw std::invalid_argument("No required osm pbf files(--pbf) argument provided.");

        program = "/bin/bash";
        working_dir = proj_root + "data/";
        arguments<<"./make_tiles.sh"<<QDir(parser.value(pbf)).absolutePath()<<QDir(parser.value(d)).absolutePath();

    }else if(mode == "c"){
        qInfo()<<"Configuring styles...";

        if(!parser.isSet(d))
            throw std::invalid_argument("No required vector tiles directory(-d) argument provided.");

        program = "python3";
        working_dir = proj_root + "styles/";
        arguments<<"./make_static.py"
                  <<"-t"<<QDir(parser.value(d)).absolutePath()
                  <<"-f"<<QDir(parser.value(f)).absolutePath()
                  <<"-s"<<QDir(parser.value(i)).absolutePath();

    }else if(mode == "r"){
        qInfo()<<"Rendering map...";

        if(!parser.isSet(s)||!parser.isSet(r)||!parser.isSet(olon)||!parser.isSet(olat))
            throw std::invalid_argument("No style(-s), radius(-r), --lat or --lon argument provided.");

        double lat = parser.value(olat).toDouble();
        double lon = parser.value(olon).toDouble();
        double dist = parser.value(r).toDouble();
        double dlon = GeoInfo::km2long(dist,lat);
        double dlat = GeoInfo::km2lat(dist);

        program = "mbgl-render";
        working_dir = "./";
        arguments<<"-s"<<parser.value(s)
                  <<"--bounds"<<QString::number(lat+dlat)<<QString::number(lon-dlon)<<QString::number(lat-dlat)<<QString::number(lon+dlon)
                  <<"-w"<<parser.value(w)
                  <<"-h"<<parser.value(h)
                  <<"-o"<<parser.value(o);

    }else throw std::invalid_argument("Undefined programm mode(-m) passed.");

    QProcess p;
    p.setWorkingDirectory(working_dir);
    p.start(program, arguments);
    p.waitForFinished(-1);
    qInfo()<<p.readAllStandardOutput();
    qInfo()<<p.readAllStandardError();
    p.close();

    return 0;
}
