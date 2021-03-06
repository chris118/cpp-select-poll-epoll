#include <iostream>
#include <thread>
#include <iomanip>
#include <logging.h>

#include <sys/epoll.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

int main(int argc, const char *argv[])
{
    //初始化参数
    FLAGS_logtostderr = false;              //TRUE:标准输出,FALSE:文件输出
    FLAGS_alsologtostderr = true;           //除了日志文件之外是否需要标准输出
    FLAGS_colorlogtostderr = true;          //标准输出带颜色
    FLAGS_logbufsecs = 0;                   //设置可以缓冲日志的最大秒数，0指实时输出
    FLAGS_max_log_size = 50;                //日志文件大小(单位：MB)
    FLAGS_stop_logging_if_full_disk = true; //磁盘满时是否记录到磁盘
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::GLOG_INFO, "./logs/");
    int ret;
    int fd;

    const char *fifo_name = "/tmp/my_fifo";

    ret = mkfifo(fifo_name, 0666); // 创建有名管道
    if (ret != 0)
    {
        perror("mkfifo：");
    }

    fd = open(fifo_name, O_RDWR); // 读写方式打开管道
    if (fd < 0)
    {
        perror("open fifo");
        return -1;
    }

    ret = 0;
    struct epoll_event event; // 告诉内核要监听什么事件
    struct epoll_event wait_event;

    int epfd = epoll_create(10); // 创建一个 epoll 的句柄，参数要大于 0， 没有太大意义
    if (-1 == epfd)
    {
        perror("epoll_create");
        return -1;
    }

    event.data.fd = 0;      // 标准输入
    event.events = EPOLLIN; // 表示对应的文件描述符可以读

    // 事件注册函数，将标准输入描述符 0 加入监听事件
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);
    if (-1 == ret)
    {
        perror("epoll_ctl");
        return -1;
    }

    event.data.fd = fd;     // 有名管道
    event.events = EPOLLIN; // 表示对应的文件描述符可以读

    // 事件注册函数，将有名管道描述符 fd 加入监听事件
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    if (-1 == ret)
    {
        perror("epoll_ctl");
        return -1;
    }

    ret = 0;

    while (1)
    {

        // 监视并等待多个文件（标准输入，有名管道）描述符的属性变化（是否可读）
        // 没有属性变化，这个函数会阻塞，直到有变化才往下执行，这里没有设置超时
        ret = epoll_wait(epfd, &wait_event, 2, -1);
        //ret = epoll_wait(epfd, &wait_event, 2, 1000);

        if (ret == -1)
        { // 出错
            close(epfd);
            perror("epoll");
        }
        else if (ret > 0)
        { // 准备就绪的文件描述符

            char buf[100] = {0};

            if ((0 == wait_event.data.fd) && (EPOLLIN == wait_event.events & EPOLLIN))
            { // 标准输入

                read(0, buf, sizeof(buf));
                printf("stdin buf = %s\n", buf);
            }
            else if ((fd == wait_event.data.fd) && (EPOLLIN == wait_event.events & EPOLLIN))
            { // 有名管道

                read(fd, buf, sizeof(buf));
                printf("fifo buf = %s\n", buf);
            }
        }
        else if (0 == ret)
        { // 超时
            printf("time out\n");
        }
    }

    close(epfd);

    LOG(INFO) << "end";
    google::ShutdownGoogleLogging();
}
