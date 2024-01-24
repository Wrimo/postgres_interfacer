#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <pqxx/pqxx>
#include <string>
#include <map>
#include "code_gen_types.h"

void codeGenStart(LanguageImplementation*, pqxx::work &);

std::vector<VariableData> getVariablesAndTypes(std::string &);
void dataCleanup(std::string &);

void generateStoredProcedures(pqxx::work &);
std::string generateFunction(pqxx::row &);
std::string generateProcedure(pqxx::row &);

std::string generateStructs(std::string &, std::string &);
void generateTableStructs(pqxx::work &);
void generateFunctionStructs(std::string, std::string);

#endif // CODE_GEN_H