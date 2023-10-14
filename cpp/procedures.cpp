#include "procedures.h"

pqxx::result get_authors(pqxx::work &txn)
{
	pqxx::result r {txn.exec("SELECT * FROM get_authors();")};
	return r;
}
pqxx::result get_stored_procedures(pqxx::work &txn)
{
	pqxx::result r {txn.exec("SELECT * FROM get_stored_procedures();")};
	return r;
}
pqxx::result get_tables_and_fields(pqxx::work &txn)
{
	pqxx::result r {txn.exec("SELECT * FROM get_tables_and_fields();")};
	return r;
}