#include <pqxx/pqxx>
#include <string>


pqxx::result get_authors_over_age(pqxx::work &txn, int);
pqxx::result get_authors(pqxx::work &txn);
pqxx::result get_author_name_like(pqxx::work &txn, std::string);
pqxx::result get_author_name_like(pqxx::work &txn, std::string, int);