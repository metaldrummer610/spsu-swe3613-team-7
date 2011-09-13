
#include <cerrno>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string>
#include "utf8.h"
#include "icd_parser.h"

using namespace std;

/* File Constants
* These are ugly filenames, but the names that come from http://cms.gov
*/
const string DATA_PATH = "./data/";
const string GEMS_FILENAME = DATA_PATH + "icd_9_cm_to_10_cm_gems.txt";
const string ICD_9_FILENAME = DATA_PATH + "icd_9_descriptions.txt";
const string ICD_10_FILENAME = DATA_PATH + "icd_10_cm_descriptions.txt";

/* SQL File Constants
* These are the files that we will output the SQL to
*/
const string SQL_DB_PATH = "../server/scripts/";
const string GEMS_SQL_FILENAME = SQL_DB_PATH + "GEMS_DATA.SQL";
const string ICD_9_SQL_FILENAME = SQL_DB_PATH + "ICD_9_DESCRIPTIONS.SQL";
const string ICD_10_SQL_FILENAME = SQL_DB_PATH + "ICD_10_DESCRIPTIONS.SQL";

/* Database Constants
* These are the database name constants
*/
const string GEMS_DB = "cm_gems";
const string ICD_9_DB = "icd_9_descriptions";
const string ICD_10_CM_DB = "icd_10_cm_descriptions";
const string ICD_10_PCS_DB = "icd_10_pcs_descriptions";

ICD_Parser::ICD_Parser()
{
    //ctor
}

ICD_Parser::~ICD_Parser()
{
    //dtor
}

/** Prints a short message
* @param message The message to be displayed
*/
void print_short_message(string message) {
	for(unsigned i=0;i<((80-message.size())/2);++i)
		cout << " ";
	cout << message << endl;
}

/** Parses the cm_gems
*/
void ICD_Parser::parse_gems() {
    string line,sql;
//    int i=0;
    while(getline(gems_file,line)) {
        stringstream str(line);
        string a, b, c;
        str >> a >> b >> c;

/*
//Begin compact fast way
        if(0==i)
            sql = "INSERT INTO " + GEMS_DB + " (icd_9_code, icd_10_code, flags) VALUES " + "('" + a + "','" + b + "'," + c + ")";
        else
            sql = ",('" + a + "','" + b + "'," + c + ")";
        gems_sql << sql;
        ++i;
    }
    gems_sql << ";";
//End compact fast way

*/
// Begin old slow debugging way
        sql = "INSERT INTO " + GEMS_DB + " (icd_9_code, icd_10_code, flags) VALUES ('" + a + "','" + b + "'," + c + ");";
        gems_sql << sql << endl;
    }
 //End slow debugging way
}


/** 
* Used to find and replace single quotes
*/
string find_and_replace( string source, string find, string replace )
{
    //Found on http://stackoverflow.com/questions/1087088/single-quote-issues-with-c-find-and-replace-function
    string::size_type pos = 0;
    while ( (pos = source.find(find, pos)) != string::npos ) {
        source.replace( pos, find.size(), replace );
        pos += replace.size();
    }
    return source;
}

/** Used to remove non-UTF-8 characters
* from library at http://utfcpp.sourceforge.net/
*/
void fix_utf8_string(string& str)
{
    std::string temp;
    utf8::replace_invalid(str.begin(), str.end(), back_inserter(temp));
    str = temp;
}

/** Used to remove non-UTF-8 characters
* from library at http://utfcpp.sourceforge.net/
*/
string replace_bad_chars(string source) {
    source=find_and_replace(source,"'","''");

    string line=source;
    string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
    if (end_it != line.end()) {
        fix_utf8_string(source);
    }
    return source;
}

/** Parses the ICD 9 Descriptions
*/
void ICD_Parser::parse_9_codes() {
    print_short_message("Parsing ICD 9 Descriptions...");
    string line,sql;
    int i=0;
    while(getline(icd_9_file,line)) {
        stringstream str(line);
        string a, b, c;
        str >> a >> b;
        c=b;
        while(str >> b)
            c+=" "+b;
        c=replace_bad_chars(c);
        if(0==i)
            sql = "INSERT INTO " + ICD_9_DB + " (code, description) VALUES " + "('" + a + "','" + c + "')";
        else
            sql = ",('" + a + "','" + c + "')";
        icd_9_sql << sql;
        ++i;
    }
    icd_9_sql << ";";
}

/** Parses the ICD 10-CM Descriptions
*/
void ICD_Parser::parse_10_cm_codes() {
    //cout << "IN 10" << endl;
    print_short_message("Parsing ICD 10 Descriptions...");
    string line,sql;
    int i=0;
    while(getline(icd_10_file,line)) {
        stringstream str(line);
        string a, b, c;
        str >> a >> b;
        c=b;
        while(str >> b)
            c+=" "+b;
        c=replace_bad_chars(c);

//Begin compact fast way
        if(0==i)
            sql = "INSERT INTO " + ICD_10_CM_DB + " (code, description) VALUES " + "('" + a + "','" + c + "')";
        else
            sql = ",('" + a + "','" + c + "')";
        icd_10_sql << sql;
        ++i;
    }
    icd_10_sql << ";";
//End compact fast way

/*
//Begin slow debugging way
        sql = "INSERT INTO " + ICD_10_CM_DB + " (code, description) VALUES " + "('" + a + "','" + c + "');";
        icd_10_sql << sql << endlls
        ls
        ;
    }
//End slow debugging way
*/
}


/** Parses the text files
*/
void ICD_Parser::parse_text_files() {
    if(!gems_file.is_open() || !icd_9_file.is_open() || !icd_10_file.is_open()) {
        print_short_message("Please load files first.");
        return;
    }
    print_short_message("Parsing...");
    parse_gems();
    parse_9_codes();
    parse_10_cm_codes();
    print_short_message("Done Parsing.");
    print_short_message("Have a nice day. :)");
}

/** Opens the .SQL files to write to
*/
void ICD_Parser::open_sql_files() {
    gems_sql.open(GEMS_SQL_FILENAME.c_str());
    icd_9_sql.open(ICD_9_SQL_FILENAME.c_str());
    icd_10_sql.open(ICD_10_SQL_FILENAME.c_str());
}

/** Closes the .SQL files after were done
*/
void ICD_Parser::close_sql_files() {
    gems_sql.close();
    icd_9_sql.close();
    icd_10_sql.close();
}

/** Opens the files to convert from
*/
void ICD_Parser::load_text_files() {
    gems_file.open(GEMS_FILENAME.c_str());
    if(gems_file.is_open())
        print_short_message("GEMs File Found.");
    else
        print_short_message("ERROR: GEMs File NOT Found.");

    icd_9_file.open(ICD_9_FILENAME.c_str());
    if(icd_9_file.is_open())
        print_short_message("ICD 9 Descriptions File Found.");
    else
        print_short_message("ERROR: ICD 9 Descriptions File NOT Found.");

    icd_10_file.open(ICD_10_FILENAME.c_str());
    if(icd_10_file.is_open())
        print_short_message("ICD 10 Descriptions File Found.");
    else
        print_short_message("ERROR: ICD 10 Descriptions File NOT Found.");
}

