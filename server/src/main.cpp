/**
* ICD Converter Server
*
* \author Robbie Diaz
* \author Jeff Lett
*
* Here's the DB schema:
* CREATE TABLE icd_9_descriptions (
*	 icd_9_code VARCHAR(10) PRIMARY KEY, 
*	 icd_9_description VARCHAR(500) DEFAULT 'No Description Found'
* );
*
* CREATE TABLE icd_10_cm_descriptions (
*	 icd_10_code VARCHAR(10) PRIMARY KEY,
*	 icd_10_description VARCHAR(500) DEFAULT 'No Description Found' 
* );
*
* CREATE TABLE cm_gems (
*	 icd_9_code VARCHAR(10) REFERENCES icd_9_descriptions,
*	 icd_10_code VARCHAR(10) REFERENCES icd_10_cm_descriptions,
*	 flags CHAR(5),
* 	 PRIMARY KEY (icd_9_code, icd_10_code, flags)
* );	
*
*/

#include <enet/enet.h>
#include <iostream>
#include "ICDNetwork.h"
#include <string>
#include <string.h>
#include <pqxx/pqxx>

using namespace pqxx;

/// Jeff's debugging macro.  Change me to 0 to remove his garbage output
#define DEBUG 0
#define PORT 9000

ENetHost* server;
ENetAddress address;
int peerNumber = 0;

/**
* Prints a result object
* \param r The result to print
*/
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

