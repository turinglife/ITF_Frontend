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
    void OpenTask(const QString &daemon, const QString &operation);
    void ExecuteCommand(const QString &daemon, const QString &operation);

private:
    void OpenDaemon(const QString &daemon);
    bool MessageHandler(const QString &daemon, const QString &operation);
    bool CommuBySocket(const std::string &message, const std::string &socket_path, std::string &received);

    std::string task_name_;
    std::string socket_path_;
    std::string socket_file_;
    std::string operaton_;
};

#endif // TASKCOMMANDCENTER_H
