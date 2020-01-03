#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <climits>
#include <stack>

#define LABEL_OFFSET 1000

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
    DIV,            //15
    AND,            //16
    OR,             //17
    EQU,            //18
    NEQU,           //19
    LESS,           //20
    LESSEQU,        //21
    GREAT,          //22
    GREATEQU,       //23
    ASSIGN,         //24
    QUESTION,       //25
    COLON,          //26
    IF,             //27
    ELSE            //28
};

enum types_list {
    FUNC_DECL,      //0
    CONSTANT,       //1
    RET,            //2
    UN_OP,          //3
    BI_OP,          //4
    VARREF,         //5
    VARASSIGN,      //6
    VARDECL,        //7
    COND_QUEST,     //8
    COND_COLON,     //9
    COND_END,       //10
    IF_ELSE,        //11
    IF_BODY,        //12
    IF_END          //13
};

class AST{
private:
    int type;                   // node type
    std::string func_name;
    std::string var_name;
    std::string op;
    int inum;
    int inum_r;

public:
    AST(){
        type = INT_MAX;
        func_name = "";
        var_name = "";
        op = "";
        inum = 0;
        inum_r = 0;
    }
    // set AST fields
    void set_type(int value){
        type = value;
    }
    void set_func_name(std::string value){
        func_name = value;
    }
    void set_var_name(std::string value){
        var_name = value;
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
    std::string check_var_name(){
        return var_name;
    }
    std::string check_op(){
        return op;
    }
    int check_inum(){
        return inum;
    }
    int check_inum_r(){
        return inum_r;
    }

    // Print AST node to console
    void print_ast(){
        std::cout << type << std::endl;
        std::cout << func_name << std::endl;
        std::cout << inum << std::endl;
        std::cout << "***" << std::endl;
    }
};

typedef std::vector<std::pair<std::string, int>> dvar_t;
typedef std::vector<std::pair<int, std::string>> tokens_t;
tokens_t lexer(const std::string& input);
std::vector<AST> parser(const tokens_t& tokens);
void to_asm(std::vector<AST>& ast);
std::string read_file(const std::string& file_location);

int find_var(const std::string& key, const dvar_t& decl_vars);
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
        if (tokens[current].first == I_NUM){
            // Create AST node.
            AST node;
            node.set_type(CONSTANT);
            node.set_inum(std::stoi(tokens[current].second));
            push_node(node);
            return;
        }

        if (tokens[current].first == IDENTIFIER){
            // Create AST node.
            AST node;
            node.set_type(VARREF);
            node.set_var_name(tokens[current].second);
            push_node(node);
            return;
        }

        if (tokens[current].first == ANEG || tokens[current].first == LNEG || tokens[current].first == COMPLEMENT){
            AST node;
            node.set_type(UN_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_factor(tokens);
            push_node(node);
            return;
        }

        if (tokens[current].first == O_PRN){
            inc_cur();
            parse_expr(tokens);
            //inc_cur();
            if (tokens[current].first != C_PRN){
                std::cout << "No pair to open parentheses\n";
                exit(0);
            }
            return;
        }

        std::cout << "Illegal value" << std::endl;
        exit(0);
    }

    void parse_term(const tokens_t& tokens){
        parse_factor(tokens);
        inc_cur();
        while (tokens[current].first == MUL || tokens[current].first == DIV){
            AST node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_factor(tokens);
            push_node(node);
            inc_cur();
        }
        dec_cur();
    }

    void parse_additive(const tokens_t& tokens){
        parse_term(tokens);
        inc_cur();
        while (tokens[current].first == ADD || tokens[current].first == ANEG){
            AST node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_term(tokens);
            push_node(node);
            inc_cur();
        }
        dec_cur();
    }

    void parse_relational(const tokens_t& tokens){
        parse_additive(tokens);
        inc_cur();
        while (tokens[current].first == LESS || tokens[current].first == LESSEQU ||
                tokens[current].first == GREAT || tokens[current].first == GREATEQU){
            AST node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_additive(tokens);
            push_node(node);
            inc_cur();
        }
        dec_cur();
    }

