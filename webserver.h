#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "./threadpool/threadpool.h"
#include "./http/http_conn.h"

const int MAX_FD = 65536;           //最大文件描述符
const int MAX_EVENT_NUMBER = 10000; //最大事件数
const int TIMESLOT = 5;             //最小超时单位

class WebServer
{
public:
    WebServer();
    ~WebServer();
    
    /**
     * 初始化函数，配置服务器端口、数据库认证信息、日志写入选项、触发模式、SQL连接数、线程数、日志关闭选项、actor模型等。
     * @param port 服务器监听端口
     * @param user 登陆数据库的用户名
     * @param passWord 登陆数据库的密码
     * @param databaseName 使用的数据库名
     * @param log_write 日志写入选项
     * @param opt_linger 关闭连接时的数据传输选项
     * @param trigmode 触发模式
     * @param sql_num SQL连接池大小
     * @param thread_num 线程池大小
     * @param close_log 是否关闭日志
     * @param actor_model 是否使用actor模型
     */
    void init(int port , string user, string passWord, string databaseName,
              int log_write , int opt_linger, int trigmode, int sql_num,
              int thread_num, int close_log, int actor_model);

    /**
     * 线程池函数，创建和管理线程池。
     */
    void thread_pool();

    /**
     * 数据库连接池函数，管理数据库连接。
     */
    void sql_pool();

    /**
     * 日志写入函数，处理日志写入操作。
     */
    void log_write();

    /**
     * 触发模式函数，设置和管理触发模式。
     */
    void trig_mode();

    /**
     * 事件监听函数，开始监听网络事件。
     */
    void eventListen();

    /**
     * 事件循环函数，处理发生的网络事件。
     */
    void eventLoop();

    /**
     * 定时器处理函数，在指定的连接上设置定时器。
     * @param connfd 连接文件描述符
     * @param client_address 客户端地址
     */    
    void timer(int connfd, struct sockaddr_in client_address);

    /**
     * 调整定时器函数，根据需要更新定时器。
     * @param timer 需要调整的定时器
     */    
    void adjust_timer(util_timer *timer);

    /**
     * 处理定时器函数，当定时器到期时进行相应处理。
     * @param timer 到期的定时器
     * @param sockfd 相关联的套接字文件描述符
     * @return 处理结果，成功返回true，失败返回false
     */    
    void deal_timer(util_timer *timer, int sockfd);

    /**
     * 处理客户端数据函数，解析和响应客户端请求。
     * @return 处理结果，成功返回true，失败返回false
     */    
    bool dealclientdata();

    /**
     * 处理信号函数，处理接收到的信号。
     * @param timeout 是否超时
     * @param stop_server 是否停止服务器
     * @return 处理结果，成功返回true，失败返回false
     */
    bool dealwithsignal(bool& timeout, bool& stop_server);

    /**
     * 读事件处理函数，处理读事件。
     * @param sockfd 套接字文件描述符
     */
    void dealwithread(int sockfd);

    /**
     * 写事件处理函数，处理写事件。
     * @param sockfd 套接字文件描述符
     */
    void dealwithwrite(int sockfd);

public:
    //基础
    int m_port;     // 监听端口
    char *m_root;   // 网站根目录
    int m_log_write;    // 日志写入方式
    int m_close_log;    // 日志是否关闭
    int m_actormodel;   // 模型选择

    int m_pipefd[2];    // 管道文件描述符
    int m_epollfd;      // epoll文件描述符
    http_conn *users;   // 用户连接池

    //数据库相关
    connection_pool *m_connPool;
    string m_user;              //登陆数据库用户名
    string m_passWord;          //登陆数据库密码
    string m_databaseName;      //使用数据库名
    int m_sql_num;              //数据库连接池数量

    //线程池相关
    threadpool<http_conn> *m_pool;
    int m_thread_num;

    //epoll_event相关
    epoll_event events[MAX_EVENT_NUMBER];

    int m_listenfd;
    int m_OPT_LINGER;
    int m_TRIGMode;
    int m_LISTENTrigmode;
    int m_CONNTrigmode;

    //定时器相关
    client_data *users_timer;
    Utils utils;
};
#endif
