#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <climits>

class AST{
private:
    std::string type;
    std::string func_name;
    int inum;

public:
    AST(){
        type = "";
        func_name = "";
        inum = 0;
    }
    // set AST fields
    void set_type(std::string value){
        type = value;
    }
    void set_func_name(std::string value){
        func_name = value;
    }
    void set_inum(int value){
        inum = value;
    }

    // check AST fields
    std::string check_type(){
        return type;
    }
    std::string check_func_name(){
        return func_name;
    }
    int check_inum(){
        return inum;
    }

    // Print AST node to console
    void print_ast(){
        std::cout << type << std::endl;
        std::cout << func_name << std::endl;
        std::cout << inum << std::endl;
        std::cout << "***" << std::endl;
    }
};

typedef std::vector<std::pair<int, std::string>> tokens_t;
tokens_t lexer(const std::string& input);
std::vector<AST> parser(const tokens_t& tokens);
void to_asm(std::vector<AST>& ast);
std::string read_file(const std::string& file_location);

bool find_str_vec(const std::string& key, const std::vector<std::string>& vector);
void out_tokens(const tokens_t& tokens);

enum tokens_list{
    O_PRN,
    C_PRN,
    O_BRACE,
    C_BRACE,
    SEMI,
    KEYWORD,
    RETURN,
    IDENTIFIER,
    I_NUM,
    ENDOFFILE
};


class Parser{
private:
    size_t current;
    std::vector<AST> nodes;

public:
    // Push node to AST
    void push_node(const AST& node){
        nodes.emplace_back(node);
    }

    // Take values of AST fields
    size_t out_cur(){
        return current;
    }

    void inc_cur(){
        current++;
    }

    std::vector<AST> out_ast(){
        return nodes;
    }

    Parser(){
        current = 0;
    }

public:
    void parse_expr(const tokens_t& tokens){
        inc_cur();
        if (tokens[current].first != I_NUM){
            std::cout << "Illegal return value" << std::endl;
            exit(0);
        }

        // Create AST node.
        AST node;
        node.set_type("constant");
        node.set_inum(std::stoi(tokens[current].second));
        push_node(node);
    }

    void parse_ret(const tokens_t& tokens){
        parse_expr(tokens);

        inc_cur();
        if (tokens[current].first != SEMI){
            std::cout << "No semicolon at the end of the statement" << std::endl;
            exit(0);
        }
    }
    void parse_statement(const tokens_t& tokens){
        inc_cur();
        if (tokens[current].first != RETURN){
            std::cout << "Error at function return" << std::endl;
            exit(0);
        }
        parse_ret(tokens);

        AST node;
        node.set_type("ret");
        push_node(node);
    }

    void parse_function(const tokens_t& tokens){
        if (tokens[current].first != KEYWORD){
            std::cout << "Wrong type at function declaration" << std::endl;
            exit(0);
        }

        inc_cur();
        if (tokens[current].first != IDENTIFIER){
            std::cout << "Illegal name of function" << std::endl;
            exit(0);
        }
        AST node;
        node.set_type("function_call");
        node.set_func_name(tokens[current].second);
        push_node(node);

        inc_cur();
        if (tokens[current].first != O_PRN){
            std::cout << "No open parentheses at function declaration" << std::endl;
            exit(0);
        }

        inc_cur();
        if (tokens[current].first != C_PRN){
            std::cout << "No close parentheses at function declaration" << std::endl;
            exit(0);
        }

        inc_cur();
        if (tokens[current].first != O_BRACE){
            std::cout << "No open brace at function declaration" << std::endl;
            exit(0);
        }

        parse_statement(tokens);

        inc_cur();
        if (tokens[current].first != C_BRACE){
            std::cout << "No close brace at function declaration" << std::endl;
            exit(0);
        }
    }
};

int main (int argc, char ** argv){
    std::string input = read_file(R"(D:\Winderton\Compiler_cvv\return2.txt)");
    tokens_t tokens = lexer(input);
    std::cout << "Lexer: no errors\n";
//    out_tokens(tokens);
    std::vector<AST> nodes = parser(tokens);
    std::cout << "Parser: no errors\n";
//    for (int i = 0; i < nodes.size(); i++){
//        nodes[i].print_ast();
//    }
    to_asm(nodes);
    std::cout << "Done\n";
    return 0;
}

void to_asm(std::vector<AST>& ast){
    // File where assembly code is written to.
    FILE *pfile;
    pfile = fopen("asm.txt", "w");

    size_t current = 0;
    while (current < ast.size()){
        if (ast[current].check_type() == "function_call"){
            fprintf(pfile, "%s:\n", ast[current].check_func_name().c_str());
            current++;
            continue;
        }
        if (ast[current].check_type() == "constant"){
            fprintf(pfile, "\tmov eax, %d\n", ast[current].check_inum());
            current++;
            continue;
        }
        if (ast[current].check_type() == "ret"){
            fprintf(pfile, "\tret\n");
            current++;
            continue;
        }
    }
    fclose(pfile);
}

std::vector<AST> parser(const tokens_t& tokens){
    Parser parser;
    while (parser.out_cur() < tokens.size()-1){
        parser.parse_function(tokens);
        parser.inc_cur();
    }
    std::vector<AST> nodes = parser.out_ast();
    return nodes;
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
            tokens.emplace_back(O_PRN, value);
            symbol = input[++current];
            continue;
        }

        // Close parentheses
        if (symbol == ')'){
            std::string value;
            value += symbol;
            tokens.emplace_back(C_PRN, value);
            symbol = input[++current];
            continue;
        }

        // Open brace
        if (symbol == '{'){
            std::string value;
            value += symbol;
            tokens.emplace_back(O_BRACE, value);
            symbol = input[++current];
            continue;
        }

        // Close brace
        if (symbol == '}'){
            std::string value;
            value += symbol;
            tokens.emplace_back(C_BRACE, value);
            symbol = input[++current];
            continue;
        }

        // Semicolon
        if (symbol == ';'){
            std::string value;
            value += symbol;
            tokens.emplace_back(SEMI, value);
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
                tokens.emplace_back(KEYWORD, value);
                continue;
            }
            if (find_str_vec(value, key_return)) {
                tokens.emplace_back(RETURN, value);
                continue;
            }
            if (!find_str_vec(value, keywords) && !find_str_vec(value, key_return)) {
                tokens.emplace_back(IDENTIFIER, value);
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
                tokens.emplace_back(I_NUM, value);
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

    tokens.emplace_back(ENDOFFILE, "EOF");

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

