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
public:
    std::string fileExtension() override
    {
        return ".h";
    }

    std::string typeRequirements() override
    {
        std::cout << "new one called\n";
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
            << "#include \"types.h\"\n\n"
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
#endif