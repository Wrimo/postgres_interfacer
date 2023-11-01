#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "types.h"

std::string quote(std::string &);


std::vector<author> get_authors_over_age(pqxx::work &txn, int);
std::vector<author> get_author_name_like(pqxx::work &txn, std::string);
std::vector<author> get_authors(pqxx::work &txn);