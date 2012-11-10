#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("kcyberoam","kcyberoam", ki18n("KCyberoam"), "0.2",ki18n("Cyberoam Login Management Client"),
                         KAboutData::License_LGPL_V3, ki18n("Copyright (c) 2012"),ki18n("Source code available at"), QByteArray(), "jaydp17@gmail.com");
    KCmdLineArgs::init(argc,argv,&aboutData);

    KApplication app;

    MainWindow *w = new MainWindow();
    w->show();
    
    return app.exec();
}
