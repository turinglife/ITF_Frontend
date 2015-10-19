/****************************************************************************
 *
 *  Copyright (C) 2015 CUHK. All rights reserved.
 *  Author: Liping Zhang
 *
 * ****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

class QStackedWidget;
class UTreeWidget;
class UTaskBoard;
class Delegate;
class UAnalyzeBoard;

/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void RecoverAnalyzeTask();

private:
    void LoadTaskTree();
    bool LoadGroupsInfo();
    bool LoadTasksInfo();
    bool IsGroupNameExist(QString str);
    bool InsertToGroups(QString str);
    bool DeleteFromTable(const std::string &table, const std::string &key, const std::string &val);
    bool SelectFromTable(const std::string &table, std::vector<std::map<std::string, std::string> > &res);
    bool SelectFromTable(const std::string &table, const std::string &key, const std::string &val, std::vector<std::map<std::string, std::string> > &res);

    void CreateDelegate(std::vector<std::map<std::string, std::string> > res, int channel);
    void EndApp();

    QStackedWidget *stackedwidget_;

    QTabWidget *tabwidget_;
    UTreeWidget *tasktree_;
    UTaskBoard *taskboard_;
    Delegate *delegate_;
    UAnalyzeBoard *analyzeboard_;
    QHash<int, Delegate *> delegate_hash_;

    QTimer timer_;

private slots:
    void CreateGroup();
    void DeleteGroup();
    void CreateTask();
    void DeleteTask();
    void SelectChannel(QString, int channel);
    void RemoveFromChannel(int);
    void ShowTaskBoard(int channel);
    void ShowAnalyzeBoard(int channel);
};

#endif // MAINWINDOW_H
