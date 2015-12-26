#ifndef DBHELPER_H
#define DBHELPER_H

#include "common.h"
#include "dbi.hpp"

/**
 * @brief The DBHelper class
 */
class DBHelper
{
public:
    /**
     * @brief IsDBConnected
     * @param db
     * @return
     */
    static bool IsDBConnected(CDbi &db) {
        std::cout << "Connect ... " << std::endl;
        if (!db.Connect(server, user, pass)) {
            std::cout << "Fail" << std::endl;
            return false;
        } else {
            std::cout << "OK" << std::endl;
        }

        std::cout<< "Select DB ... " << std::endl;
        if (!db.UseDB(db_name)) {
            std::cout << "Fail" << std::endl;
            return false;
        } else {
            std::cout << "OK" << std::endl;
        }
        return true;
    }

    /**
     * @brief InsertToGroups
     * @param str
     * @return
     */
    static bool InsertToGroups(const string &str) {
        // Connect to DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        return db.RunSQL("INSERT INTO Groups VALUES ('" + str + "');");
    }

    /**
     * @brief InsertToTasks
     * @param task_name
     * @param task_type
     * @param task_status
     * @param camera_type
     * @param camera_status
     * @param task_path
     * @param alarm_switch
     * @param report_switch
     * @param group_name
     * @return
     */
    static bool InsertToTasks (const string &task_name, const string &task_type, const string &task_status, const string &camera_type, const string &camera_status,const string &task_path, const string &alarm_switch, const string &report_switch, const string &group_name) {
        // Connect DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        // Insert to Tasks
        bool ok = db.RunSQL("INSERT INTO Tasks VALUES ('"
                             + task_name + "', '"
                             + task_type + "', '"
                             + task_status + "', '"
                             + camera_type + "', '"
                             + camera_status + "', '"
                             + task_path + "', '"
                             + alarm_switch + "', '"
                             + report_switch + "', '"
                             + group_name + "');");
        return ok;
    }

    /**
     * @brief InsertToCameras
     * @param camera_name
     * @param width
     * @param height
     * @param address
     * @param host
     * @param port
     * @param username
     * @param password
     * @return
     */
    static bool InsertToCameras(const string &camera_name, int width, int height, const string &address, const string &host, const string &port, const string &username, const string &password) {
        // Connect to DB
        CDbi db_;
        if (! IsDBConnected(db_))
            return false;
        // Insert New Camera info to DB
        bool ok = db_.RunSQL("INSERT INTO Cameras VALUES ('"
                             + camera_name + "', "
                             + std::to_string(width) + ", "
                             + std::to_string(height) + ", '"
                             + address + "', '"
                             + host + "', '"
                             + port + "', '"
                             + username + "', '"
                             + password + "');");
        return ok;
    }

    /**
     * @brief InsertToTaskCamera
     * @param task_name
     * @param camera_name
     * @return
     */
    static bool InsertToTaskCamera(const string &task_name, const string &camera_name) {
        // Connect DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        // Insert to Task_Camera
        bool ok = db.RunSQL("INSERT INTO Task_Camera VALUES ('"
                             + task_name + "', '"
                             + camera_name + "');");
        return ok;
    }

    /**
     * @brief InsertToFiles
     * @param file_url
     * @param width
     * @param height
     * @param fps
     * @param total_frames
     * @param task_name
     * @return
     */
    static bool InsertToFiles(const string &file_url, int width, int height, int fps, int total_frames, const string &task_name) {
        // Connect DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        // Insert to Files
        bool ok = db.RunSQL("INSERT INTO Files VALUE ('"
                             + file_url + "', "
                             + std::to_string(width) + ", "
                             + std::to_string(height) + ", "
                             + std::to_string(fps) + ", "
                             + std::to_string(total_frames) + ", '"
                             + task_name + "');");
        return ok;
    }

    static bool InsertToDensityDetail(const string &pers_file, const string &roi_file, const string &lm_file, const string &task_name) {
        // Connect to DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        // Insert to DensityDetail
        bool ok = db.RunSQL("INSERT INTO DensityDetail VALUES ('"
                             + pers_file+ "', '"
                             + roi_file + "', '"
                             + lm_file + "', '"
                             + task_name + "');");
        return ok;
    }

    /**
     * @brief InsertToDensityAlarmStrategy
     * @param priority_low
     * @param priority_medium
     * @param priority_high
     * @param task_name
     * @return
     */
    static bool InsertToDensityAlarmStrategy(int priority_low, int priority_medium, int priority_high, const string &task_name) {
        // Connect DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        // Insert to DensityAlarmStrategy
        bool ok = db.RunSQL("INSERT INTO DensityAlarmStrategy VALUES ("
                             + std::to_string(priority_low) + ", "
                             + std::to_string(priority_medium) + ", "
                             + std::to_string(priority_high) + ", '"
                             + task_name + "');");
        return ok;
    }



    /**
     * @brief DeleteFromTable
     * @param table
     * @param key
     * @param val
     * @return
     */
    static bool DeleteFromTable(const string &table, const string &key, const string &val) {
        // Connect to DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        return db.RunSQL("DELETE FROM " + table + " WHERE " + key + "='" + val + "';");
    }

    /**
     * @brief SelectFromTable
     * @param table
     * @param res
     * @return
     */
    static bool SelectFromTable(const string &table, std::vector<std::map<string, string> > &res) {
        // Connect to DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        // Get Info From Table
        res = db.Query("SELECT * FROM " + table + ";");
        return true;
    }

    /**
     * @brief SelectFromTable
     * @param table
     * @param key
     * @param val
     * @param res
     * @return
     */
    static bool SelectFromTable(const string &table, const string &key, const string &val, std::vector<std::map<string, string> > &res) {
        // Connect to DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        // Get Info From Table
        res = db.Query("SELECT * FROM " + table + " WHERE " + key + "='" + val + "';");
        return true;
    }

    static bool SelectFromTable(const string &table, const string &key, const string &val, const string &time1, const string &time2, std::vector<std::map<string, string> > &res) {
        // Connect to DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        // Get Info From Table
        res = db.Query("SELECT * FROM " + table + " WHERE " + key + "='" + val + "' AND date_time >='" + time1 + "' AND date_time <'" + time2 + "';");
        return true;
    }

    static bool SelectFromDensityAlarmRecord(std::vector<std::map<string, string> > &res) {
        // Connect to DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;
        // Get Info From Table
        res = db.Query("SELECT * FROM DensityAlarmRecord ORDER BY date_time DESC LIMIT 1000 OFFSET 0;");
        return true;
    }

    static bool UpdateStatusToTasks(const string &key, const string &status, const string &task_name) {
        // Connect to DB
        CDbi db;
        if (! IsDBConnected(db))
            return false;

        bool ok = db.RunSQL("UPDATE Tasks SET " + key + "='" + status + "' WHERE task_name='" + task_name + "';");
        return ok;
    }
};

#endif // DBHELPER_H