    void parse_equ(const tokens_t& tokens){
        parse_relational(tokens);
        inc_cur();
        while (tokens[current].first == NEQU || tokens[current].first == EQU){
            AST node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_relational(tokens);
            push_node(node);
            inc_cur();
        }
        dec_cur();
    }

    void parse_log_and(const tokens_t& tokens){
        parse_equ(tokens);
        inc_cur();
        while (tokens[current].first == AND){
            AST node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_equ(tokens);
            push_node(node);
            inc_cur();
        }
        dec_cur();
    }

    void parse_log_or(const tokens_t& tokens){
        parse_log_and(tokens);
        inc_cur();
        while (tokens[current].first == OR){
            AST node;
            node.set_type(BI_OP);
            node.set_op(tokens[current].second);
            inc_cur();
            parse_log_and(tokens);
            push_node(node);
            inc_cur();
        }
        dec_cur();
    }

    void parse_conditional(const tokens_t& tokens){
        parse_log_or(tokens);

        inc_cur();
        if (tokens[current].first == QUESTION){
            AST node_cond_quest;
            node_cond_quest.set_type(COND_QUEST);
            push_node(node_cond_quest);

            inc_cur();
            parse_expr(tokens);
            //inc_cur();
            if (tokens[current].first != COLON){
                std::cout << "No colon in ternary expression" << std::endl;
                exit(0);
            }
            AST node_cond_colon;
            node_cond_colon.set_type(COND_COLON);
            push_node(node_cond_colon);

            inc_cur();
            parse_conditional(tokens);

            AST node_cond_end;
            node_cond_end.set_type(COND_END);
            push_node(node_cond_end);
        }
    }
    void parse_expr(const tokens_t& tokens){
        AST node;
        node.set_type(VARASSIGN);
        node.set_var_name(tokens[current].second);

        AST node_ref;
        node_ref.set_type(VARREF);
        node_ref.set_var_name(tokens[current].second);

        inc_cur();
        if (tokens[current].first == ASSIGN){
            inc_cur();

            parse_expr(tokens);

            push_node(node);

            push_node(node_ref);
        } else {
            dec_cur();
            parse_conditional(tokens);
        }
    }

    void parse_statement(const tokens_t& tokens){
        if (tokens[current].first == RETURN){
            inc_cur();
            parse_expr(tokens);
            //inc_cur();
            if (tokens[current].first != SEMI){
                std::cout << "No semicolon at the end of the statement1" << std::endl;
                exit(0);
            }
            AST node;
            node.set_type(RET);
            push_node(node);
            return;
        }

        // Variable assignment
        if (tokens[current].first == IDENTIFIER || tokens[current].first == I_NUM ||
            tokens[current].first == ANEG || tokens[current].first == LNEG || tokens[current].first == COMPLEMENT ||
            tokens[current].first == O_PRN){
            parse_expr(tokens);
            //inc_cur();
            if (tokens[current].first != SEMI){
                std::cout << "No semicolon at the end of the statement2" << std::endl;
                exit(0);
            }
            return;
        }

        // If - Else
        if (tokens[current].first == IF){
            inc_cur();
            if (tokens[current].first != O_PRN){
                std::cout << "No open parentheses in IF statement" << std::endl;
                exit(0);
            }

            inc_cur();
            parse_expr(tokens);
            //inc_cur();

            if (tokens[current].first != C_PRN){
                std::cout << "No close parentheses in IF statement" << std::endl;
                exit(0);
            }

            AST node_if;
            node_if.set_type(IF_ELSE);
            push_node(node_if);

            inc_cur();
            parse_statement(tokens);
            inc_cur();

            AST node_body;
            node_body.set_type(IF_BODY);
            push_node(node_body);

            if (tokens[current].first == ELSE){
                inc_cur();
                parse_statement(tokens);

            } else {
                dec_cur();
            }
            AST node_if_end;
            node_if_end.set_type(IF_END);
            push_node(node_if_end);

            //dec_cur();      // ???
            return;
        }

        std::cout << "Wrong statement" << std::endl;
        exit(0);

        /*dec_cur();      // in case of no return statement   */
    }

