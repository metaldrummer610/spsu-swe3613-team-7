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
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Multiline_Input.H>
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
BOOST_CLASS_EXPORT(ICDCommandGetICD9Code)
BOOST_CLASS_EXPORT(ICDResponseGetICD9Code)
BOOST_CLASS_EXPORT(ICDCommandGetICD10Code)
BOOST_CLASS_EXPORT(ICDResponseGetICD10Code)
BOOST_CLASS_EXPORT(ICDCommandGetDXCode)
BOOST_CLASS_EXPORT(ICDResponseGetDXCode)
BOOST_CLASS_EXPORT(ICDCommandCreateDXCode)
BOOST_CLASS_EXPORT(ICDCommandGetDXCodes)
BOOST_CLASS_EXPORT(ICDResponseGetDXCodes)

Fl_Select_Browser* codeList;
Fl_Thread enetThread;
Fl_Menu_Bar* menu;
Fl_Input* codeInputBox;
bool enetThreadRunning = false;
ENetHost* client;
ENetPeer* peer;
ENetAddress address;
ENetEvent event;
std::string Header[4];
std::string inputFBox;

std::vector<std::vector<std::string> > data;
std::vector<std::vector<std::string> > codesToClaim;
std::vector<std::string> recent;
std::vector<std::string> newDxCode;

class ICDTable : public Fl_Table_Row
{
		  int typeOfTable;
		  int currentSRow;
		  // This is to make sure that I can use the table in other windows  
		  // without have to make up some new classes that have this class in
		  // it. So for this nonsense we will use: 0 for the main window,
		  // 1 for claims, and 2 to be added later when I figure out what its for.

		  protected:
		  void draw_cell(TableContext context,  		// table cell drawing
								int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
		  //    void callback(TableContext context, 		// callback for table events
		  //    		   int R, int C);
		  public:
		  ICDTable(int x, int y, int w, int h, const char *l=0) : Fl_Table_Row(x,y,w,h,l)
		  {
					 typeOfTable = 0;
					 end(); 
		  }
		  ~ICDTable() { }

		  void setTableType(int type)
		  {
					 typeOfTable = type;
		  }
		  int getTableType()
		  {
					 return typeOfTable;
		  }
		  int getSRow()
		  {
					 return currentSRow;
		  }
		  void setSRow(int s)
		  {
					 currentSRow = s;
		  }
};

// Handle drawing all cells in table
void ICDTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
		  static char s[40];
		  sprintf(s, "%s", "No Data");

