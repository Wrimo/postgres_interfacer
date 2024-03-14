#ifndef CODE_GEN_TYPES_H
#define CODE_GEN_TYPES_H
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

struct VariableData
{
    std::string name;
    std::string type;
};

struct TableData
{
    std::string name;
    std::vector<VariableData> rows;
};

class LanguageImplementation
{
public:
    virtual std::string fileExtension() = 0;
    virtual std::string typeRequirements() = 0;
    virtual std::string typeGen(std::string &name, std::vector<VariableData> vars) = 0;
    virtual std::string procedureRequirements() = 0;
    virtual std::string convertType(std::string &postgresType) = 0;
    virtual std::string formatParameters(std::vector<VariableData> &params) = 0;
    virtual std::string generateFunctionCall(std::string name, std::vector<VariableData> &params) = 0;
    virtual std::string createProcedure(std::string name, std::string sql, std::vector<VariableData> &params) = 0;
    virtual std::string createFunction(std::string name, std::string sql, std::vector<VariableData> &params, TableData table) = 0;
};

class CPPImplementation : public LanguageImplementation
{
    std::string fileExtension() override
    {
        return ".h";
    }

    std::string typeRequirements() override
    {
        return "#include <string>\n";
    }

    std::string typeGen(std::string &name, std::vector<VariableData> vars) override
    {
        std::ostringstream strStruct;
        strStruct << "struct " << name << " {\n";
        for (size_t i = 0; i < vars.size(); ++i)
        {
            strStruct << "  " << vars[i].type << " " << vars[i].name << ";\n";
        }
        strStruct << "};\n";
        return "\n" + strStruct.str();
    }

    std::string procedureRequirements() override
    {
        std::ostringstream out;
        out << "#include <pqxx/pqxx>\n"
            << "#include <string>\n"
            << "#include <vector>\n"
            << "#include \"generated_types.h\"\n\n"
            << R"(std::string quote(std::string & str) { return "\'" + str + "\'";})"
            << "\n\n";
        return out.str();
    }

    std::string convertType(std::string &postgresType) override
    {
        if (postgresType == "text")
            return "std::string";
        else if (postgresType == "integer")
            return "int";
        else if (postgresType == "numeric")
            return "double";
        else if (postgresType == "boolean")
            return "bool";
        std::cout << postgresType << " is not a valid type\n";
        return "void *";
    }

    std::string formatParameters(std::vector<VariableData> &params) override
    {
        std::ostringstream out;

        for (size_t i = 0; i < params.size(); ++i)
        {
            out << ", " << params[i].type << " " << params[i].name;
        }

        return out.str();
    }

    std::string generateFunctionCall(std::string name, std::vector<VariableData> &params) override
    {

        if (params.size() == 0)
            return name + "()";

        std::ostringstream call;
        call << name << "(\"";

        for (size_t i = 0; i < params.size(); ++i)
        {
            if (params[i].type == "std::string")
                call << " + quote(" << params[i].name << ")";
            else if (params[i].type == "int" || params[i].type == "float")
                call << " + std::to_string(" << params[i].name << ")";

            call << (i == params.size() - 1 ? "" : R"(+ ", ")");
        }

        call << " + \")";
        return call.str();
    }

    std::string createProcedure(std::string name, std::string sql, std::vector<VariableData> &params) override
    {
        std::ostringstream out;
        std::string paramsString = this->formatParameters(params);
        out << "\nvoid " << name << "(pqxx::work &txn";
        out << paramsString << ")\n";
        out << "{\n	pqxx::result r {txn.exec(\"";
        out << sql << "\")};\n";
        out << "}";

        return out.str();
    }

    std::string createFunction(std::string name, std::string sql, std::vector<VariableData> &params, TableData table) override
    {
        std::ostringstream out;
        std::string paramsString = this->formatParameters(params);

        out << "\nstd::vector<" << table.name << "> ";
        out << name << "(pqxx::work &txn";
        out << paramsString << ")\n";

        out << "{\n    pqxx::result r {txn.exec(\"";
        out << sql;
        out << "\")};\n";

        out << "    std::vector<" << table.name << "> data;\n";
        out << "    for (auto row : r)\n    {\n"
            << "        " << table.name << " x;\n";

        for (VariableData row : table.rows) // convert each value in row to value in struct
        {
            if (row.type == "int")
                out << "        x." << row.name << " = "
                    << "std::stoi(row[\"" << row.name << "\"].c_str());\n";
            else if (row.type == "float")
                out << "        x." << row.name << " = "
                    << "std::stof(row[\"" << row.name << "\"].c_str());\n";
            else
                out << "        x." << row.name << " = "
                    << "row[\"" << row.name << "\"].c_str();\n";
        }
        out << "        data.push_back(x);\n";
        out << "    }";

        out << "\n	return data;\n}";

        return out.str();
    }
};

class PythonImplementation : public LanguageImplementation
{
    std::string fileExtension() override
    {
        return ".py";
    }

    std::string typeRequirements() override
    {
        return "from dataclasses import dataclass";
    }

    std::string typeGen(std::string &name, std::vector<VariableData> vars) override
    {
        std::ostringstream strStruct;
        strStruct << "@dataclass\n";
        strStruct << "class " << name << ":\n";
        for (size_t i = 0; i < vars.size(); ++i)
        {
            strStruct << "    " << vars[i].name << " = None\n";
        }
        return "\n" + strStruct.str();
    }

