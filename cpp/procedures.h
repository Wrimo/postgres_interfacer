#include <pqxx/pqxx>
#include <string>


pqxx::result get_authors(pqxx::work &);
pqxx::result get_stored_procedures(pqxx::work &);
pqxx::result get_tables_and_fields(pqxx::work &);