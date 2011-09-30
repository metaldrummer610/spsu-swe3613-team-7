#include <iostream>
#include <sstream>
#include <fstream>
#include "stdlib.h"
#include "icd_parser.h"




using namespace std;



// Menu Constants
const string COMMANDS[] = {"Load Files","Open SQL Files to Write to","Write to SQL Files","Close SQL Files","Quit" };
const int NUM_COMMANDS = 5;


/** This function will print a prettier
* centered message sandwiched in between
* lines of asterisks.
* @param message The message to be displayed
*/
void print_message(string message) {
	cout << "********************************************************************************" << endl;
	for(unsigned i=0;i<((80-message.size())/2);++i)
		cout << " ";
	cout << message << endl << "********************************************************************************" << endl;
}

/** This function will print the menu for
* the user.
*/
void print_menu() {
	print_message("Please choose one of the following options:");
	for(int i=0;i<NUM_COMMANDS;++i) {
		cout << "                          Select: " << i+1 << " " << COMMANDS[i] << endl;
	}
}

/** This is a little helper function to
* make converting strings a little easier.
* @param s String to be converted
* @return The int value of the string, 0 if input lead by chars.
*/
int str_to_int(string s) {
    return atoi(s.c_str());
}

/** This function will print the menu, get the users input
* and call the function asked by the user's commands.
* @param my_parser The parser to use
*/
void process_commands(ICD_Parser* my_parser) {
	string line;
	do {
		print_menu();
		getline(cin,line);
		if(0==str_to_int(line)) { //user entered characters or a 0! NOO!
			print_message("Invalid input. Please enter an integer.");
		}
		else {
			switch(str_to_int(line)) {
				case 1:     my_parser->load_text_files();
              		        	break;
				case 2:     my_parser->open_sql_files();
                            		break;
                		case 3:     my_parser->parse_text_files();
					break;
                		case 4:     my_parser->close_sql_files();
                            		break;
			}
		}
	} while (str_to_int(line) < NUM_COMMANDS);
}

void run(ICD_Parser* my_parser) {  
      	my_parser->load_text_files();
        my_parser->open_sql_files();
        my_parser->parse_text_files();
        my_parser->close_sql_files();
}


int main()
{
   	ICD_Parser* my_parser=new ICD_Parser();
    	//For Menu
	//process_commands(my_parser);
    	//No Menu
    	run(my_parser);
	//print_message("Have a nice day.");

    	return 0;

}