    std::string procedureRequirements() override
    {

        return "import psycopg2\nfrom generated_types import *\n";
    }

    std::string convertType(std::string &postgresType) override
    {
        return "";
    }

    std::string formatParameters(std::vector<VariableData> &params) override
    {
        std::ostringstream out;

        for (size_t i = 0; i < params.size(); ++i)
        {
            out << ", " << params[i].name;
        }

        return out.str();
    }

    std::string generateFunctionCall(std::string name, std::vector<VariableData> &params) override
    {

        if (params.size() == 0)
            return "\"" + name + "()\"";

        std::ostringstream call;
        call << "f\"" << name << "(";

        for (size_t i = 0; i < params.size(); ++i)
        {
            call << "{" << params[i].name << "}";
            call << (i == params.size() - 1 ? "" : ", ");
        }
        call << ")\"";
        return call.str();
    }

    std::string createProcedure(std::string name, std::string sql, std::vector<VariableData> &params) override
    {
        std::ostringstream out;

        std::string paramsString = this->formatParameters(params);
        out << "\ndef " << name << "(cur";
        out << paramsString << "):\n";
        out << "    cur.execute(" << sql << ")\n";

        return out.str();
    }

    std::string createFunction(std::string name, std::string sql, std::vector<VariableData> &params, TableData table) override
    {
        std::ostringstream out;
        std::string paramsString = this->formatParameters(params);

        out << "\ndef " << name << "(cur";
        out << paramsString << "):\n";

        out << "    cur.execute(" << sql << ")\n";

        out << "    data = []\n";
        out << "    colnames = [desc[0] for desc in cur.description]\n";
        out << "    for item in cur.fetchall():\n";
        out << "        x = " << table.name << "()\n";
        out << "        for idx, name in enumerate(colnames):\n";
        out << "            setattr(x, name, item[idx])\n";
        out << "        data.append(x)\n";

        out << "\n    return data\n";

        return out.str();
    }
};

class RustImplementation : public LanguageImplementation
{
    std::string fileExtension() override
    {
        return ".rs";
    }

    std::string typeRequirements() override
    {
        return "";
    }

    std::string typeGen(std::string &name, std::vector<VariableData> vars) override
    {
        std::ostringstream strStruct;
        strStruct << "struct " << name << " {\n";
        for (size_t i = 0; i < vars.size(); ++i)
        {
            strStruct << " " << vars[i].name << ": " << vars[i].type << ",\n";
        }
        strStruct << "\n}";
        return "\n" + strStruct.str();
    }

    std::string procedureRequirements() override
    {
        return "use sqlx;";
    }

    std::string convertType(std::string &postgresType) override
    {
        if (postgresType == "text")
            return "String";
        else if (postgresType == "integer")
            return "i32";
        else if (postgresType == "numeric")
            return "f64";
        else if (postgresType == "boolean")
            return "bool";
        std::cout << postgresType << " is not a valid type\n";
        return "BAD_TYPE";
    }
    std::string formatParameters(std::vector<VariableData> &params) override
    {
        std::ostringstream out;

        for (size_t i = 0; i < params.size(); ++i)
        {
            out << ", " << params[i].name << ": " << params[i].type;
        }

        return out.str();
    }

    std::string generateFunctionCall(std::string name, std::vector<VariableData> &params) override
    {
        if (params.size() == 0)
            return name + "()";

        std::ostringstream call;
        call << name << "(\"";

        for (size_t i = 0; i < params.size(); ++i)
        {
            if (params[i].type == "std::string")
                call << " + quote(" << params[i].name << ")";
            else if (params[i].type == "i32" || params[i].type == "f64")
                call << " + " << params[i].name << ".to_string()";

            call << (i == params.size() - 1 ? "" : R"(+ ", ")");
        }

        call << " + \")";
        return call.str();
    }

    std::string createProcedure(std::string name, std::string sql, std::vector<VariableData> &params)
    {
        std::ostringstream out;
        std::string paramsString = this->formatParameters(params);
        out << "\npub async fn " << name << "(pool: &sqlx::Pool<sqlx::Postgres>";
        out << paramsString << ") -> Result<(), sqlx::Error>\n";

        out << "{\n sqlx::raw_sql(";
        out << sql;
        out << ").execute(pool).await?;}";

        return out.str();
    }

    std::string createFunction(std::string name, std::string sql, std::vector<VariableData> &params, TableData table) override
    {
        std::ostringstream out;
        std::string paramsString = this->formatParameters(params);

        out << "\npub async fn " << name << "(pool: &sqlx::Pool<sqlx::Postgres>";
        out << paramsString << ") -> Result<Vec<" << table.name << ">, sqlx::Error> {\n";

        out << "{\n";
        out << "    let response: Vec<PgRow> = sqlx::query(" << sql << ").fetch_all(pool).await?;\n";
        out << "    let result: Vec<" << table.name << "> = Vec::new();\n";
        out << "    for i in 0..result.len() {\n";
        out << "        let x = " << table.name << " {\n";

        for (size_t i = 0; i < table.rows.size(); ++i)
        {
            out << table.rows[i].name << ": "
                << "response[i].try_get(" << i << ")?,";
        }
        out << "    };";
        out << "        result.push(x);\n";
        out << "    }";

        out << "\n	Ok(result);\n}";

        return out.str();
    }
};
#endif