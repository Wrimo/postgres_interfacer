#include <iostream> 
#include <pqxx/pqxx>
#include <sstream>
#include "hidden.h"

int main() 
{   
    std::stringstream conString; 
    conString << "postgresql://" << USERNAME << ":" << PASSWORD << "@localhost:" <<  PORT << "/" << USERNAME;
    pqxx::connection c{conString.str()};
    pqxx::work txn{c};
    txn.exec0(
        "CREATE TABLE BOOK("
        "name   text,"
        "author text)"
    );

    txn.commit();

    return 0; 
}