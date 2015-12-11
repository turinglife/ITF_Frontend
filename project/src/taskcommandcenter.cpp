#include "taskcommandcenter.h"
#include "common.h"

TaskCommandCenter::TaskCommandCenter(std::string task_name, QObject *parent)
    : QObject(parent)
    , task_name_(task_name)
{

}

TaskCommandCenter::~TaskCommandCenter()
{

}

void TaskCommandCenter::OpenDaemon(const QString &daemon, const QString &operation)
{
    std::string dae = daemon.toStdString();
    std::transform(dae.begin(), dae.end(), dae.begin(), ::tolower);
    int status = 0;


    std::cout<<"enter TaskCommandCenter::OpenDaemon function"<<std::endl;

    pid_t mpid = fork();
    switch (mpid) {
    case -1:
        std::cout << "Create ad/cd Fail!" << std::endl;
        break;
    case 0:
    {
        pid_t pid = fork();
        if (pid == 0) {
            std::string path = "/usr/local/itf/daemons/" + dae + ".bin";
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

    // need to change sleep time in different machines, because starting cd is so slow in some machines
    sleep(4);

    ExecuteCommand(daemon, operation);
}

void TaskCommandCenter::ExecuteCommand(const QString &daemon, const QString &operation)
{
    bool flag = false;
    std::string dae = daemon.toStdString();
    std::string message = operation.toStdString();
    std::string socket_path = dae + "_" + task_name_;
    std::string received;

    if (!CommuBySocket(message, socket_path, received)) {
        std::cerr << "Fail to send " << message << " to " << dae << "_" << task_name_ << std::endl;
    } else if (received == kMessageReceivedOk) {
        flag = true;
    }

    emit CommandExecuted(daemon, operation, flag);
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
        perror("ERROR socket");
        return false;
    }

    /* Fill out socked address */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path)-1);

    /* connect to server(in our example, server is another process running in the same machine. */
    if (::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("ERROR connect");
        return false;
    }

    // send message to server
    if (send(fd, message.c_str(), message.size(), MSG_NOSIGNAL) < 0) {
        perror("ERROR send");
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
            perror("ERROR recv");
        } else {
            perror("ERROR server closed connection");
        }
    }

    close(fd);

    return true;
}
