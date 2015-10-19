/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef APPCONFIG_H
#define APPCONFIG_H

class QString;

/**
 * @brief The AppConfig class
 */
class AppConfig
{
public:
    static QString app_path_;
    static QString app_title_;    //app name
    static int desk_width_;       // desktop width
    static int desk_height_;      // desktop height;

    static void ReadConfig();
    static void WriteConfig();
};
#endif // APPCONFIG_H
