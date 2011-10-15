#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Bar.H>
#include <stdio.h>
#include <string.h>

#define HAVE_PTHREAD_H 1
#include "threads.h" // FLTK Threads Support

#include <enet/enet.h>
#include <ICDNetwork.h>
#include <iostream>

Fl_Select_Browser* codeList;
Fl_Thread enetThread;
Fl_Menu_Bar* menu;
bool enetThreadRunning = false;
ENetHost* client;
ENetPeer* peer;
ENetAddress address;
ENetEvent event;
int currentSRow;
std::string Header[4];
std::string inputFBox;

std::vector<std::vector<std::string> > data;
std::vector<std::vector<std::string> > defaultV;
std::vector<std::string> recent;

class ICDTable : public Fl_Table_Row
{
protected:
    void draw_cell(TableContext context,  		// table cell drawing
    		   int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
//    void callback(TableContext context, 		// callback for table events
//    		   int R, int C);
public:
    ICDTable(int x, int y, int w, int h, const char *l=0) : Fl_Table_Row(x,y,w,h,l)
	{
		end(); 
	}
    ~ICDTable() { }

	void set_data(int r, int c, std::string l)
	{
		std::string str(l);
		//data[r][c] = str;
	}
};

// Handle drawing all cells in table
void ICDTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
    static char s[40];
    sprintf(s, "%d/%d", R, C);

    switch ( context )
    {
	case CONTEXT_STARTPAGE:
	    fl_font(FL_HELVETICA, 16);
	    return;

	case CONTEXT_ROW_HEADER:
	{
	    sprintf(s, "%d", R);
	    fl_push_clip(X, Y, W, H);
	    {
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, color());
		fl_color(FL_BLACK);
		fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
	    }
	    fl_pop_clip();
	    return;
	}	
	case CONTEXT_COL_HEADER:
	    sprintf(s, "%s", Header[C].c_str());
	    fl_push_clip(X, Y, W, H);
	    {
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, color());
		fl_color(FL_BLACK);
		fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
	    }
	    fl_pop_clip();
	    return;

	case CONTEXT_CELL:
	{
		if(data.size() > 0)
		{
	  		 sprintf(s, "%s", data.at(R).at(C).c_str());
			 //sprintf(s, "%s", "no data");
	  		 fl_push_clip(X, Y, W, H);
	   		 {
	       		 	// BG COLOR
				fl_color( row_selected(R) ? selection_color() : FL_WHITE);
				fl_rectf(X, Y, W, H);

				// TEXT
				fl_color(FL_BLACK);
				fl_draw(s, X, Y, W, H, FL_ALIGN_LEFT);

				// BORDER
				fl_color(FL_LIGHT2); 
				fl_rect(X, Y, W, H);
	    		}
	    		fl_pop_clip();
	    		return;
		 }

		 else
		 {
	  		 sprintf(s, "%s", "No Data");
	  		 fl_push_clip(X, Y, W, H);
	   		 {
	       		 	// BG COLOR
				fl_color( row_selected(R) ? selection_color() : FL_WHITE);
				fl_rectf(X, Y, W, H);

				// TEXT
				fl_color(FL_BLACK);
				fl_draw(s, X, Y, W, H, FL_ALIGN_LEFT);

				// BORDER
				fl_color(FL_LIGHT2); 
				fl_rect(X, Y, W, H);
	    		}
	    		fl_pop_clip();
	    		return;
		 }
	}

	default:
	    return;
    }
}

void table_cb(Fl_Widget* o, void* cdata)
{
    Fl_Table *table = (Fl_Table*)cdata;
    fprintf(stderr, "%s callback: row=%d col=%d, context=%d, event=%d clicks=%d\n",
	(const char*)table->label(),
	(int)table->callback_row(),
	(int)table->callback_col(),
	(int)table->callback_context(),
	(int)Fl::event(),
	(int)Fl::event_clicks());
	currentSRow = (int)(table->callback_row());
}

void testCallback(Fl_Widget* w, void* ptr)
{
	std::cout << menu->text() << std::endl;
}

void fixRecent()
{
	std::vector<std::string> tempV;
	for(std::vector<std::string>::iterator it = recent.begin(); it != recent.end(); it++)
	{
		tempV.push_back((*it));
	}

	recent.clear();

	for(int i = tempV.size()-1; i >= 0; i--)
	{
		recent.push_back(tempV.at(i));
	}
}

ICDTable* ICBMTable; //Just creating the table here for the purpose of being able to access it outside of main