/**
* Runs a query
* \param c The connection to use
* \param query The query to search
* \return The result of the search
*/
result runQuery(connection *c, std::string query) {
	result r;	
	try {
		work *w = new work(*c);
		r = w->exec(query);
     		w->commit();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	return r;
}

/**
* Disconnects a connection
* \param c The connection to close
*/
void disconnect(connection *c) {
	c->disconnect();	
}

/**
* Connects to a database
* \return The connection that was established
*/
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

/**
* Gets a query to search 9 codes
* \param cstr The char* to search for
* \return A query to search 9 codes
*/
char* get9CodeQuery(char* cstr) {
	char *first_half = (char*)"select g.icd_9_code, i_9.icd_9_description, g.icd_10_code, i_10.icd_10_description, g.flags FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code where g.icd_9_code = upper('";
	char *query = new char[strlen(first_half)+strlen(cstr)+3];
	memset(query, 0, strlen(first_half)+strlen(cstr)+2);	
	strncpy(query,first_half,strlen(first_half));
	strcat(query,cstr);
	strcat(query,"')");
	return query;
}

/**
* Gets a query to search 9 descriptions
* \param cstr The char* to search for
* \return A query to search 9 descriptions
*/
char* get9DescQuery(char* cstr) {
	char *first_half = (char*)"select g.icd_9_code, i_9.icd_9_description, g.icd_10_code, i_10.icd_10_description, g.flags FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code where upper(i_9.icd_9_description) LIKE upper('%";
	char *query = new char[strlen(first_half)+strlen(cstr)+4];
	memset(query, 0, strlen(first_half)+strlen(cstr)+3);	
	strncpy(query,first_half,strlen(first_half));
	strcat(query,cstr);
	strcat(query,"%')");
	return query;
}

/**
* Gets a query to search 10 codes
* \param cstr The char* to search for
* \return A query to search 10 codes
*/
char* get10CodeQuery(char* cstr) {
	char *first_half = (char*)"select g.icd_9_code, i_9.icd_9_description, g.icd_10_code, i_10.icd_10_description, g.flags FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code where i_10.icd_10_code = upper('";
	char *query = new char[strlen(first_half)+strlen(cstr)+3];
	memset(query, 0, strlen(first_half)+strlen(cstr)+2);	
	strncpy(query,first_half,strlen(first_half));
	strcat(query,cstr);
	strcat(query,"')");
	return query;
}

/**
* Gets a query to search 10 decriptions
* \param cstr The char* to search for
* \return A query to search 10 descriptions
*/
char* get10DescQuery(char* cstr) {
	char *first_half = (char*)"select g.icd_9_code, i_9.icd_9_description, g.icd_10_code, i_10.icd_10_description, g.flags FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code where upper(i_10.icd_10_description) LIKE upper('%";
	char *query = new char[strlen(first_half)+strlen(cstr)+4];
	memset(query, 0, strlen(first_half)+strlen(cstr)+3);	
	strncpy(query,first_half,strlen(first_half));
	strcat(query,cstr);
	strcat(query,"%')");
	return query;
}

/**
* Processes results into a supplied vector
* \param r The results to process
* \param v the vector to put the results in
* \return A vector of Basecodes
*/
std::vector<BaseCode*> processResults(result r,std::vector<BaseCode*> v) {
	for(result::const_iterator row=r.begin();row!=r.end();++row) {
		BaseCode* code = new BaseCode(intToCode(0),(char*)row[2].c_str(),(int)strlen(row[2].c_str()),(char*)row[3].c_str(),(int)strlen(row[3].c_str()),(char*)row[4].c_str(),(int)strlen(row[4].c_str()));
		v.push_back(code);
	}
	return v;
}

/**
* Processes results into a new vector
* \param r The results to process
* \return A new vector of Basecodes
*/
std::vector<BaseCode*> processResults(result r) {
	std::vector<BaseCode*> v;
	v=processResults(r,v);
	return v;
}


/** 
* Runs the queries
* \cstr The char* to be searched
* \result The result
*/
std::vector<BaseCode*> handleQuery(char* cstr) {
	connection *c = connectToDatabase();
	char* nineCodeQuery = get9CodeQuery(cstr);		
	char* nineDescQuery = get9DescQuery(cstr);		
	char* tenCodeQuery = get10CodeQuery(cstr);		
	char* tenDescQuery = get10DescQuery(cstr);		
	if(DEBUG) {
		std::cout << "nineCodeQuery=" << nineCodeQuery << std::endl;
		std::cout << "nineDescQuery=" << nineDescQuery << std::endl;
		std::cout << "tenCodeQuery=" << tenCodeQuery << std::endl;
		std::cout << "tenDescQuery=" << tenDescQuery << std::endl;
	}	
	result r = runQuery(c,nineCodeQuery);
	if(DEBUG)
		printResults(r);
	std::vector<BaseCode*> v = processResults(r);	

	r = runQuery(c,nineDescQuery);
	if(DEBUG)
		printResults(r);
	v = processResults(r,v);	

	r = runQuery(c,tenCodeQuery);
	if(DEBUG)
		printResults(r);
	v = processResults(r,v);	
	
	r = runQuery(c,tenDescQuery);
	if(DEBUG)
		printResults(r);
	v = processResults(r,v);	

	disconnect(c);	
	return v;	

}


void handleConvert9To10Command(ICDCommandPacket* packet, ENetPeer* peer)
{
	//TODO: Jeff: We would add calls to the database here
	// Since we know this is a convert 9 to 10 packet, we know what the payload is
	// The payload is a string, and the length of that payload is the string length
	// Therefore, we can just do a strncpy and stuff it into a c-string

	char cstr[packet->getArgLen() + 1];
	memset(cstr, 0, packet->getArgLen()); // Clear out the memory, just in case :)
	strncpy(cstr, (char*)packet->getArgs(), packet->getArgLen());
	cstr[packet->getArgLen()] = '\0';
	
	//Handle Query
//	result r=handleQuery(cstr);	
	//Process Results
	//std::vector<BaseCode*> v = processResults(r);
	
	std::vector<BaseCode*> v=handleQuery(cstr);	
	

	void* codeBuffer = codeListToBuffer(v);
	int bufferSize = 0;
	memcpy(&bufferSize, codeBuffer, sizeof(int));
	bufferSize += sizeof(int) + sizeof(int);
	std::cout << "Dumping the code buffer" << std::endl;
	dumpBuffer((const char*)codeBuffer, bufferSize);

	ICDResponsePacket* resp = new ICDResponsePacket(ICD_RESPONSE_CONVERT_9_TO_10, codeBuffer, bufferSize);

	sendPacket(resp, peer);
	delete resp;
//	delete (char*)codeBuffer;

	// Delete the vector
	for(std::vector<BaseCode*>::iterator it = v.begin(); it != v.end(); it++)
	{
		delete (*it);
		*it = NULL;
	}

	v.clear();
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
