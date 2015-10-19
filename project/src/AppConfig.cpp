#include "AppConfig.h"
#include "utility.h"

QString AppConfig::app_path_ = "";
QString AppConfig::app_title_ = "ITF_Frontend";
int AppConfig::desk_width_ = 0;
int AppConfig::desk_height_ = 0;

//void AppConfig::ReadConfig() {
//    QString filename = AppConfig::app_path_ + "config.txt";

//    if (!Utility::FileIsExist(filename)) {
//        return;
//    }
//}

//void AppConfig::WriteConfig() {
//    QString filename = AppConfig::app_path_ + "config.txt";
//    QSettings *setting = new QSettings(filename, QSettings::IniFormat);

//    setting->beginGroup("AppConfig");
//    setting->setValue("AppTitle", AppConfig::app_title_);
//    setting->endGroup();
//}

