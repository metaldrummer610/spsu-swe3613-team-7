#include "ICDNetworkHelpers.h"
#include <string.h>
#include <iostream>
#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ICDPacket.h"

void sendPacket(ICDPacket* packet, ENetPeer* p)
{
	std::stringstream ss;
	boost::archive::text_oarchive archive(ss);
	archive << packet;

	int size = ss.str().size();
	char* buffer = new char[size + sizeof(int)];
	memcpy(buffer, &size, sizeof(int));

	memcpy(buffer + sizeof(int), ss.str().c_str(), size);

	std::cout << "Sending: " << ss.str() << std::endl;
	ENetPacket* packetToSend = enet_packet_create(buffer, size + 1 + sizeof(int), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(p, 0, packetToSend);
	delete buffer;
}

void sendPacket(ENetPacket* packet, ENetPeer* p)
{
	enet_peer_send(p, 0, packet);
}

#define LOG(x) std::cout << x << std::endl;
ICDPacket* getPacket(ENetPacket* p)
{
	LOG("in getPacket");
	int size = 0;
	memcpy(&size, p->data, sizeof(int));

	LOG("Got the size");

	char* data = new char[size];
	memcpy(data, p->data + sizeof(int), size);
	
	LOG("Got the data");

	std::string str;
	str += data;

	std::cout << "Data: " << str << "." << std::endl;
	
	LOG("Data added to string");

	std::stringstream ss(str);
	ICDPacket* packet;

	LOG("String in stream");

	boost::archive::text_iarchive archive(ss);
	LOG("Archive fed?");
	archive >> packet;
	LOG("packet filled");

	delete data;
	LOG("Deleted data");
	return packet;
}
