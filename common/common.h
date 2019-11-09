#ifndef COMMON_H
#define COMMON_H


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <functional>
#include <stdio.h>
#include <fstream> 

#define _WINSOCK_DEPRECATED_NO_WARNINGS 0
#if defined(_WIN32)

#include <WinSock2.h>
#include <windows.h>
#include <corecrt_io.h>
#include <WS2tcpip.h>

#pragma comment(lib, "WS2_32")



#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/md5.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>

#endif


#define PORT 8087
#define BUFFER_SIZE 1576 * 10
#define CACHE_SIZE 1576 * 100
#define NowTime  time(NULL)
#define MILLION 1000000
#define SBUFFER_SIZE 1024

#endif