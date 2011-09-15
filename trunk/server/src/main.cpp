#include <enet/enet.h>
#include <iostream>
#include "ICDNetwork.h"
#include <string>
#include <string.h>

#define PORT 9000

ENetHost* server;
ENetAddress address;
int peerNumber = 0;

void handleConvert9To10Command(ICDCommandPacket* packet, ENetPeer* peer)
{
	//TODO: Jeff: We would add calls to the database here
	// Since we know this is a convert 9 to 10 packet, we know what the payload is
	// The payload is a string, and the length of that payload is the string length
	// Therefore, we can just do a strncpy and stuff it into a c-string

	char cstr[packet->getArgLen()];
	memset(cstr, 0, packet->getArgLen()); // Clear out the memory, just in case :)
	strncpy(cstr, (char*)packet->getArgs(), packet->getArgLen());

	// Now that we have the string, we can do as we wish with it.
	// For the demo purposes, we are just going to return what we got.
	// In the actual implementation, we would make calls to the database and stuff
	ICDResponsePacket* resp = new ICDResponsePacket(ICD_RESPONSE_CONVERT_9_TO_10);
	resp->setData(&cstr, packet->getArgLen()); // This could be done in the ctor, but I've done this as an example

	sendPacket(resp, peer);
	delete resp;
}

void handlePacket(ENetPacket* p, ENetPeer* peer)
{
	ICDPacket* packet = ICDPacket::createPacketFromBuffer(p->data);

	switch(packet->getType())
	{
		case ICD_PACKET_TYPE_COMMAND:
			{
				ICDCommandPacket* commandPacket = (ICDCommandPacket*)packet;
				if(commandPacket->getCommandType() == ICD_COMMAND_CONVERT_9_TO_10)
				{
					handleConvert9To10Command(commandPacket, peer);
				}
			}
			break;
		case ICD_PACKET_TYPE_RESPONSE:
			break;
		default:
			std::cerr << "ERROR!! Invalid packet type encountered!" << std::endl;
			break;
	}
}

void loop()
{
	ENetEvent event;

	while(true)
	{
		while(enet_host_service(server, &event, 1) > 0)
		{
			switch(event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "Client connected" << std::endl;
					
					event.peer->data = (void*)peerNumber;
					peerNumber++;
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					handlePacket(event.packet, event.peer);
					enet_packet_destroy(event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << event.peer->data << " disconnected" << std::endl;
					// Reset client's information
					event.peer->data = NULL;
					break;
				case ENET_EVENT_TYPE_NONE:
					break;
			}
		}
	}
}

int main()
{
	// Initialize ENet
	if (enet_initialize() != 0) {
		std::cout << "enet did not initialize" << std::endl;
		exit(EXIT_FAILURE);
	}

	address.host = ENET_HOST_ANY;
	address.port = PORT;

	server = enet_host_create(&address, 32, 2, 0, 0);

	if (server == NULL) {
		std::cout << "server is null.... that is bad" << std::endl;
		exit(EXIT_FAILURE);
	}

	atexit(enet_deinitialize);

	loop();

	// End of program
	enet_host_destroy(server);
}
