#include "procedures.h"
std::string quote(std::string & str) { return "\'" + str + "\'";}


std::vector<author> get_authors_over_age(pqxx::work &txn, int age)
{
	pqxx::result r {txn.exec("SELECT * FROM get_authors_over_age(" + std::to_string(age) + ");")};
    std::vector<author> data;
    for (auto row : r)
    {
        author x;
        x.lastname = row["lastname"].c_str();
        x.firstname = row["firstname"].c_str();
        x.id = std::stoi(row["id"].c_str());
        x.age = std::stoi(row["age"].c_str());
        data.push_back(x);
    }
	return data;
}
std::vector<author> get_author_name_like(pqxx::work &txn, std::string name)
{
	pqxx::result r {txn.exec("SELECT * FROM get_author_name_like(" + quote(name) + ");")};
    std::vector<author> data;
    for (auto row : r)
    {
        author x;
        x.lastname = row["lastname"].c_str();
        x.firstname = row["firstname"].c_str();
        x.id = std::stoi(row["id"].c_str());
        x.age = std::stoi(row["age"].c_str());
        data.push_back(x);
    }
	return data;
}
std::vector<author> get_authors(pqxx::work &txn)
{
	pqxx::result r {txn.exec("SELECT * FROM get_authors()")};
    std::vector<author> data;
    for (auto row : r)
    {
        author x;
        x.lastname = row["lastname"].c_str();
        x.firstname = row["firstname"].c_str();
        x.id = std::stoi(row["id"].c_str());
        x.age = std::stoi(row["age"].c_str());
        data.push_back(x);
    }
	return data;
}