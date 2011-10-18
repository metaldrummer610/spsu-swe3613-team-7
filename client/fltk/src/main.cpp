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
#include <FL/Fl_Tree.H>
#include <FL/Fl_Multiline_Output.H>
#include "FL/Fl_Export.H"
#include "FL/fl_types.h"
#include <stdio.h>
#include <string.h>

#define HAVE_PTHREAD_H 1
#include "threads.h" // FLTK Threads Support

#include <enet/enet.h>
#include <ICDNetwork.h>
#include <iostream>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/export.hpp>

BOOST_SERIALIZATION_ASSUME_ABSTRACT(ICDPacket)
BOOST_CLASS_EXPORT(ICDResponsePacket)
BOOST_CLASS_EXPORT(ICDCommandPacket)
BOOST_CLASS_EXPORT(ICDCommandConvert9To10)
BOOST_CLASS_EXPORT(ICDResponseConvert9To10)

Fl_Select_Browser* codeList;
Fl_Thread enetThread;
Fl_Menu_Bar* menu;
Fl_Input* codeInputBox;
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
	// It's but a simple test callback
	std::cout << menu->text() << std::endl;
}

void recentCallback(Fl_Widget* w, void* ptr)
{
	data.clear();
	codeInputBox->value(menu->text());
	inputFBox = codeInputBox->value();

	std::string str(codeInputBox->value());
	ICDCommandConvert9To10* convertCommand = new ICDCommandConvert9To10(str);
	ICDCommandPacket* command = new ICDCommandPacket(convertCommand);
	sendPacket(command, peer);

	delete command;
}

void fixRecent()
{
	// All this thing does it switch around the values in the recent vector which basically holds
	// the most recent codes. Yes it's oddly pointless but I don't really care.
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
	codeInputBox = (Fl_Input*)ptr;
	std::cout << "Input: " << codeInputBox->value() << std::endl;

// Simply ignore everything till the next comment, all this stuff does is add the recent vector to the menu
// so that it can be accessed properly. At this point it still needs to have a proper callback function
// but I haven't gotten to that just yet, so for now it just prints whatever has been clicked.
	inputFBox = codeInputBox->value();

	recent.push_back(inputFBox);

	fixRecent();
	int index = menu->find_index("Recent");
	if(index != -1) menu->clear_submenu(index);

	for(std::vector<std::string>::iterator it = recent.begin(); it != recent.end(); it++)
	{
		std::string temp = "Recent/"+(*it);
		menu->add(temp.c_str(), 0, recentCallback);
	}

	// Let the magic begin...
	std::string str(codeInputBox->value());
	ICDCommandConvert9To10* convertCommand = new ICDCommandConvert9To10(str);
	ICDCommandPacket* command = new ICDCommandPacket(convertCommand);
	sendPacket(command, peer);

	delete command;
}

// Not actually sure if this stuff is still relevant, but I'll keep it here just to be safe
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

void handleConvert9To10Response(ICDResponseConvert9To10* packet)
{
	std::vector<ICDCode*> codes = packet->getCodes();

	Fl::lock();
	for(auto it = codes.cbegin(); it != codes.cend(); it++)
	{
		//std::cout << "Got a code!: " << (*it) << std::endl;
		// Hey, guess what TODO, we fixed it you can haz your leave now
		std::vector<std::string> columns;
		columns.push_back(inputFBox);
		columns.push_back((*it)->getCode());
		columns.push_back((*it)->getDesc());
		columns.push_back((*it)->getFlags());
		
		if((*it)->getType() == CodeType::ICD10)
		{
			Header[0] = "Searched";
			Header[1] = "ICD 10 Code";
		}
		data.push_back(columns);
		columns.clear();
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
	};
	Fl::unlock();
	Fl::awake(packet);

	for(auto it = codes.begin(); it != codes.end(); it++)
	{
		delete (*it);
		(*it) = NULL;
	}

	codes.clear();

	std::cout << "Atreyu!!! Falkor!!! Need I go on....?" << std::endl;
}

void handlePacket(ENetPacket* p)
{
	std::cout << "ZOMG WE GOT A PACKET1!!1!!!" << std::endl;
	ICDPacket* packet = getPacket(p);

	switch(packet->getType())
	{
		case ICD_PACKET_TYPE_COMMAND:
		{
			ICDCommandPacket* commandPacket = (ICDCommandPacket*)packet;
			break;
		}
		case ICD_PACKET_TYPE_RESPONSE:
		{
			ICDResponsePacket* responsePacket = (ICDResponsePacket*)packet;
			ICDResponse* response = responsePacket->getResponse();
			if(response->getResponseType() == ICD_RESPONSE_CONVERT_9_TO_10)
			{
				handleConvert9To10Response((ICDResponseConvert9To10*)response);
			}
		}
	}

	delete packet;
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
	// Just an exit callback, nothing to be concerned about
	exit(EXIT_SUCCESS);
}

Fl_Multiline_Output* helpOut;

