#include <QApplication>
#include "intelligentvideoanalysis.h"
#include <QMessageBox>
#include "utility.h"
#include "common.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Intelligent Video Analysis for Crowd Management");

    /* Test DB */
    CDbi db;
    if (!DBHelper::IsDBConnected(db)) {
        QMessageBox::information(NULL, "Database Check", "DB connnect Failed!", QMessageBox::Ok, QMessageBox::Ok);
    }

    /* Test Daemon Path*/
    if (!Utility::FileIsExist(QString::fromStdString(kDaemonPath))) {
        QMessageBox::information(NULL, "Daemon Check", "Daemon can not find!", QMessageBox::Ok, QMessageBox::Ok);
    }

    IntelligentVideoAnalysis iva;
    iva.show();

    return a.exec();
}
