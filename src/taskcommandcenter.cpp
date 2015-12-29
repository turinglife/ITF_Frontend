#include "taskcommandcenter.h"
#include "common.h"
#include "utility.h"

TaskCommandCenter::TaskCommandCenter(std::string task_name, QObject *parent)
    : QObject(parent)
    , task_name_(task_name)
{

}

TaskCommandCenter::~TaskCommandCenter()
{
    std::cout << "releaseTaskCommandCenter begin" << std::endl;
    std::cout << "releaseTaskCommandCenter done" << std::endl;
}

void TaskCommandCenter::OpenTask(const QString &daemon, const QString &operation)
{
    // Open Daemon
    OpenDaemon(daemon);

    // ExecuteCommand
    ExecuteCommand(daemon, operation);
}

void TaskCommandCenter::ExecuteCommand(const QString &daemon, const QString &operation)
{
    bool flag = MessageHandler(daemon, operation);

    emit CommandExecuted(daemon, operation, flag);
}

void TaskCommandCenter::OpenDaemon(const QString &daemon)
{
    std::string dae = daemon.toStdString();
    std::transform(dae.begin(), dae.end(), dae.begin(), ::tolower);
    int status = 0;

    pid_t mpid = fork();
    switch (mpid) {
    case -1:
        std::cout << "Create ad/cd Fail!" << std::endl;
        break;
    case 0:
    {
        pid_t pid = fork();
        if (pid == 0) {
            std::string path = kDaemonPath + dae + ".bin";
            std::string arg0 = dae + ".bin_" + task_name_;
            std::string arg1 =  task_name_;
            char* parmList[] = {const_cast<char*>(arg0.c_str()), const_cast<char*>(arg1.c_str()), 0};
            execv(path.c_str(), parmList);
            std::cout << "ERROR when execv" << std::endl;
        } else {
            exit(0);
        }
    }
        break;
    default:
        waitpid(mpid, &status, 0);
        break;
    }
}

bool TaskCommandCenter::MessageHandler(const QString &daemon, const QString &operation)
{
    std::string dae = daemon.toStdString();
    socket_path_ = dae + "_" + task_name_;
    socket_file_ = boost::filesystem::current_path().string() + "/" + socket_path_;
    operaton_ = operation.toStdString();

    int connection_times = 0;
    while (++connection_times < kMaxConnectionTimes) {
        std::string received;
        if (CommuBySocket(operaton_, socket_path_, received)) {
            if (received.compare(kMessageReceivedOk) == 0) {
                return true;
            } else {
                return false;
            }
        } else {
            if (received.compare(kMessageReceivedNoSuchFile) == 0) {   /// Daemon is openning but not finish
                sleep(1);
                continue;
            } else if (received.compare(kMessageReceivedConnectionRefused) == 0) { /// remove socket file and Open Daemon
                Utility::RemoveFile(socket_file_);
                OpenDaemon(daemon);
                continue;
            } else {
                std::cout << "Received Message: " << received << std::endl;
            }
        }
    }

    return false;
}

bool TaskCommandCenter::CommuBySocket(const std::string &message, const std::string &socket_path, std::string &received)
{
    /* If do not specify socket_path, default socket_path will be employed.
     * AF_UNIX: The AF_UNIX (also known as AF_LOCAL) socket family is used to communicate
     * between processes on the same machine efficiently.
     * SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based byte streams.
     * An out-of-band data transmission mechanism may be sup-ported. */
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
//        perror("ERROR socket");
        received = std::string(strerror(errno));
        return false;
    }

    /* Fill out socked address */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path)-1);

    /* connect to server(in our example, server is another process running in the same machine. */
    if (::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
//        perror("ERROR connect");
        received = std::string(strerror(errno));
        return false;
    }

    // send message to server
    if (send(fd, message.c_str(), message.size(), MSG_NOSIGNAL) < 0) {
//        perror("ERROR send");
        received = std::string(strerror(errno));
        return false;
    }

    // receive message from server
    int recvbytes = 0;
    char recv_buf[256];
    memset(&recv_buf, 0, sizeof(recv_buf));
    if ((recvbytes = recv(fd, recv_buf, sizeof(recv_buf), 0)) > 0) {
        recv_buf[recvbytes] = '\0';
        received = std::string(recv_buf);
    } else {
        if (recvbytes < 0) {
//            perror("ERROR recv");
            received = std::string(strerror(errno));
        } else {
//            perror("ERROR server closed connection");
            received = std::string(strerror(errno));

        }
    }

    close(fd);

    return true;
}
