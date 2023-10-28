#include "procedures.h"

pqxx::result get_authors_over_age(pqxx::work &txn, int age)
{
	pqxx::result r {txn.exec("SELECT * FROM get_authors_over_age(" + std::to_string(age) + ");")};
	return r;
}
pqxx::result get_authors(pqxx::work &txn)
{
	pqxx::result r {txn.exec("SELECT * FROM get_authors()")};
	return r;
}
pqxx::result get_author_name_like(pqxx::work &txn, std::string name)
{
	pqxx::result r {txn.exec("SELECT * FROM get_author_name_like(" + name + ");")};
	return r;
}
pqxx::result get_author_name_like(pqxx::work &txn, std::string name, int age)
{
	pqxx::result r {txn.exec("SELECT * FROM get_author_name_like(" + name + ","  + std::to_string(age) + ");")};
	return r;
}