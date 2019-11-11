#include "sendService.h"

#if defined(_WIN32)
bool SendService::StartUp()
{
	//声明地址结构
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	//初始化 socket dll
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 2);

	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		return false;
	}

	//创建socket
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == m_socket)
	{
		return false;
	}

	//绑定
	if (SOCKET_ERROR == bind(m_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		return false;
	}

	//监听
	if (SOCKET_ERROR == listen(m_socket, 10))
	{
		return false;
	}

	return m_socket;
}

#else

bool SendService::StartUp()
{
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == -1)
	{
		std::cout << "Error: socket" << std::endl;
		return false;
	}

	if (bind(m_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cout << "Error: bind" << std::endl;
		return false;
	}

	if (listen(m_socket, 5) == -1)
	{
		std::cout << "Error: listen" << std::endl;
		return false;
	}

	return true;
}

#endif

void SendService::DoSendData(int c_socket, std::vector<std::string>&filepath)
{
	std::string ip;
	int port;
	get_peer_ip_port(c_socket,ip,port);
	//printf("now pid is %d", GetCurrentProcessId());
	//printf("now tid is %d \n", GetCurrentThreadId());
	for (auto it : filepath)
	{
		uint64_t filesize = getFileSize((char*)it.c_str());
		std::cout << "file size :" << filesize<<" K " << std::endl;
		uint64_t filesize2 = filesize;

		char buffer[SBUFFER_SIZE];
		std::cout << it << std::endl;
		FILE *fp = fopen(it.c_str(), "rb");
		if (fp == nullptr)
		{ 
			std::cout << "error can not find "<< it << std::endl;
			return;
		}
		std::cout << "debug3" << std::endl;
		memset(buffer, 0, SBUFFER_SIZE);
		int length = 0;
		int index = 0;
		
		std::vector<std::string> v;
#if defined(_WIN32)
		SplitString(it, v, "\\");
#else
		SplitString(it, v, "/");
#endif
		if (v.empty())
		{
			std::cout << "Error PathName : " << it << std::endl;
			return;
		}

		std::cout << "start send : " << it << std::endl;

		while ((length = fread(buffer, sizeof(char), SBUFFER_SIZE, fp)) > 0)
		{
			filesize -= length;

			NetPacket sendPack;
			sendPack.Header.wOpcode = SENDDATA;
			sendPack.Header.wDataSize = length + sizeof(NetPacketHeader);
			sendPack.Header.wOrderindex = index;
			sendPack.Header.filesize = filesize2;
			memcpy(sendPack.Header.filename, v[v.size()-1].c_str(), MAXNAME);

			memcpy(sendPack.Data, buffer, length);
			if (filesize == 0)
			{
				std::cout << "send last char  " << std::endl;

				sendPack.Header.tail = 1;
			}

			int sendlen = send(c_socket, (const char*)&sendPack, sizeof(sendPack), 0);

			if (sendlen == -1)
			{
				fclose(fp);
				CloseSocket(c_socket);
				std::cout << "socket close " << std::endl;
				return;
			}

			while (sendlen < length)
			{
				sendlen = send(c_socket, ((const char*)&sendPack) + sendlen, sizeof(sendPack) - sendlen, 0);
			    if (sendlen == -1)
			    {
				    fclose(fp);
					CloseSocket(c_socket);
				    std::cout << "socket close " << std::endl;
				    return;
			    }

				length -= sendlen;
				std::cout << "buff -- " <<sendlen <<" length : "<<length << std::endl;
			}

			memset(buffer, 0, SBUFFER_SIZE);
			index++;
		}

		std::cout <<"MD5 : "<< get_file_md5(it) << std::endl;
		fclose(fp);
		std::cout << it << " Transfer Successful !" << std::endl;
	}

	CloseSocket(c_socket);
}


void SendService::Doaccept(std::vector<std::string>&filepath)
{
	while (true)
	{
		std::cout << "Listening To Client ..." << std::endl;
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int m_new_socket = accept(m_socket, (struct sockaddr *)&client_addr, &client_addr_len);
		if (m_new_socket < 0)
		{
			std::cout << "SOCKET_ERROR ..." << std::endl;
			continue;
		}
		ClientScoketVec.push_back(m_new_socket);
		std::cout << "New Client Connection" << std::endl;
		std::thread T(std::bind(&SendService::DoSendData, this,m_new_socket,filepath));
		T.detach();
	}
}

void SendService::Start()
{
	std::vector<std::string>m_vtFileList;

#if defined(_WIN32)
	/*
	std::string workSpacePath = GetCurrentExeDir();
	std::string sqlPath = workSpacePath + "\\src";
	std::cout << "sqlPath : " << sqlPath << std::endl;
	get_all_files(sqlPath,m_vtFileList);
	*/
	get_all_files("src", m_vtFileList);
#else
	get_all_files("src", m_vtFileList);
#endif
	for(auto it : m_vtFileList)
	{
		std::cout<<it<<std::endl;
	}
	
	
	if(!StartUp() || m_socket < 0)
	{
		std::cout << "SOCKET_ERROR " << std::endl;
		return;
	}

	std::thread taccept(std::bind(&SendService::Doaccept, this, m_vtFileList));
	taccept.join();
	std::cout << "ThreadPool over" << std::endl;
	close(m_socket);
}