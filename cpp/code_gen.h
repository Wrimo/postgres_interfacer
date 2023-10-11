#include <pqxx/pqxx>
#include <string>

void generateTypes(pqxx::work &);
std::string getDataType(std::string &);
void dataCleanup(std::string &);