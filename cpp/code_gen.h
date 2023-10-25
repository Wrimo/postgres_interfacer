#include <pqxx/pqxx>
#include <string>

std::string convertDataType(std::string &);
std::string getTypes(std::string &);
void dataCleanup(std::string &);


void generateFunctions(pqxx::work &); 
void generateStructs(pqxx::work &);