void submitButtonClick(Fl_Widget* widget, void* ptr)
{
	data.clear();
	std::cout << "Click!" << std::endl;
	Fl_Input* codeInputBox = (Fl_Input*)ptr;
	std::cout << "Input: " << codeInputBox->value() << std::endl;
	std::cout << "len: " << codeInputBox->size() << std::endl;
	std::cout << "sf: " << sizeof(char) << std::endl;

	inputFBox = codeInputBox->value();

	recent.push_back(inputFBox);

	fixRecent();
	int index = menu->find_index("Recent");
	if(index != -1) menu->clear_submenu(index);

	for(std::vector<std::string>::iterator it = recent.begin(); it != recent.end(); it++)
	{
		std::string temp = "Recent/"+(*it);
		menu->add(temp.c_str(), 0, testCallback);
	}

	int len = codeInputBox->size();

	void* str = new char[len];
	memset(str, 0, len);
	memcpy(str, codeInputBox->value(), len);

	// Let the magic begin...
	ICDCommandPacket* command = new ICDCommandPacket(ICD_COMMAND_CONVERT_9_TO_10, str, len);
	sendPacket(command, peer);

	delete command;
}

void defaultButtonClick(Fl_Widget* widget)
{
	if(data.size() > 0)
	{
		defaultV.push_back(data.at(currentSRow));
	
		for(int i = 0; i < 4; i++)
		{
			std::cout << Header[i] << ": " << defaultV.at(0).at(i) << std::endl;
		}
	}
	else
	{
		std::cout << "No data to make default" << std::endl;
	}

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

	enet_address_set_host(&address, "localhost");
	address.port = 9000; //y u no over 9000?

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
	for(std::vector<BaseCode*>::iterator it = codes.begin(); it != codes.end(); it++)
	{
		std::cout << "Got a code!: " << (*it) << std::endl;
		// TODO: Add codes to ICBM Table
		std::vector<std::string> columns;
		columns.push_back(inputFBox);
		columns.push_back((*it)->getCode());
		columns.push_back((*it)->getDesc());
		columns.push_back((*it)->getFlags());

		if((*it)->getType() == 1)
		{
			Header[0] = "Searched";
			Header[1] = "ICD 10 Code";
		}

		data.push_back(columns);
	}
	if(codes.size() > 0)
	{
		ICBMTable->rows(codes.size());
		ICBMTable->redraw();
	}
	else if(codes.size() == 0)
	{
		Header[0] = "Searched";
		ICBMTable->rows(1);
		ICBMTable->redraw();
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

void exitCallback(Fl_Widget* w, void* ptr)
{
	exit(EXIT_SUCCESS);
}

void helpCallback(Fl_Widget* w, void* ptr)
{
	Fl_Window* helpWindow = new Fl_Window(100, 100, "Help");
	helpWindow->end();
	helpWindow->show();
}


int main(int argc, char** argv)
{
	// Start up the netwOrk connection...
	std::cout << "About to init enet" << std::endl;
	initEnet();
	atexit(destroyEnet);

	Fl_Window* window = new Fl_Window(575, 500, "ICD Conversion Application");
	Fl_Input* codeInputBox = new Fl_Input(15, 30, 435, 20);

	Fl_Button* submitButton = new Fl_Button(485, 30, 75, 20, "Submit");
	submitButton->callback(&submitButtonClick, codeInputBox);

	//Fl_Button *defaultButton = new Fl_Button(15, 435, 150, 20, "Make default code");
	//defaultButton->callback(&defaultButtonClick);

	menu = new Fl_Menu_Bar(0, 0, 575, 20, " ");
	menu->add("File/Exit", "esc", exitCallback);
	menu->add("Recent/...", 0, testCallback);
	menu->add("Help/Get Help", 0, helpCallback);
	menu->add("Help/About", 0, testCallback);

	// Filling the column header information
 	Header[0] = "Searched";
	Header[1] = "ICD Code";
	Header[2] = "Description";
	Header[3] = "Flags";

	// Creating all the table stuff	
	ICBMTable = new ICDTable(15, 75, 545, 350, "ICBM Table");
	ICBMTable->selection_color(FL_YELLOW);
	ICBMTable->when(FL_WHEN_RELEASE);
	ICBMTable->col_header(1);		// enable col headers
	ICBMTable->col_resize(1);		// enable col resizing
	ICBMTable->cols(4);
   ICBMTable->col_width_all(125);   	// setting width of all cols
	ICBMTable->row_header(1);		// enable row headers
   ICBMTable->row_resize(1); 		// enable row resizing
	ICBMTable->rows(10);	
   ICBMTable->callback(table_cb, (void*)ICBMTable);
   ICBMTable->when(FL_WHEN_CHANGED|FL_WHEN_RELEASE);
   ICBMTable->end();
	// end table making

	window->end();
	window->show(argc, argv);
	
	Fl::lock();

	fl_create_thread(enetThread, enetMain, NULL);
	return Fl::run();
}