		  switch ( context )
		  {
					 case CONTEXT_STARTPAGE:
								fl_font(FL_HELVETICA, 16);
								return;

					 case CONTEXT_ROW_HEADER:
								{
										  sprintf(s, "%d", R+1);
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
										  if(typeOfTable == 0)
										  {
													 if(data.size() > 0)
													 {
																sprintf(s, "%s", data.at(R).at(C).c_str());
													 }
													 else
													 {
																sprintf(s, "%s", "No Data");
													 }
										  }
										  else if(typeOfTable == 1)
										  {
													 if(codesToClaim.size() > 0)
													 {
																sprintf(s, "%s", codesToClaim.at(R).at(C).c_str());
													 }
										  }
										  else if(typeOfTable == 2)
										  {
										  }
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
					 default:
								return;
		  }
}

void testCallback(Fl_Widget* w, void* ptr)
{
		  // It's but a simple test callback
		  std::cout << menu->text() << std::endl;
}

void recentCallback(Fl_Widget* w, void* ptr)
{

		  // Bet you can't figure out what this does...
		  // Incase you can't, it basically does what the submit button callback does
		  // which is submitting the code that was chosen to the server to get some
		  // ICD codes back.

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

ICDTable* ICBMTable; 
ICDTable* currentClaim;
ICDTable* claimTable;

void table_cb(Fl_Widget* o, void* cdata)
{
		  // I'm fairly certain at this point that this callback is by far the most useless callback
		  // in this project, I've seen more productivity in a method that returns the sum of 1+1
		  ICDTable* table = (ICDTable*)cdata;
		 /* fprintf(stderr, "%s callback: row=%d col=%d, context=%d, event=%d clicks=%d\n",
								(const char*)table->label(),
								(int)table->callback_row(),
								(int)table->callback_col(),
								(int)table->callback_context(),
								(int)Fl::event(),
								(int)Fl::event_clicks());*/
		  int  tableType = table->getTableType();
		  if(tableType == 0)
					 ICBMTable->setSRow((int)(table->callback_row()));
		  else if(tableType == 1)
					 currentClaim->setSRow((int)(table->callback_row()));
		  else if(tableType == 2)
					 claimTable->setSRow((int)(table->callback_row()));
}

void submitButtonClick(Fl_Widget* widget, void* ptr)
{
		  data.clear();
		  std::cout << "Click!" << std::endl;
		  codeInputBox = (Fl_Input*)ptr;
		  std::cout << "Input: " << codeInputBox->value() << std::endl;

		  // Simply ignore everything till the next comment, all this stuff does is add the recent vector to the menu
		  // so that it can be accessed properly. At this point it still needs to have a proper callback function
		  // but I haven't gotten to that just yet, so for now it just prints whatever has been clicked.
		  // As of now the callback has been added so you can ignore the previous line, I just like to have
		  // a history of what I've accomplished over the last few days
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
/*void defaultButtonClick(Fl_Widget* widget)
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

}*/

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

		  while(enet_host_service(client, &event, 500) > 0)
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

void handleGetICD9Code(ICDResponseGetICD9Code* response)
{
}

void handleGetICD10Code(ICDResponseGetICD10Code* response)
{
}

void handleGetDXCode(ICDResponseGetDXCode* response)
{
}

void handleGetDXCodes(ICDResponseGetDXCodes* response)
{
}

void handlePacket(ENetPacket* p)
{
		  std::cout << "ZOMG WE GOT A PACKET1!!1!!!" << std::endl;
		  ICDPacket* packet = getPacket(p);

		  switch(packet->getType())
		  {
					 case PacketType::Response:
								{
										  ICDResponsePacket* responsePacket = (ICDResponsePacket*)packet;
										  ICDResponse* response = responsePacket->getResponse();
										  if(response->getResponseType() == ResponseType::Convert9To10)
										  {
													 handleConvert9To10Response((ICDResponseConvert9To10*)response);
										  }
										  else if(response->getResponseType() == ResponseType::GetICD9Code)
										  {
										  	handleGetICD9Code((ICDResponseGetICD9Code*)response);
										  }
										  else if(response->getResponseType() == ResponseType::GetICD10Code)
										  {
										  	handleGetICD10Code((ICDResponseGetICD10Code*)response);
										  }
										  else if(response->getResponseType() == ResponseType::GetDXCode)
										  {
										  	handleGetDXCode((ICDResponseGetDXCode*)response);
										  }
										  else if(response->getResponseType() == ResponseType::GetDXCodes)
										  {
										  	handleGetDXCodes((ICDResponseGetDXCodes*)response);
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

		  helpTree->close("1. ICD Conversion");
		  helpTree->close("2. Recent");

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

void addToClaimCallback(Fl_Widget* w, void* ptr)
{
		  // Another one of those function that really doesn't do much but has to be there to be sure that you don't
		  // add duplicate codes to your claim. I don't think that the insurance company would like you doing that...
		  // But without this you could be robin hood.......

		  bool inClaim = false;
		  if(codesToClaim.size() > 0)
		  {
					 for(int i = 0; i < codesToClaim.size(); i++)
					 {
								if(strcmp(codesToClaim.at(i).at(1).c_str(), data.at(ICBMTable->getSRow()).at(1).c_str()) == 0)
										  inClaim = true;
					 }
		  }
		  if(!inClaim && data.size() > 0)
		  {
					 codesToClaim.push_back(data.at(ICBMTable->getSRow()));
		  }
}

// This stuff right here is only here because it kept throwing a seg fault when I did something
// so in order to avoid that and make things less complicated for me I just moved it here.
// Basically all this is is just a window and bool value that all you to click yes on
// the prompt window and it'll bring up a thing to remove some codes. Also it makes it so
// that the claim window thingy below works properly with that if statement.
// so ya... thats the run down.

Fl_Window* prompt;
bool isPrompt = false;
void windowKill(Fl_Widget* w, void* ptr)
{
		  Fl_Window* win = (Fl_Window*)ptr;

		  if(strcmp(win->label(), "DX Code Creation") == 0)
		  {
					 std::cout << "F**k trees I climb boyous motherf**ker" << std::endl;
					 Fl_Window* noData = new Fl_Window(200, 155, "No Data");
					 Fl_Box* nothing = new Fl_Box(0, 0, noData->w(), 60, "No data to submit");
					 nothing->align(FL_ALIGN_CENTER);
					 Fl_Button* ok = new Fl_Button((noData->w()/2) - 30, 75, 60, 20, "OK");
					 ok->callback(windowKill, (void*)noData);
					 noData->end();
					 noData->show();
		  }
		  else
					 std::cout << "Believe me when I say, I f**ked a mermaid" << std::endl;
		  win->hide();
		  win->default_callback(win, ptr);
}

void removeElementCallback(Fl_Widget* w, void* ptr)
{
		 if(codesToClaim.size() > 0 && currentClaim->getSRow() >= 0)
		 {
					codesToClaim.erase(codesToClaim.begin() + currentClaim->getSRow());
					currentClaim->rows(currentClaim->rows() - 1);
					currentClaim->redraw();
		 }
}

void toClaimCallback(Fl_Widget* w, void* ptr)
{
		  // This guy right here, hes a man. All this function does is show you what you're about to claim
		  // Then you can remove stuff and head back to that main screen and add some more. Like a real man
		  // He builds and destroys. He's not like the bitch below...
		  if(isPrompt)
		  {
					 prompt->hide();
					 prompt->default_callback(prompt, ptr);
					 isPrompt = false;
		  }

		  Fl_Window* toClaim = new Fl_Window(600, 400, "Current Claim");
		  {
					 currentClaim = new ICDTable(15, 15, toClaim->w() - 30, toClaim->h() - 80);
					 {
								currentClaim->selection_color(FL_YELLOW);
								currentClaim->when(FL_WHEN_RELEASE);
								currentClaim->setTableType(1);
								currentClaim->col_header(1);
								currentClaim->col_resize(1);
								currentClaim->cols(4);
								currentClaim->col_width_all((currentClaim->w()-40)/4);
								currentClaim->row_header(1);
								currentClaim->row_resize(1);

								if(codesToClaim.size() > 0)
										  currentClaim->rows(codesToClaim.size());	
								else
										  currentClaim->rows(1);
								currentClaim->callback(table_cb, (void*)currentClaim);
								currentClaim->when(FL_WHEN_RELEASE | FL_WHEN_CHANGED);
					 }
					 currentClaim->end();

					 Fl_Button* removeStuff = new Fl_Button(15, 400 - 35, 75, 20, "Remove");
					 removeStuff->callback(removeElementCallback);
		  }

		  toClaim->end();
		  toClaim->show();
}

void submitClaimCallback(Fl_Widget* w, void* ptr)
{
		  // So this stuff is crazy, you can add some codes using the defualt button (Soon to be changed)
		  // Then it pops up with this nifty looking window that has a table full of the codes that you want to claim
		  // To be nice I made it so that you can only claim up to 8 codes otherwise it pops up and will take you to a
		  // screen that will show you what you are currently claiming and you can remove some so that you can make a claim.
		  // Thats right, only 8 codes and then it nags you about how you have to many codes.
		  // Next thing you know it will be telling you to come home earlier and spend more time with.
		  // Clingy bitch.....

		  if(codesToClaim.size() <= 8)
		  {
					 Fl_Window* claimWindow = new Fl_Window(600, 415, "Submit a Claim");
					 {
								Fl_Tabs* claims = new Fl_Tabs(15, 15, claimWindow->w() - 30, 350, " ");
								{
										  Fl_Group* codes = new Fl_Group(15, 30, claims->w(), 350, "Claims");
										  {

													 claimTable = new ICDTable(30, 45, codes->w() - 30, 300, "");
													 claimTable->setTableType(1);
													 claimTable->col_header(1);		// enable col headers
													 claimTable->col_resize(1);		// enable col resizing
													 claimTable->cols(4);
													 claimTable->col_width_all((claimTable->w()-40)/4);   	// setting width of all cols
													 claimTable->row_header(1);		// enable row headers
													 claimTable->row_resize(1);		// enable row resizing

													 if(codesToClaim.size() > 0)
																claimTable->rows(codesToClaim.size());	
													 else
																claimTable->rows(1);

													 claimTable->end();
										  }
										  codes->end();

										  Fl_Group* patientInfo = new Fl_Group(30, 30, claims->w(), 350, "Patient Info");
										  {
													 Fl_Input* name = new Fl_Input(80, 45, patientInfo->w() - 80, 20, "Name");
													 name->maximum_size(100);

													 Fl_Input* number = new Fl_Input(80, 80, patientInfo->w() - 80, 20, "Number");
													 number->maximum_size(100);

													 Fl_Multiline_Input* notes = new Fl_Multiline_Input(80, 115, patientInfo->w() - 80, 100, "Notes");
													 notes->maximum_size(500);
													 notes->wrap(1);
										  }
										  patientInfo->end();
								}
								claims->end();
					 }

					 Fl_Button* submitButton = new Fl_Button(15, 380, 100, 20, "Submit Claim");
					 submitButton->callback(testCallback);

					 claimWindow->resizable(claimWindow);
					 claimWindow->end();
					 claimWindow->show();
		  }
		  else
		  {
		  			 isPrompt = true;
					 prompt = new Fl_Window(415, 125, "Too many codes");

					 Fl_Box* textBox = new Fl_Box(15, 15, prompt->w() - 30, 50);
					 textBox->label("You have too many codes in your claim. Would you like to remove some?");
					 textBox->align(FL_ALIGN_WRAP);
					 Fl_Button* yes = new Fl_Button(235, 90, 75, 20, "Yes");
					 yes->callback(toClaimCallback, ptr);
					 Fl_Button* no = new Fl_Button(325, 90, 75, 20, "No");
					 no->callback(windowKill, (void*)prompt);
					 prompt->end();
					 prompt->show();
		  }
}

Fl_Window* dxCodeWindow;

void saveDxCodeCallback(Fl_Widget* w, void* ptr)
{
		  newDxCode.clear();
		  Fl_Multiline_Input* dxDescription = (Fl_Multiline_Input*) ptr;
		  newDxCode.push_back(data.at(ICBMTable->getSRow()).at(1));
		  newDxCode.push_back(dxDescription->value());
		  std::string str(newDxCode.at(0));
		  std::string str2(newDxCode.at(1));
		  ICDCommandCreateDXCode* command = new ICDCommandCreateDXCode(str, str2);
		  ICDCommandPacket* packet = new ICDCommandPacket(command);
		  sendPacket(packet, peer);
		  delete packet;
		  dxCodeWindow->hide();
		  dxCodeWindow->default_callback(dxCodeWindow, ptr);
}

void dxCodeCreateCallback(Fl_Widget* w, void* ptr)
{
		  dxCodeWindow = new Fl_Window(340, 200, "DX Code Creation");
		  {
					 Fl_Input* icdCode = new Fl_Input(100, 15, 225, 20, "ICD 10 Code");
					 icdCode->readonly(1);
					 if(data.size() > 0)
								icdCode->value(data.at(ICBMTable->getSRow()).at(1).c_str());
					 else
								icdCode->value("No Data");

					 Fl_Multiline_Input* dxDescription = new Fl_Multiline_Input(100, 50, 225, 60, "Description");
					 dxDescription->wrap(1);
					 dxDescription->maximum_size(250);

					 Fl_Button* saveDxCode = new Fl_Button(100, 125, 75, 20, "Save");
					 if(data.size() > 0)
								saveDxCode->callback(saveDxCodeCallback, dxDescription);
					 else
								saveDxCode->callback(windowKill, dxCodeWindow);
		  }
		  dxCodeWindow->end();
		  dxCodeWindow->show();
}

int main(int argc, char** argv)
{
		  // Start up the netwOrk connection...
		  std::cout << "About to init enet" << std::endl;
		  initEnet();
		  atexit(destroyEnet);

		  Fl_Window* window = new Fl_Window(750, 435, "ICD Conversion Application");
		  codeInputBox = new Fl_Input(15, 30, 435, 20);
		  codeInputBox->maximum_size(45);

		  Fl_Button* submitButton = new Fl_Button(window->w()-90, 30, 75, 20, "Submit");
		  submitButton->callback(&submitButtonClick, codeInputBox);

		  codeInputBox->callback(&submitButtonClick, codeInputBox);
		  codeInputBox->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);

		  Fl_Button* addToButton = new Fl_Button(window->w() - 165, 75, 150, 20, "Add Code To Claim");
		  addToButton->callback(addToClaimCallback, (void*)ICBMTable);

		  Fl_Button* createDxCode = new Fl_Button(window->w() - 165, 110, 150, 20, "Create DX Code");
		  createDxCode->callback(dxCodeCreateCallback);

		  Fl_Button* showClaim = new Fl_Button(window->w() - 165, 145, 150, 20, "Show Current Claim");
		  showClaim->callback(toClaimCallback);

		  Fl_Button* submitClaim = new Fl_Button(window->w() - 165, 180, 150, 20, "Submit Claim");
		  submitClaim->callback(submitClaimCallback);

		  // Who knew that creating a menu could be so boring......
		  menu = new Fl_Menu_Bar(0, 0, window->w(), 20, " ");
		  menu->add("File/Submit Claim", "", submitClaimCallback);
		  menu->add("File/Current Claim", "", toClaimCallback);
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
		  ICBMTable = new ICDTable(15, 75, window->w() - submitClaim->w() - 45, 350, "Code Table");
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
