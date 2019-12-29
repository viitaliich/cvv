#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <climits>

enum tokens_list{
    O_PRN,          //0
    C_PRN,          //1
    O_BRACE,        //2
    C_BRACE,        //3
    SEMI,           //4
    KEYWORD,        //5
    RETURN,         //6
    IDENTIFIER,     //7
    I_NUM,          //8
    ENDOFFILE,      //9
    ANEG,           //10
    COMPLEMENT,     //11
    LNEG,           //12
    ADD,            //13
    MUL,            //14
    DIV             //15
};

enum types_list {
    FUNC_DECL,
    CONSTATNT,
    RET,
    UN_OP,
    BI_OP
};
class AST{
private:
    int type;
    std::string func_name;
    std::string op;
    int inum;
    int inum_r;

public:
    AST(){
        type = INT_MAX;
        func_name = "";
        inum = 0;
    }
    // set AST fields
    void set_type(int value){
        type = value;
    }
    void set_func_name(std::string value){
        func_name = value;
    }
    void set_op(std::string value){
        op = value;
    }
    void set_inum(int value){
        inum = value;
    }
    void set_inum_r(int value){
        inum_r = value;
    }

    // check AST fields
    int check_type(){
        return type;
    }
    std::string check_func_name(){
        return func_name;
    }
    std::string check_op(){
        return op;
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

    void dec_cur(){
        current--;
    }

    std::vector<AST> out_ast(){
        return nodes;
    }

    Parser(){
        current = 0;
    }

public:
    void parse_factor(const tokens_t& tokens){
//        inc_cur();
        if (tokens[current].first == I_NUM){
            // Create AST node.
            AST node;
            node.set_type(CONSTATNT);
            node.set_inum(std::stoi(tokens[current].second));
            push_node(node);
            return;
        }

        if (tokens[current].first == ANEG || tokens[current].first == LNEG || tokens[current].first == COMPLEMENT){
            AST node;
            node.set_type(UN_OP);
            node.set_op(tokens[current].second);

            parse_factor(tokens);

            push_node(node);

            return;
        }

        if (tokens[current].first == O_PRN){
            parse_expr(tokens);
            inc_cur();
            if (tokens[current].first != C_PRN){
                std::cout << "No pair to open parentheses\n";
                exit(0);
            }
//            dec_cur();
            return;
        }

        std::cout << "Illegal value" << std::endl;
        exit(0);
    }

    void parse_term(const tokens_t& tokens){
//        inc_cur();

        parse_factor(tokens);
        inc_cur();
        while (tokens[current].first == MUL || tokens[current].first == DIV){

            AST node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
//            dec_cur();
            inc_cur();
            parse_factor(tokens);
            push_node(node);
//            parse_term(tokens);
        }
//        else{
//            dec_cur();
//        }
        dec_cur();

    }

    void parse_expr(const tokens_t& tokens){
        inc_cur();

        parse_term(tokens);
        inc_cur();
        while (tokens[current].first == ADD || tokens[current].first == ANEG){

            AST node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
//            dec_cur();
            inc_cur();
            parse_term(tokens);
            push_node(node);
            inc_cur();
//            parse_expr(tokens);
        }
//        else {
//            dec_cur();
//        }
        dec_cur();

    }

    void parse_ret(const tokens_t& tokens){
        parse_expr(tokens);
    }

    void parse_statement(const tokens_t& tokens){
        inc_cur();
        if (tokens[current].first != RETURN){
            std::cout << "Error at function return" << std::endl;
            exit(0);
        }
        parse_ret(tokens);

        AST node;
        node.set_type(RET);
        push_node(node);

        inc_cur();
        if (tokens[current].first != SEMI){
            std::cout << "No semicolon at the end of the statement" << std::endl;
            exit(0);
        }
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
        node.set_type(FUNC_DECL);
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
    std::string input = read_file(R"(D:\Winderton\Compiler_cvv\stage3_tests\valid\associativity.c)");
    tokens_t tokens = lexer(input);
    std::cout << "Lexer: no errors\n";
    out_tokens(tokens);
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
        switch (ast[current].check_type()){
            case FUNC_DECL:
                fprintf(pfile, "%s:\n", ast[current].check_func_name().c_str());
                current++;
                break;

            case CONSTATNT:
                fprintf(pfile, "\tmov eax, %d\n", ast[current].check_inum());
                fprintf(pfile, "\tpush eax\n");
                current++;
                break;

            case RET:
                fprintf(pfile, "\tret\n");
                current++;
                break;

            case UN_OP:
                if (ast[current].check_op() == "-"){
                    fprintf(pfile, "\tneg eax\n");
                    current++;
                    break;
                }
                if (ast[current].check_op() == "!"){
                    fprintf(pfile, "\tcmp eax, 0\n");      //set ZF on if exp == 0, set it off otherwise
                    fprintf(pfile, "\tmov eax, 0\n");     // zero out EAX (doesn't change FLAGS)
                    fprintf(pfile, "\tsete al\n");          //set AL register (the lower byte of EAX) to 1 if ZF is on
                    current++;
                    break;
                }
                if (ast[current].check_op() == "~"){
                    fprintf(pfile, "\tnot eax\n");
                    current++;
                    break;
                }

            case BI_OP:
                if (ast[current].check_op() == "+"){
//                    fprintf(pfile, "\tpush eax\n");
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tadd eax, ecx\n");
                    current++;
                    break;
                }

                if (ast[current].check_op() == "*"){
//                    fprintf(pfile, "\tpush eax\n");
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\timul eax, ecx\n");
                    current++;
                    break;
                }

                if (ast[current].check_op() == "-"){
//                    fprintf(pfile, "\tpush eax\n");
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tsub eax, ecx\n");
                    fprintf(pfile, "\tpush eax\n");
                    current++;
                    break;
                }

                if (ast[current].check_op() == "/"){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcdq\n");
                    fprintf(pfile, "\tidiv ecx\n");
                    fprintf(pfile, "\tmov eax, ecx\n");
                    current++;
                    break;
                }

            default:
                std::cout << "BI_ERROR";
                exit(0);
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

        // Addition
        if (symbol == '+'){
            std::string value;
            value += symbol;
            tokens.emplace_back(ADD, value);
            symbol = input[++current];
            continue;
        }

        // Multiplication
        if (symbol == '*'){
            std::string value;
            value += symbol;
            tokens.emplace_back(MUL, value);
            symbol = input[++current];
            continue;
        }

        // Division
        if (symbol == '/'){
            std::string value;
            value += symbol;
            tokens.emplace_back(DIV, value);
            symbol = input[++current];
            continue;
        }

        // Arithmetical negation
        if (symbol == '-'){
            std::string value;
            value += symbol;
            tokens.emplace_back(ANEG, value);
            symbol = input[++current];
            continue;
        }

        // Logical negation
        if (symbol == '!'){
            std::string value;
            value += symbol;
            tokens.emplace_back(LNEG, value);
            symbol = input[++current];
            continue;
        }

        // Bitwise complement
        if (symbol == '~'){
            std::string value;
            value += symbol;
            tokens.emplace_back(COMPLEMENT, value);
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
        if (symbol == '`' || symbol == '@' || symbol == '#' || symbol == '$' || symbol == '?' ||
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