void helpTreeCallback(Fl_Widget* w, void* data)
{
	// I thought I told you the last one was nothing to be concerned about.... Fine, moving on:
	// This function is the for the help menu, its a basic tree that will eventually contain all
	// the information on how this program will work. So pretty much it'll just say "Oh fuck!"
	// Not really. The switch statement below and corresponding tree/item creates allow
	// for the fucntion to properly show which item has been selected so that it can show
	// the right thing inside the other box which I'll eventually put in
	// TODO: Create another box to put all this whacky information into
	Fl_Tree* tree = (Fl_Tree*)w;
	Fl_Tree_Item* item = (Fl_Tree_Item*)tree->callback_item();

	if(! item) return;
	switch(tree->callback_reason())
	{
		case FL_TREE_REASON_SELECTED:
		char path[256];
		tree->item_pathname(path, sizeof(path), item);

			if(strncmp(path, "1x", 1) == 0)
			{
				if(strncmp(item->label(), "1x", 1) == 0)
				{
					std::cout << item->label() << std::endl;
					helpOut->value(item->label());
				}
				else if(strncmp(item->label(), "2x", 1) == 0)
				{
					helpOut->value(item->label());
				}
			}
			else if(strncmp(path, "2x", 1) == 0)
			{
				if(strncmp(item->label(), "1x", 1) == 0)
				{
					helpOut->value(item->label());
				}
			}
			break;
		case FL_TREE_REASON_DESELECTED:
			break;
		case FL_TREE_REASON_OPENED:
			break;
		case FL_TREE_REASON_CLOSED:
			break;
		case FL_TREE_REASON_NONE:
			break;
	}

}


void helpCallback(Fl_Widget* w, void* ptr)
{
	// Incase you were wondering why we have two help callback things,
	// this one actually creates the window, the tree, and the soon to exist box.
	Fl_Window* helpWindow = new Fl_Window(600, 265, "Help");

	Fl_Tree* helpTree = new Fl_Tree(15, 15, 250, 235, "");

	helpOut = new Fl_Multiline_Output(280, 15, 600-250-45, 235, "");
	helpOut->align(FL_ALIGN_WRAP);

	helpTree->showroot(0);
	helpTree->callback(helpTreeCallback);

	helpTree->add("1. ICD Conversion/1. Submitting a code");
	helpTree->add("1. ICD Conversion/2. How to read the table");
	helpTree->add("2. Recent/1. How to use the recent menu");

	helpTree->close("ICD Conversion");
	helpTree->close("Recent");

	helpWindow->end();
	helpWindow->show();
}

void aboutCallback(Fl_Widget* w, void* ptr)
{
	// This function is all about making the help about window, which is completely useless for all intents and purposes
	Fl_Window* aboutWindow = new Fl_Window(250, 250, "About");
	aboutWindow->begin();
	{
		Fl_Box* aboutBox = new Fl_Box(15, 15, aboutWindow->w()-30, aboutWindow->h()-30, " ");
		aboutBox->box(FL_UP_BOX);
		aboutBox->label("ICD code conversion program \n Class: SWE3613 Software System Engineering \n Group 7 \n David Butcher, Kevin DeBrito, Robbie Diaz, Gregory Goncharov, and Jeff Lett");
		aboutBox->align(FL_ALIGN_WRAP|FL_ALIGN_CENTER);
	}
	aboutWindow->resizable(aboutWindow); // Yeah, I can haz resize
	aboutWindow->end();
	aboutWindow->show();
}

int main(int argc, char** argv)
{
	// Start up the netwOrk connection...
	std::cout << "About to init enet" << std::endl;
	initEnet();
	atexit(destroyEnet);

	Fl_Window* window = new Fl_Window(600, 500, "ICD Conversion Application");
   codeInputBox = new Fl_Input(15, 30, 435, 20);

	Fl_Button* submitButton = new Fl_Button(window->w()-90, 30, 75, 20, "Submit");
	submitButton->when(FL_WHEN_RELEASE);
	submitButton->callback(&submitButtonClick, codeInputBox);
	submitButton->shortcut(FL_ENTER);

	//Fl_Button *defaultButton = new Fl_Button(15, 435, 150, 20, "Make default code");
	//defaultButton->callback(&defaultButtonClick);

	// Who knew that creating a menu could be so boring......
	menu = new Fl_Menu_Bar(0, 0, window->w(), 20, " ");
	menu->add("File/Exit", "esc", exitCallback);
	menu->add("Recent/", 0, testCallback);
	menu->add("Help/Get Help", "h", helpCallback);
	menu->add("Help/About", 0, aboutCallback);

	// Filling the column header information
 	Header[0] = "Searched";
	Header[1] = "ICD Code";
	Header[2] = "Description";
	Header[3] = "Flags";

	// Creating all the table stuff	
	ICBMTable = new ICDTable(15, 75, window->w() - 30, 350, "ICBM Table");
	ICBMTable->selection_color(FL_YELLOW);
	ICBMTable->when(FL_WHEN_RELEASE);
	ICBMTable->col_header(1);		// enable col headers
	ICBMTable->col_resize(1);		// enable col resizing
	ICBMTable->cols(4);
   ICBMTable->col_width_all((ICBMTable->w()-40)/4);   	// setting width of all cols
	ICBMTable->row_header(1);		// enable row headers
   ICBMTable->row_resize(1); 		// enable row resizing
	ICBMTable->rows(10);	
   ICBMTable->callback(table_cb, (void*)ICBMTable);
   ICBMTable->when(FL_WHEN_CHANGED|FL_WHEN_RELEASE);
   ICBMTable->end();
	// end table making
	
	window->resizable(window);
	window->end();
	window->show(argc, argv);
	
	Fl::lock();

	fl_create_thread(enetThread, enetMain, NULL);
	return Fl::run();
}

/* How nice of you to continue reading past all the other nonsense and read this comment. Honestly though, this comment has absolutely nothing to do
 * with the program, I just felt like anyone who made it past the rest of my bullshit with all the additional callbacks and what-have-you. You deserve * some kind of thanks, so here you go.... "Thanks." Best thank you in history right?
*/
