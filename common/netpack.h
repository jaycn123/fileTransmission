#pragma once

#define NET_PACKET_DATA_SIZE 1024
#define NET_CODE 0x2766
#define MAXNAME 512

enum NetMessage
{
	SENDDATA = 1,

};

struct  NetPacketHeader
{
	unsigned short      wDataSize;  ///< ���ݰ���С���������ͷ�ͷ�����ݴ�С
	NetMessage          wOpcode;    ///< ������
	unsigned short      wCode = NET_CODE;
	long long           wOrderindex = 0;
	char                filename[MAXNAME] = { 0 };
	int                 tail = 0;
	long long           filesize = 0;
};
/// �������ݰ�
struct NetPacket
{
	NetPacketHeader     Header;                         ///< ��ͷ
	char       Data[NET_PACKET_DATA_SIZE];     ///< ����
};

