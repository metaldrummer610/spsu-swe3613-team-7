/**
* ICD Converter Server
*
* \author Robbie Diaz
* \author Jeff Lett
*
* Here's the DB schema:
* CREATE TABLE icd_9_descriptions (
* 	icd_9_code VARCHAR(10) PRIMARY KEY, 
* 	icd_9_description VARCHAR(500) DEFAULT 'No Description Found'
* );
*
* CREATE TABLE icd_10_cm_descriptions (
*	icd_10_code VARCHAR(10) PRIMARY KEY,
*	icd_10_description VARCHAR(500) DEFAULT 'No Description Found' 
* );
*
* CREATE TABLE cm_gems (
*	icd_9_code VARCHAR(10) REFERENCES icd_9_descriptions,
*	icd_10_code VARCHAR(10) REFERENCES icd_10_cm_descriptions,
*	flags CHAR(5),
*	PRIMARY KEY (icd_9_code, icd_10_code, flags)
*);
*
* CREATE TABLE dx_codes (
*	icd_10_code VARCHAR(10) REFERENCES icd_10_cm_descriptions,
*	dx_code VARCHAR(10) PRIMARY KEY
* );
*
*/

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <enet/enet.h>

#include <iostream>
#include "ICDNetwork.h"
#include <string>
#include <string.h>
#include <sstream>
#include <pqxx/pqxx>
#include <string.h>

#include <boost/serialization/export.hpp>

BOOST_SERIALIZATION_ASSUME_ABSTRACT(ICDPacket)
BOOST_CLASS_EXPORT(ICDResponsePacket)
BOOST_CLASS_EXPORT(ICDCommandPacket)
BOOST_CLASS_EXPORT(ICDCommandConvert9To10)
BOOST_CLASS_EXPORT(ICDResponseConvert9To10)
BOOST_CLASS_EXPORT(ICDCommandGetICD9Code)
BOOST_CLASS_EXPORT(ICDResponseGetICD9Code)
BOOST_CLASS_EXPORT(ICDCommandGetICD10Code)
BOOST_CLASS_EXPORT(ICDResponseGetICD10Code)
BOOST_CLASS_EXPORT(ICDCommandGetDXCode)
BOOST_CLASS_EXPORT(ICDResponseGetDXCode)
BOOST_CLASS_EXPORT(ICDCommandCreateDXCode)
BOOST_CLASS_EXPORT(ICDCommandGetDXCodes)
BOOST_CLASS_EXPORT(ICDResponseGetDXCodes)

using namespace pqxx;

/// Jeff's debugging macro.  Change me to 0 to remove his garbage output
#define DEBUG 0
#define PORT 9000

///Limits the number of results per query (for blank queries, "the", etc)
#define MAX_RESULTS 50
#define MAX_DX_RESULTS 50

ENetHost* server;
ENetAddress address;
int peerNumber = 0;

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
* Inserts a new entry into the DX table
* \param dx_code The dx_code to be entered
* \param icd_10_code The icd_10_code to be entered
*/
void insertDxCode(connection *c, std::string dx_code, std::string icd_10_code) {
	std::string query = "INSERT INTO dx_codes VALUES (upper('" + icd_10_code + "'),'" + dx_code + "')";
	runQuery(c,query);
}

