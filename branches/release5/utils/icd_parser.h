#ifndef ICD_PARSER_H

#define ICD_PARSER_H
#include <iostream>
#include <fstream>
#include <sstream>





class ICD_Parser

{
    std::ifstream gems_file, icd_9_file, icd_10_file;
    std::ofstream gems_sql, icd_9_sql, icd_10_sql;
    void parse_gems();
    void parse_9_codes();
    void parse_10_cm_codes();
    public:
        void open_sql_files();
        void close_sql_files();
        void parse_text_files();
        void load_text_files();
        ICD_Parser();

        virtual ~ICD_Parser();

    protected:



};



#endif // ICD_PARSER_H


