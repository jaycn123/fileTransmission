// server.cpp: 定义控制台应用程序的入口点。
//

#define SENDSERVICE_H
#define SENDSERVICE_H

#include "common.h"
#include "netpack.h"
#include "tool.h"

class SendService
{


public:

#if defined(_WIN32)
	bool StartUp();
#else
	bool StartUp();
#endif

	void DoSendData(int c_socket, std::vector<std::string>&filepath);

	void Doaccept(std::vector<std::string>&filepath);

	void Start();

	int m_socket;

	std::vector<int>ClientScoketVec;

};



