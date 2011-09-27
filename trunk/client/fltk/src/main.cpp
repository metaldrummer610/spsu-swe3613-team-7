#include <FL/FL.h>
#include <FL/FL_Window.h>
#include <FL/FL_Box.h>
#include <FL/FL_Button.h>
#include <FL/FL_Input.h>
#include <FL/FL_Select_Browser.h>

#define HAVE_PTHREAD_H 1
#include "threads.h" // FLTK Threads Support

#include <enet/enet.h>
#include <ICDNetwork.h>
#include <iostream>

Fl_Select_Browser* codeList;
Fl_Thread enetThread;
bool enetThreadRunning = false;
ENetHost* client;
ENetPeer* peer;
ENetAddress address;
ENetEvent event;

void submitButtonClick(Fl_Widget* widget, void* ptr)
{
	std::cout << "Click!" << std::endl;
	Fl_Input* codeInputBox = (Fl_Input*)ptr;
	std::cout << "Input: " << codeInputBox->value() << std::endl;
	std::cout << "len: " << codeInputBox->size() << std::endl;
	std::cout << "sf: " << sizeof(char) << std::endl;

	int len = codeInputBox->size();

	void* str = new char[len];
	memset(str, 0, len);
	memcpy(str, codeInputBox->value(), len);

	// Let the magic begin...
	ICDCommandPacket* command = new ICDCommandPacket(ICD_COMMAND_CONVERT_9_TO_10, str, len);
	sendPacket(command, peer);

	delete command;
}

void rowClickedCallback(Fl_Widget* widget)
{
	std::cout << "row clicked" << std::endl;
	Fl_Select_Browser* browser = (Fl_Select_Browser*)widget;
	int line = browser->value();
	if(line != 0)
	{
			const char* text = browser->text(line);
			std::cout << "Selected line: " << text << std::endl;
	}
}

void initEnet()
{
	if(enet_initialize() != 0)
	{
		std::cout << "ERROR!!!! Enet failed to initialize!!!" << std::endl;
		exit(EXIT_FAILURE);
	}

	client = enet_host_create(NULL, 1, 0, 0, 0);

	if(client == NULL)
	{
		std::cout << "ERROR!!!! Failed to create the host!!!" << std::endl;
		exit(EXIT_FAILURE);
	}

	enet_address_set_host(&address, "10.0.1.7");
	address.port = 9000;

	peer = enet_host_connect(client, &address, 2, 0);

	if(peer == NULL)
	{
		std::cout << "ERROR!!!! Failed to connect to icechamps!" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void destroyEnet()
{
	enetThreadRunning = false;
	enet_peer_disconnect(peer, 0);

	while(enet_host_service(client, &event, 3000) > 0)
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_DISCONNECT:
				std::cout << event.peer->data << " disconnected" << std::endl;
				event.peer->data = NULL;
				break;
			default:
				break;
		}
	}

	enet_host_destroy(client);
}

void handleConvert9To10Response(ICDResponsePacket* packet)
{
	dumpBuffer((const char*)packet->getData(), packet->getDataLen());
	// The payload of one of these packets is a code list
	// A code list is basically a vector of BaseCode*'s
	std::vector<BaseCode*> codes = bufferToCodeList(packet->getData());

	Fl::lock();
	codeList->clear();
	for(std::vector<BaseCode*>::iterator it = codes.begin(); it != codes.end(); it++)
	{
		std::cout << "Got a code!: " << (*it) << std::endl;
		codeList->add((*it)->getCode());
	}
	Fl::unlock();
	Fl::awake(packet);

	for(std::vector<BaseCode*>::iterator it = codes.begin(); it != codes.end(); it++)
	{
		delete (*it);
		(*it) = NULL;
	}

	codes.clear();
}

void handlePacket(ENetPacket* p)
{
	std::cout << "ZOMG WE GOT A PACKET1!!1!!!" << std::endl;
	ICDPacket* packet = ICDPacket::createPacketFromBuffer(p->data);

	switch(packet->getType())
	{
		case ICD_PACKET_TYPE_COMMAND:
		{
			ICDCommandPacket* commandPacket = (ICDCommandPacket*)packet;
			break;
		}
		case ICD_PACKET_TYPE_RESPONSE:
		{
			ICDResponsePacket* response = (ICDResponsePacket*)packet;
			if(response->getResponseType() == ICD_RESPONSE_CONVERT_9_TO_10)
			{
				handleConvert9To10Response(response);
			}
		}
	}
}

void* enetMain(void* p)
{
	enetThreadRunning = true;
	while(enetThreadRunning)
	{
		while(enet_host_service(client, &event, 1) > 0)
		{
			switch(event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					event.peer->data = (void*)"Client Information"; // Probably should change that at some point...
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					handlePacket(event.packet);
					enet_packet_destroy(event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << event.peer->data << " disconnected." << std::endl;
					event.peer->data = NULL;
					break;
				case ENET_EVENT_TYPE_NONE:
					break;
			}
		}
	}

	return 0;
}

int main(int argc, char** argv)
{
	// Start up the network connection...
	std::cout << "About to init enet" << std::endl;
	initEnet();
	atexit(destroyEnet);

	Fl_Window* window = new Fl_Window(340, 180, "ICD Conversion Application");
	Fl_Input* codeInputBox = new Fl_Input(15, 15, 300, 20);

	Fl_Button* submitButton = new Fl_Button(240, 40, 75, 20, "Submit");
	submitButton->callback(&submitButtonClick, codeInputBox);

	codeList = new Fl_Select_Browser(15, 60, 300, 100);
	codeList->callback(&rowClickedCallback);
	window->end();
	window->show(argc, argv);

	Fl::lock();

	fl_create_thread(enetThread, enetMain, NULL);
	return Fl::run();
}