    void parse_block_item(const tokens_t& tokens){
        // Variable declaration
        if (tokens[current].first == KEYWORD){
            inc_cur();
            if (tokens[current].first != IDENTIFIER){
                std::cout << "Wrong name in variable declaration" << std::endl;
                exit(0);
            }
            AST node;
            node.set_type(VARDECL);
            node.set_var_name(tokens[current].second);
            push_node(node);

            AST node_assign;
            node_assign.set_type(VARASSIGN);
            node_assign.set_var_name(tokens[current].second);

            inc_cur();
            if (tokens[current].first == ASSIGN){
                inc_cur();
                parse_expr(tokens);
                push_node(node_assign);
                //inc_cur();
            }
//            push_node(node);

            if (tokens[current].first != SEMI){
                std::cout << "No semicolon at the end of the statement3" << std::endl;
                exit(0);
            }
            return;
        } else {
            parse_statement(tokens);
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
        inc_cur();
        parse_block_item(tokens);
        inc_cur();
        while (tokens[current].first != C_BRACE){
            parse_block_item(tokens);
            inc_cur();
        }

//        if (tokens[current].first != C_BRACE){
//            std::cout << "No close brace at function declaration" << std::endl;
//            exit(0);
//        }
    }
};

int main (int argc, char ** argv){
    std::string input = read_file(R"(D:\Winderton\Compiler_cvv\stage6_tests\valid\expression\ternary_short_circuit_2.c)");
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

    int label = 0;
    int stack_index = -4;
    dvar_t decl_vars;
    std::stack<int> labels;

    size_t current = 0;
    while (current < ast.size()){
        switch (ast[current].check_type()){
            case VARDECL:
                if (find_var(ast[current].check_var_name(), decl_vars) != 0){
                    std::cout << "Variable is already defined" << std::endl;
                    exit(0);
                }
                decl_vars.emplace_back(ast[current].check_var_name(), stack_index);
                //stack_index -= 4;
                current++;
                break;

            case VARASSIGN:
                if (find_var(ast[current].check_var_name(), decl_vars) == 0){
                    std::cout << "Variable is not defined1" << std::endl;
                    exit(0);
                }
                fprintf(pfile, "\tmov [ebp + %d], eax\n", find_var(ast[current].check_var_name(), decl_vars));
                current++;
                break;

            case VARREF:
                if (find_var(ast[current].check_var_name(), decl_vars) == 0){
                    std::cout << "Variable is not defined2" << std::endl;
                    exit(0);
                }
                fprintf(pfile, "\tmov eax, [ebp + %d]\n", find_var(ast[current].check_var_name(), decl_vars));
                fprintf(pfile, "\tpush eax\n");
                stack_index += 4;
                current++;
                break;

            case FUNC_DECL:
                fprintf(pfile, "%s:\n", ast[current].check_func_name().c_str());
                fprintf(pfile, "\tpush ebp\n");     //save old value of EBP
                fprintf(pfile, "\tmov ebp, esp\n"); //current top of stack is bottom of new stack frame
                current++;
                break;

            case COND_QUEST:
                fprintf(pfile, "\tpop eax\n");
                fprintf(pfile, "\tcmp eax, 0\n");
                fprintf(pfile, "\tje label%d\n", label);
                labels.push(label);
                current++;
                label++;
                break;

            case COND_COLON:
                fprintf(pfile, "\tjmp label%d\n", label);
//                label--;
                fprintf(pfile, "\tlabel%d:\n", labels.top());
                labels.pop();
                labels.push(label);
                label++;
                current++;
                break;

            case COND_END:
                fprintf(pfile, "\tlabel%d:\n", labels.top());
                labels.pop();
                label++;
                current++;
                break;

            case IF_ELSE:
                fprintf(pfile, "\tpop eax\n");
                fprintf(pfile, "\tcmp eax, 0\n");
                fprintf(pfile, "\tje label%d\n", label);
                labels.push(label);
                current++;
                label++;
                break;

            case IF_BODY:
                fprintf(pfile, "\tjmp label%d\n", label);
//                label--;
                fprintf(pfile, "\tlabel%d:\n", labels.top());
                labels.pop();
                labels.push(label);
                label++;
                current++;
                break;

            case IF_END:
                fprintf(pfile, "\tlabel%d:\n", labels.top());
                labels.pop();
                label++;
                current++;
                break;

            case CONSTANT:
                fprintf(pfile, "\tmov eax, %d\n", ast[current].check_inum());
                fprintf(pfile, "\tpush eax\n");
                stack_index -= 4;
                current++;
                break;

            case RET:
                fprintf(pfile, "\tmov esp, ebp\n"); //restore ESP; now it points to old EBP
                fprintf(pfile, "\tpop ebp\n");  // restore old EBP; now ESP is where it was before prologue
                fprintf(pfile, "\tret\n");
                current++;
                break;

            case UN_OP:
                if (ast[current].check_op() == "-"){
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tneg eax\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }
                if (ast[current].check_op() == "!"){
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcmp eax, 0\n");      //set ZF on if exp == 0, set it off otherwise
                    fprintf(pfile, "\tmov eax, 0\n");     // zero out EAX (doesn't change FLAGS)
                    fprintf(pfile, "\tsete al\n");          //set AL register (the lower byte of EAX) to 1 if ZF is on
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }
                if (ast[current].check_op() == "~"){
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tnot eax\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

            case BI_OP:
                if (ast[current].check_op() == "=="){
//                    fprintf(pfile, "\tpush eax\n");
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcmp eax, ecx\n");     //set ZF on if e1 == e2, set it off otherwise
                    fprintf(pfile, "\tmov eax, 0\n");       //zero out EAX
                    fprintf(pfile, "\tsete al\n");          //set AL if ZF is on
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "!="){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcmp eax, ecx\n");     //set ZF on if e1 == e2, set it off otherwise
                    fprintf(pfile, "\tmov eax, 0\n");       //zero out EAX
                    fprintf(pfile, "\tsetne al\n");          //set AL if ZF is on
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "<"){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcmp eax, ecx\n");     //set ZF on if e1 == e2, set it off otherwise
                    fprintf(pfile, "\tmov eax, 0\n");       //zero out EAX
                    fprintf(pfile, "\tsetl al\n");          //set AL if ZF is on
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == ">"){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcmp eax, ecx\n");     //set ZF on if e1 == e2, set it off otherwise
                    fprintf(pfile, "\tmov eax, 0\n");       //zero out EAX
                    fprintf(pfile, "\tsetg al\n");          //set AL if SF is on
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "<="){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcmp eax, ecx\n");     //set ZF on if e1 == e2, set it off otherwise
                    fprintf(pfile, "\tmov eax, 0\n");       //zero out EAX
                    fprintf(pfile, "\tsetle al\n");          //set AL if SF is on
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == ">="){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcmp eax, ecx\n");     //set ZF on if e1 == e2, set it off otherwise
                    fprintf(pfile, "\tmov eax, 0\n");       //zero out EAX
                    fprintf(pfile, "\tsetge al\n");          //set AL if SF is on
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "||"){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcmp eax, 0\n");           //check if e1 is true
                    fprintf(pfile, "\tje label%d\n", label);    //e1 is 0, so we need to evaluate clause 2
                    fprintf(pfile, "\tmov eax, 1\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    fprintf(pfile, "\tjmp end_label%d\n", label);
                    fprintf(pfile, "\tlabel%d:\n", label);
                    fprintf(pfile, "\tmov eax, ecx\n");
                    fprintf(pfile, "\tcmp eax, 0\n");
                    fprintf(pfile, "\tmov eax, 0\n");           //zero out EAX
                    fprintf(pfile, "\tsetne al\n");             //set AL if e2 != 0
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    fprintf(pfile, "\tend_label%d:\n", label);
                    current++;
                    label++;
                    break;
                }

                if (ast[current].check_op() == "&&"){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcmp eax, 0\n");           //check if e1 is true
                    fprintf(pfile, "\tjne label%d\n", label);    //e1 is not 0, so we need to evaluate clause 2
                    fprintf(pfile, "\tjmp end_label%d\n", label);
                    fprintf(pfile, "\tlabel%d:\n", label);
                    fprintf(pfile, "\tmov eax, ecx\n");
                    fprintf(pfile, "\tcmp eax, 0\n");           //check if e2 is true
                    fprintf(pfile, "\tmov eax, 0\n");           //zero out EAX
                    fprintf(pfile, "\tsetne al\n");             //set AL if e2 != 0
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    fprintf(pfile, "\tend_label%d:\n", label);
                    current++;
                    label++;
                    break;
                }

                if (ast[current].check_op() == "+"){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tadd eax, ecx\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "*"){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\timul eax, ecx\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "-"){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tsub eax, ecx\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "/"){
                    fprintf(pfile, "\tpop ecx\n");
                    fprintf(pfile, "\tpop eax\n");
                    fprintf(pfile, "\tcdq\n");
                    fprintf(pfile, "\tidiv ecx\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
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
            while (symbol != '\n' && symbol != '\r'){
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

        // Colon
        if (symbol == ':'){
            std::string value;
            value += symbol;
            tokens.emplace_back(COLON, value);
            symbol = input[++current];
            continue;
        }

        // Question mark
        if (symbol == '?'){
            std::string value;
            value += symbol;
            tokens.emplace_back(QUESTION, value);
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

        // Logical AND
        if (symbol == '&'){
            std::string value;
            value += symbol;
            symbol = input[++current];
            while (symbol == '&'){
                value += symbol;
                symbol = input[++current];
            }
            if (value == "&&"){
                tokens.emplace_back(AND, value);
            }
            continue;
        }

        // Logical OR
        if (symbol == '|'){
            std::string value;
            value += symbol;
            symbol = input[++current];
            while (symbol == '|'){
                value += symbol;
                symbol = input[++current];
            }
            if (value == "||"){
                tokens.emplace_back(OR, value);
            }
            continue;
        }

        // Equal to
        if (symbol == '='){
            std::string value;
            value += symbol;
            symbol = input[++current];
            while (symbol == '='){
                value += symbol;
                symbol = input[++current];
            }
            if (value == "="){
                tokens.emplace_back(ASSIGN, value);
            }
            if (value == "=="){
                tokens.emplace_back(EQU, value);
            }
            continue;
        }

        // Not Equal to, Logical negation
        if (symbol == '!'){
            std::string value;
            value += symbol;
            symbol = input[++current];
            while (symbol == '='){
                value += symbol;
                symbol = input[++current];
            }
            if (value == "!="){
                tokens.emplace_back(NEQU, value);
            }
            if (value == "!"){
                tokens.emplace_back(LNEG, value);
            }
            continue;
        }

        // Lesser Equal then
        if (symbol == '<'){
            std::string value;
            value += symbol;
            symbol = input[++current];
            while (symbol == '='){
                value += symbol;
                symbol = input[++current];
            }
            if (value == "<="){
                tokens.emplace_back(LESSEQU, value);
            }
            if (value == "<"){
                tokens.emplace_back(LESS, value);
            }
            continue;
        }

        // Greater Equal then
        if (symbol == '>'){
            std::string value;
            value += symbol;
            symbol = input[++current];
            while (symbol == '='){
                value += symbol;
                symbol = input[++current];
            }
            if (value == ">="){
                tokens.emplace_back(GREATEQU, value);
            }
            if (value == ">"){
                tokens.emplace_back(GREAT, value);
            }
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
            if (value == "return") {
                tokens.emplace_back(RETURN, value);
                continue;
            }
            if (value == "if") {
                tokens.emplace_back(IF, value);
                continue;
            }
            if (value == "else") {
                tokens.emplace_back(ELSE, value);
                continue;
            }
            tokens.emplace_back(IDENTIFIER, value);
            continue;
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

int find_var(const std::string& key, const dvar_t& decl_vars){
    for (size_t i = 0; i < decl_vars.size(); i++){
        if (key == decl_vars[i].first){
            return decl_vars[i].second;
        }
    }
    return 0;
}