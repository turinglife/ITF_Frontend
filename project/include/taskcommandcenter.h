#ifndef TASKCOMMANDCENTER_H
#define TASKCOMMANDCENTER_H

#include <QObject>

class TaskCommandCenter : public QObject
{
    Q_OBJECT
public:
    explicit TaskCommandCenter(std::string task_name, QObject *parent = 0);
    ~TaskCommandCenter();


signals:
    void DaemonOpened(QString daemon, QString operaton);
    void CommandExecuted(QString daemon, QString operation, bool flag);

public slots:
    void OpenDaemon(const QString &daemon, const QString &operation);
    void ExecuteCommand(const QString &daemon, const QString &operation);

private:
    bool CommuBySocket(const std::string &message, const std::string &socket_path, std::string &received);

    std::string task_name_;
};

#endif // TASKCOMMANDCENTER_H