/**
* Prints a result object
* \param r The result to print
*/
void printResults(result r) {
	int i=0;
	for(result::const_iterator row=r.begin();row!=r.end() && i<MAX_RESULTS;++row) {
		std::cout << "[";
   	for(result::tuple::const_iterator field=row->begin();field!=row->end();++field) {
      	std::cout << field->c_str();
			if(field!=row->end()-1)
				std::cout << ",";		
		}
		++i;
		std::cout << "]" << std::endl;
	}
	std::cout << "Results:" << r.size() << std::endl;
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
		connection *c = new connection("host=localhost dbname=swe3613 user=swe3613 password=swe3613");
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
* \return A query to search 9 descriptionINSERT INTO dx_codes VALUES (upper('V222'),'Accidental')s
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
* Gets a query to search dx codes
* \param cstr The char* to search for
* \return A query to search dx codes
*/
char* getDxCodeQuery(char* cstr) {
	char *first_half = (char*)"select g.icd_9_code, i_9.icd_9_description, g.icd_10_code, i_10.icd_10_description, g.flags FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code JOIN dx_codes dx ON dx.icd_10_code = i_10.icd_10_code where upper(dx.dx_code) LIKE upper('%";
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
std::vector<ICDCode*> processResults(result& r, std::vector<ICDCode*>& v, CodeType type) {
	std::cout << "Processing Results v.size()=" << v.size() << std::endl;		
	int j=0;		
	for(result::const_iterator row = r.begin(); row != r.end() && j<MAX_RESULTS; ++row, ++j) {
		if(true) {
			std::cout << "description=" << row[3] << std::endl;	
		}
		ICDCode* code = new ICDCode(type, row[2].c_str(), row[3].c_str(), row[4].c_str());
		
		v.push_back(code);
	}
	return v;
}

/**
* Processes results into a new vector
* \param r The results to process
* \return A new vector of Basecodes
*/
std::vector<ICDCode*> processResults(result r, CodeType type) {
	std::vector<ICDCode*> v;
	v = processResults(r, v, type);
	//printResults(r);
	return v;
}

/** 
* Runs the queries
* \cstr The char* to be searched
* \result The result
*/
std::vector<ICDCode*> handleQuery(char* cstr) {
	connection *c = connectToDatabase();
	char* nineCodeQuery = get9CodeQuery(cstr);
	char* nineDescQuery = get9DescQuery(cstr);
	char* tenCodeQuery = get10CodeQuery(cstr);
	char* tenDescQuery = get10DescQuery(cstr);
	char* dxQuery = getDxCodeQuery(cstr);	
	if(DEBUG) {
		std::cout << "nineCodeQuery=" << nineCodeQuery << std::endl;
		std::cout << "nineDescQuery=" << nineDescQuery << std::endl;
		std::cout << "tenCodeQuery=" << tenCodeQuery << std::endl;
		std::cout << "tenDescQuery=" << tenDescQuery << std::endl;
	}
	//LOG("dxquery=");
	//LOG(dxQuery);

	result r;

	r = runQuery(c,dxQuery);
	if(DEBUG)
		printResults(r);
	std::vector<ICDCode*> v = processResults(r, CodeType::ICD10);
	
	r = runQuery(c,nineCodeQuery);
	if(DEBUG)
		printResults(r);
	v = processResults(r, v, CodeType::ICD10);

	r = runQuery(c,nineDescQuery);
	if(DEBUG)
		printResults(r);
	v = processResults(r, v, CodeType::ICD10);

	r = runQuery(c,tenCodeQuery);
	if(DEBUG)
		printResults(r);
	v = processResults(r, v, CodeType::ICD10);
	
	r = runQuery(c,tenDescQuery);
	if(DEBUG)
		printResults(r);
	v = processResults(r, v, CodeType::ICD10);

	disconnect(c);
	return v;
}

void handleConvert9To10Command(ICDCommandConvert9To10* packet, ENetPeer* peer)
{
	std::vector<ICDCode*> codes = handleQuery((char*)packet->getCode().c_str());

	ICDResponseConvert9To10* respConvert = new ICDResponseConvert9To10(codes);
	ICDResponsePacket* resp = new ICDResponsePacket(respConvert);
	
	sendPacket(resp, peer);
}

void handleGetICD9CodeCommand(ICDCommandGetICD9Code* packet, ENetPeer* peer)
{
/*	ICDCode* code = excuteQuery(packet->getCode());

	ICDResponseGetICD9Code* responseGet = new ICDResponseGetICD9Code(code);
	ICDResponsePacket* resp = new ICDResponsePacket(respGet);

	sendPacket(resp, peer);*/
}

void handleGetICD10CodeCommand(ICDCommandGetICD10Code* packet, ENetPeer* peer)
{
}

void handleGetDXCodeCommand(ICDCommandGetDXCode* packet, ENetPeer* peer)
{
	connection *c=connectToDatabase();
	
	std::string query="select d.dx_code,ten.icd_10_description, ten.icd_10_code FROM dx_codes d JOIN icd_10_cm_descriptions ten ON (d.icd_10_code=ten.icd_10_code)";
	result r=runQuery(c,query);
	
	std::string code, description, dx_code;
	for(result::const_iterator row2=r.begin();row2!=r.end();++row2) {
		dx_code=row2[0].c_str();
		description=row2[1].c_str();
		code=row2[2].c_str();
	}

  	ICDCode* returnCode = new ICDCode(CodeType::ICD10,code,description,"00000");
	DXCode* dxCode = new DXCode(dx_code, returnCode);
	
	ICDResponseGetDXCode* response = new ICDResponseGetDXCode(dxCode);
	ICDResponsePacket* resp = new ICDResponsePacket(response);
	
	sendPacket(resp, peer);
	
	disconnect(c);

	delete dxCode;
	delete returnCode;
	delete response;
	delete resp;
}


void handleCreateDXCodeCommand(ICDCommandCreateDXCode* packet, ENetPeer* peer)
{
	connection* c = connectToDatabase();

	insertDxCode(c, packet->getDXCode(), packet->getICD10Code());

	disconnect(c);
}

void handleGetDXCodesCommand(ICDCommandGetDXCodes* packet, ENetPeer* peer)
{
	connection *c=connectToDatabase();
	std::string query = "select d.dx_code,ten.icd_10_description, ten.icd_10_code FROM dx_codes d JOIN icd_10_cm_descriptions ten ON (d.icd_10_code=ten.icd_10_code)";
//	std::cout << "query=" << query << std::endl;
	result r=runQuery(c,query);
	int i=0;		
	
	std::vector<DXCode*> codes;	
	for(result::const_iterator row = r.begin(); row != r.end() && i<MAX_DX_RESULTS; ++row, ++i) {
		ICDCode* newCode = new ICDCode(CodeType::ICD10,row[2].c_str(),row[1].c_str(),"00000");
		DXCode* dxCode = new DXCode(row[0].c_str(), newCode);
		
		codes.push_back(dxCode);
	}
	
	ICDResponseGetDXCodes* response = new ICDResponseGetDXCodes(codes);
	ICDResponsePacket *resp = new ICDResponsePacket(response);

	sendPacket(resp, peer);
	
	disconnect(c);
}

void handlePacket(ENetPacket* p, ENetPeer* peer)
{
	LOG("about to get the packet");
	ICDPacket* packet = getPacket(p);
	LOG("Got the packet");

	switch(packet->getType())
	{
		case PacketType::Command:
			{
				ICDCommandPacket* commandPacket = (ICDCommandPacket*)packet;
				ICDCommand* command = commandPacket->getCommand();
				
				if(command->getCommandType() == CommandType::Convert9To10)
				{
					handleConvert9To10Command((ICDCommandConvert9To10*)command, peer);
					LOG("Handled. Hyaaa! Hyaaa Falkor! Onward!!11?eleven"); // lol
				}
				else if(command->getCommandType() == CommandType::GetICD9Code)
				{
					handleGetICD9CodeCommand((ICDCommandGetICD9Code*)command, peer);
				}
				else if(command->getCommandType() == CommandType::GetICD10Code)
				{
					handleGetICD10CodeCommand((ICDCommandGetICD10Code*)command, peer);
				}
				else if(command->getCommandType() == CommandType::GetDXCode)
				{
					handleGetDXCodeCommand((ICDCommandGetDXCode*)command, peer);
				}
				else if(command->getCommandType() == CommandType::CreateDXCode)
				{
					handleCreateDXCodeCommand((ICDCommandCreateDXCode*)command, peer);
				}
				else if(command->getCommandType() == CommandType::GetDXCodes)
				{
					handleGetDXCodesCommand((ICDCommandGetDXCodes*)command, peer);
				}
			}
			break;
		default:
			break;
	}

	delete packet;
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
	if(enet_initialize() != 0)
	{
		std::cout << "enet did not initialize" << std::endl;
		exit(EXIT_FAILURE);
	}

	address.host = ENET_HOST_ANY;
	address.port = PORT;

	server = enet_host_create(&address, 32, 2, 0, 0);

	if(server == NULL)
	{
		std::cout << "server is null.... that is bad" << std::endl;
		exit(EXIT_FAILURE);
	}

	atexit(enet_deinitialize);

	loop();

	// End of program
	enet_host_destroy(server);

	return 0;
}
