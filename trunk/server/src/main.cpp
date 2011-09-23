#include <enet/enet.h>
#include <iostream>
#include "ICDNetwork.h"
#include <string>
#include <string.h>
#include <pqxx/pqxx>
#include "BaseCode.h"

using namespace pqxx;

//Hi.  I'm Jeff's debugging macro.  Change me to 0 to remove his garbage output.
#define DEBUG 1
#define PORT 9000


ENetHost* server;
ENetAddress address;
int peerNumber = 0;

void printResults(result r) {
	for(result::const_iterator row=r.begin();row!=r.end();++row) {
		std::cout << "[";
   	for(result::tuple::const_iterator field=row->begin();field!=row->end();++field) {
      	std::cout << field->c_str();
			if(field!=row->end()-1)
				std::cout << ",";		
		}
		std::cout << "]" << std::endl;
	}
	std::cout << "Results:" << r.size() << std::endl;
}

void testQuery(connection *c, std::string query) {
	try {
		work *w = new work(*c);
		if(DEBUG)
			std::cout << "Transaction created.  Yeeeeeeeah!!1eleven" << std::endl;
		result r = w->exec(query);
     		w->commit();
		if(DEBUG)
			printResults(r);
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void disconnect(connection *c) {
	c->disconnect();	
}

connection* connectToDatabase() {
	try {
		//Make connection
		connection *c = new connection("host=localhost dbname=swe3613 user=jeff password=swe3613");
		if(DEBUG)
			std::cout << "Connected to swe3613@localhost." << std::endl;
		return c;
	}
   catch (const std::exception &e) {
   	std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
   }
}

char* constructQuery(char* cstr) {
	char *first_half = (char*)"select g.icd_9_code, i_9.icd_9_description, g.icd_10_code, i_10.icd_10_description, g.flags FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code where g.icd_9_code = '";
	/*
CREATE TABLE icd_9_descriptions (
	icd_9_code VARCHAR(10) PRIMARY KEY, 
	icd_9_description VARCHAR(500) DEFAULT 'No Description Found'
);

CREATE TABLE icd_10_cm_descriptions (
	icd_10_code VARCHAR(10) PRIMARY KEY,
	icd_10_description VARCHAR(500) DEFAULT 'No Description Found' 
);

CREATE TABLE cm_gems (
	icd_9_code VARCHAR(10) REFERENCES icd_9_descriptions,
	icd_10_code VARCHAR(10) REFERENCES icd_10_cm_descriptions,
	flags CHAR(5),
	PRIMARY KEY (icd_9_code, icd_10_code, flags)
);
	
	
	*/
	char *query = new char[strlen(first_half)+strlen(cstr)+2];
	memset(query, 0, strlen(first_half)+strlen(cstr)+2);	
	strncpy(query,first_half,strlen(first_half));
	strcat(query,cstr);
	strcat(query,"'");
	return query;
}

void handleConvert9To10Command(ICDCommandPacket* packet, ENetPeer* peer)
{

	//TODO: Jeff: We would add calls to the database here
	// Since we know this is a convert 9 to 10 packet, we know what the payload is
	// The payload is a string, and the length of that payload is the string length
	// Therefore, we can just do a strncpy and stuff it into a c-string

	char cstr[packet->getArgLen()];
	memset(cstr, 0, packet->getArgLen()); // Clear out the memory, just in case :)
	strncpy(cstr, (char*)packet->getArgs(), packet->getArgLen());

	//Connect to the Database. 
	connection *c = connectToDatabase();
	
	char* query = constructQuery(cstr);	
	
	if(DEBUG)	
		std::cout << "query=" << query << std::endl;

	if(DEBUG)
		testQuery(c,query);
	
	disconnect(c);	
	

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
		

	//Connect to the Database. 
	connection *c = connectToDatabase();
	char *cstr = (char*)"6259";
	
	char *query = constructQuery(cstr);
	if(DEBUG)
		testQuery(c,query);
	disconnect(c);	
	

	atexit(enet_deinitialize);

	loop();

	// End of program
	enet_host_destroy(server);
}