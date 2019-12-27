#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <climits>

typedef std::vector<std::pair<std::string, std::string>> tokens_t;

bool find_str_vec(const std::string& key, const std::vector<std::string>& vector);
void out_tokens(const tokens_t& tokens);
std::string read_file(const std::string& file_location);
tokens_t lexer(const std::string& input);
void parser(const tokens_t& tokens);

int parse_function(const tokens_t& tokens, size_t current);
int parse_statement(tokens_t tokens, size_t current);

int main (int argc, char ** argv){
    std::string input = read_file(R"(D:\Winderton\Compiler_cvv\return2.txt)");
    tokens_t tokens = lexer(input);
    out_tokens(tokens);
    parser(tokens);
    return 0;
}

int parse_statement(tokens_t tokens, size_t current){
    if (tokens[++current].first != "RETURN"){
        std::cout << "Error at function return" << std::endl;
        exit(0);
    }
    if (tokens[++current].first != "I_NUM"){
        std::cout << "Illegal return value" << std::endl;
        exit(0);
    }
    if (tokens[++current].first != "SEMI"){
        std::cout << "No semicolon at the end of the statement" << std::endl;
        exit(0);
    }
    return current;
}

int parse_function(const tokens_t& tokens, size_t current){
    if (tokens[current].first != "KEYWORD"){
        std::cout << "Wrong type at function declaration" << std::endl;
        exit(0);
    }
    if (tokens[++current].first != "IDENTIFIER"){
        std::cout << "Illegal name of function" << std::endl;
        exit(0);
    }
    if (tokens[++current].first != "O_PRN"){
        std::cout << "No open parentheses at function declaration" << std::endl;
        exit(0);
    }
    if (tokens[++current].first != "C_PRN"){
        std::cout << "No close parentheses at function declaration" << std::endl;
        exit(0);
    }
    if (tokens[++current].first != "O_BRACE"){
        std::cout << "No open brace at function declaration" << std::endl;
        exit(0);
    }

    current = parse_statement(tokens, current);

    if (tokens[++current].first != "C_BRACE"){
        std::cout << "No close brace at function declaration" << std::endl;
        exit(0);
    }

    return current;
}

void parser(const tokens_t& tokens){
    size_t current = 0;
    while (current < tokens.size()-1){

        current = parse_function(tokens, current);
        current++;
    }

}

tokens_t lexer(const std::string& input){
    const std::vector<std::string> keywords = {"int"};    // the list of keywords
    const std::vector<std::string> key_return = {"return"};    // return keyword

    size_t current = 0;     //for tracking our position in the code
    tokens_t tokens;        //list of tokens

    while (current < input.size()){
        char symbol = input[current];   // current symbol

        // Cyrillic
        if (int(symbol) < 0 || int(symbol) > 127){
            std::cout << "Undefined symbol " << symbol << std::endl;
            exit(0);
        }

        // Comments /*...*/
        if (symbol == '/' && input[current + 1] == '*'){
            current += 2;
            symbol = input[current];

            while (current < input.size() && symbol != '*' && input[current + 1] != '/'){
                symbol = input[++current];
            }
            current += 2;
            continue;
        }

        // Comments //...
        if (symbol == '/' && input[current + 1] == '/'){
            while (symbol != '\n' && symbol != '\r'){       //???
                symbol = input[++current];
            }
            current++;
            continue;
        }

        // Open parentheses
        if (symbol == '('){
            std::string value;
            value += symbol;
            tokens.emplace_back("O_PRN", value);
            symbol = input[++current];
            continue;
        }

        // Close parentheses
        if (symbol == ')'){
            std::string value;
            value += symbol;
            tokens.emplace_back("C_PRN", value);
            symbol = input[++current];
            continue;
        }

        // Open brace
        if (symbol == '{'){
            std::string value;
            value += symbol;
            tokens.emplace_back("O_BRACE", value);
            symbol = input[++current];
            continue;
        }

        // Close brace
        if (symbol == '}'){
            std::string value;
            value += symbol;
            tokens.emplace_back("C_BRACE", value);
            symbol = input[++current];
            continue;
        }

        // Semicolon
        if (symbol == ';'){
            std::string value;
            value += symbol;
            tokens.emplace_back("SEMI", value);
            symbol = input[++current];
            continue;
        }

        // Keywords and Identifiers
        if (std::isalpha(symbol)){
            std::string value;
            value += symbol;
            symbol = input[++current];
            while (std::isalnum(symbol) || symbol == '_'){
                value += symbol;
                symbol = input[++current];
            }
            if (find_str_vec(value, keywords)){
                tokens.emplace_back("KEYWORD", value);
                continue;
            }
            if (find_str_vec(value, key_return)) {
                tokens.emplace_back("RETURN", value);
                continue;
            }
            if (!find_str_vec(value, keywords) && !find_str_vec(value, key_return)) {
                tokens.emplace_back("IDENTIFIER", value);
                continue;
            }
        }

        // I_Numbers
        if (std::isdigit(symbol)){
            std::string value;
            value += symbol;
            symbol = input[++current];
            while (std::isdigit(symbol) || symbol == '_'){                                                      // shit
                if (symbol != '_') value += symbol;
                symbol = input[++current];
            }
            if (std::isalpha(symbol)){
                while (std::isalpha(symbol)){
                    value += symbol;
                    symbol = input[++current];
                }
                std::cout << "Wrong identifier " << value << std::endl;
                exit(0);
            } else {
                tokens.emplace_back("I_NUM", value);
                continue;
            }
        }

        // Illegal symbols
        if (symbol == '~' || symbol == '`' || symbol == '@' || symbol == '#' || symbol == '$' || symbol == '?' ||
            symbol == '\\' || symbol == '_'){
            std::cout << "Illegal symbol " << symbol << std::endl;
            exit(0);
        }

        current++;

    }

    tokens.emplace_back("EOF", "EOF");

    return tokens;
}

std::string read_file(const std::string& file_location){
    std::string input;
    std::string current_line;
    std::ifstream file (file_location);     // open file
    if (file.is_open()){
        while (!file.eof()){
            getline(file, current_line);
            input += current_line;
        }
    }
    file.close();
    return input;
}

void out_tokens(const tokens_t& tokens){
    for (size_t i = 0; i < tokens.size(); i++){
        std::cout << tokens[i].first << " -> " << tokens[i].second << std::endl;
    }
}

bool find_str_vec(const std::string& key, const std::vector<std::string>& vector){
    bool flag = false;
    for (size_t i = 0; i < vector.size(); i++){
        if (key == vector[i]){
            flag = true;
            break;
        }
    }
    if (flag){
        return true;
    } else return false;
}

