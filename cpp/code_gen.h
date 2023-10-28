#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <pqxx/pqxx>
#include <string>
#include <vector>

class VariableData
{
public:
    std::string name;
    std::string type;
};

std::string convertDataType(std::string &);
std::vector<VariableData> getVariablesAndTypes(std::string &);
void dataCleanup(std::string &);

void generateFunctions(pqxx::work &);
void generateStructs(pqxx::work &);

#endif // CODE_GEN_H