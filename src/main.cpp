#include <QApplication>
#include "intelligentvideoanalysis.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Intelligent Video Analysis for Crowd Management");

    IntelligentVideoAnalysis iva;
    iva.show();

    return a.exec();
}
