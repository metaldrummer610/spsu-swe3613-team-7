#include "ICDNetworkHelpers.h"
#include <string.h>
#include <iostream>
#include "Utils.h"

void sendPacket(ICDPacket* packet, ENetPeer* p)
{
	char* buf = (char*)packet->toBuffer();
	int size = 0;
	memcpy(&size, buf, sizeof(int));
	size += sizeof(int);

	std::cout << "Dumping the packet" << std::endl;
	dumpBuffer((const char*)buf, size);

	ENetPacket* packetToSend = enet_packet_create(buf, size + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(p, 0, packetToSend);
	delete buf;
}

void sendPacket(ENetPacket* packet, ENetPeer* p)
{
	enet_peer_send(p, 0, packet);
}
