#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <algorithm>

bool isValidIdentifier(const std::string &identifier)
{
    std::regex identifierRegex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    return std::regex_match(identifier, identifierRegex);
}

std::string cdecl_translate(const std::string &declaration)
{
    std::string input = declaration;

    std::replace(input.begin(), input.end(), ';', ' ');
    input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end());

    std::string description = "declare ";
    std::string baseType;
    std::string identifier;
    std::string pointerPart;
    std::string arrayPart;
    std::string functionPart;

    std::smatch match;
    std::regex baseTypeRegex("^(int|char|float|double|void)");

    if (std::regex_search(input, match, baseTypeRegex))
    {
        baseType = match[0].str();
        input = match.suffix().str();
    }
    else
    {
        return "Syntax error: Invalid base type.";
    }

    std::regex funcPtrRegex("^\\(\\*\\s*([a-zA-Z_][a-zA-Z0-9_]*)\\)\\((.*)\\)");

    std::regex funcRegex("^\\(?([a-zA-Z_][a-zA-Z0-9_]*)\\)?\\((.*)\\)");

    if (std::regex_search(input, match, funcPtrRegex))
    {
        identifier = match[1].str();
        if (!isValidIdentifier(identifier))
        {
            return "Syntax error: invalid identifier.";
        }
        std::string args = match[2].str();
        description += identifier + " as pointer to function";
        if (!args.empty() && args != "void")
        {
            description += " accepting arguments: " + args;
        }
        description += " returning " + baseType;
        return description;
    }
    else if (std::regex_search(input, match, funcRegex))
    {
        identifier = match[1].str();
        if (!isValidIdentifier(identifier))
        {
            return "Syntax error: invalid identifier.";
        }
        std::string args = match[2].str();
        description += identifier + " as function";
        if (!args.empty() && args != "void")
        {
            description += " accepting arguments: " + args;
        }
        description += " returning " + baseType;
        return description;
    }

    std::regex pointerRegex("^(\\*+)");
    if (std::regex_search(input, match, pointerRegex))
    {
        pointerPart = match[0].str();
        input = match.suffix().str();
    }

    std::regex identifierRegex("^([a-zA-Z_][a-zA-Z0-9_]*)");
    if (std::regex_search(input, match, identifierRegex))
    {
        identifier = match[0].str();
        if (!isValidIdentifier(identifier))
        {
            return "Syntax error: invalid identifier.";
        }
        input = match.suffix().str();
    }
    else
    {

        return "Syntax error: Invalid identifier.";
    }

    description += identifier + " as ";

    if (!pointerPart.empty())
    {
        for (char p : pointerPart)
        {
            if (p == '*')
            {
                description += "pointer to ";
            }
        }
    }

    std::regex arrayRegex("^\\[([0-9]*)\\]");
    while (std::regex_search(input, match, arrayRegex))
    {
        std::string size = match[1].str();
        if (size.empty())
        {
            arrayPart += "array of ";
        }
        else
        {
            arrayPart += "array of " + size + " elements of ";
        }
        input = match.suffix().str();
    }
    if (!arrayPart.empty())
    {
        description += arrayPart;
    }

    description += baseType;
    return description;
}

void interpretFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::cout << cdecl_translate(line) << std::endl;
    }

    file.close();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    interpretFile(filePath);

    return 0;
}