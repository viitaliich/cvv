#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <climits>
#include <stack>

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
    ELSE,           //28
    FOR,            //29
    DO,             //30
    WHILE,          //31
    BREAK,          //32
    CONTINUE,       //33
    COMA
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
    IF_END,         //13
    O_BR,           //14
    C_BR,           //15
    WHILE_LABEL,    //16
    WHILE_EXPR,     //17
    WHILE_END,      //18
    NEXT,           //19
    SKIP,           //20
    FUNC_CALL_VALID,//21
    FUNC_DECL_VALID //22
};

class AST{
private:
    int type;                   // node type
    std::string func_name;
    std::string func_type;
    std::string var_name;
    std::string op;
    std::vector <std::string> func_param_types;
    int inum;
    int inum_r;

public:
    AST(){
        type = INT_MAX;
        func_param_types = {};
        func_name = "";
        func_type = "";
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
    void set_func_type(std::string value){
        func_type = value;
    }
    void set_func_param_types(std::string value){
        func_param_types.emplace_back(value);
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
    std::string check_func_type(){
        return func_type;
    }
    size_t check_func_num_param(){
        return func_param_types.size();
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

bool find_call_num_param(std::vector<AST> functions, std::string name, int length, int type);
bool find_func_name(std::vector<AST> functions, std::string key, int type);
bool find_func_num_param(std::vector<AST> functions, std::string name, int length, int type);
std::pair<int, int> find_var(const std::string& key, const dvar_t& decl_vars);
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

    void pop_node(){
        nodes.pop_back();
    }

    AST top_node(){
        return nodes.back();
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

        // Identifier and function call
        if (tokens[current].first == IDENTIFIER){
            AST valid_node;
            valid_node.set_type(FUNC_CALL_VALID);
            valid_node.set_func_name(tokens[current].second);
            inc_cur();
            if (tokens[current].first == O_PRN){
                inc_cur();
                if (tokens[current].first != C_PRN){
                    parse_expr(tokens);
                    valid_node.set_func_param_types("arg");
                    while (tokens[current].first == COMA){
                        inc_cur();
                        parse_expr(tokens);
                        valid_node.set_func_param_types("arg");
                    }
                    if (tokens[current].first != C_PRN){
                        std::cout << "No close parentheses in function call\n";
                        exit(0);
                    }
                    push_node(valid_node);
                    return;
                }
                push_node(valid_node);
                return;
            } else {
                dec_cur();
                AST node;
                node.set_type(VARREF);
                node.set_var_name(tokens[current].second);
                push_node(node);
                return;
            }
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

    bool parse_expr_opt(const tokens_t& tokens){
        if (tokens[current].first == IDENTIFIER || tokens[current].first == I_NUM ||
            tokens[current].first == ANEG || tokens[current].first == LNEG || tokens[current].first == COMPLEMENT ||
            tokens[current].first == O_PRN){
            parse_expr(tokens);
        } else return true;
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

        if (tokens[current].first == O_BRACE){
            AST o_node;
            o_node.set_type(O_BR);
            push_node(o_node);

            inc_cur();
            parse_block_item(tokens);
            inc_cur();
            while (tokens[current].first != C_BRACE){
                parse_block_item(tokens);
                inc_cur();
            }

            AST c_node;
            c_node.set_type(C_BR);
            push_node(c_node);
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
        if (tokens[current].first == SEMI){
            return;
        }

        //For
        if (tokens[current].first == FOR){
            inc_cur();
            if (tokens[current].first != O_PRN){
                std::cout << "No open parentheses after FOR" << std::endl;
                exit(0);
            }
            inc_cur();

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
                if (tokens[current].first != ASSIGN){
                    std::cout << "Error in variable declaration in an initial clause" << std::endl;
                    exit(0);
                }
                inc_cur();
                parse_expr(tokens);
                push_node(node_assign);
                //inc_cur();
//            push_node(node);

                if (tokens[current].first != SEMI){
                    std::cout << "No semicolon after initial clause in FOR 1" << std::endl;
                    exit(0);
                }
                //return;
            } else {
                parse_expr_opt(tokens);
                if (tokens[current].first != SEMI){
                    std::cout << "No semicolon after initial clause in FOR 2" << std::endl;
                    exit(0);
                }
            }

            AST node_while_label;
            node_while_label.set_type(WHILE_LABEL);
            push_node(node_while_label);

            inc_cur();
            bool semi = parse_expr_opt(tokens);
            if (tokens[current].first != SEMI){
                std::cout << "No semicolon after controlling expression in FOR" << std::endl;
                exit(0);
            }

            if (!semi){
                AST node_while_expr;
                node_while_expr.set_type(WHILE_EXPR);
                push_node(node_while_expr);
            }


            size_t length = nodes.size();

            inc_cur();
            parse_expr_opt(tokens);
            if (tokens[current].first != C_PRN){
                std::cout << "No close parentheses in FOR statement" << std::endl;
                exit(0);
            }

            // swap nodes
            std::vector<AST> temp;
            size_t index = 0;
            length = nodes.size() - length;
            while (index < length){
                temp.emplace_back(top_node());
                pop_node();
                index++;
            }

            inc_cur();
            parse_statement(tokens);

            index = 0;
            size_t temp_size = temp.size();
            while (index < temp_size){
                push_node(temp.back());
                temp.pop_back();
                index++;
            }

            AST node_while_end;
            node_while_end.set_type(WHILE_END);
            push_node(node_while_end);

            return;
        }

        // While
        if (tokens[current].first == WHILE){
            inc_cur();
            if (tokens[current].first != O_PRN){
                std::cout << "No open parentheses after WHILE" << std::endl;
                exit(0);
            }
            inc_cur();

            AST node_while_label;
            node_while_label.set_type(WHILE_LABEL);
            push_node(node_while_label);

            parse_expr(tokens);
            if (tokens[current].first != C_PRN){
                std::cout << "No close parentheses in WHILE statement" << std::endl;
                exit(0);
            }
            AST node_while_expr;
            node_while_expr.set_type(WHILE_EXPR);
            push_node(node_while_expr);

            inc_cur();
            parse_statement(tokens);

            AST node_while_end;
            node_while_end.set_type(WHILE_END);
            push_node(node_while_end);

            return;
        }

        // Do
        if (tokens[current].first == DO){
            AST node_while_label;
            node_while_label.set_type(WHILE_LABEL);
            push_node(node_while_label);

            inc_cur();
            parse_statement(tokens);
            inc_cur();
            if (tokens[current].first != WHILE){
                std::cout << "No WHILE in DO statement" << std::endl;
                exit(0);
            }
            inc_cur();
            parse_expr(tokens);
            if (tokens[current].first != SEMI){
                std::cout << "No semicolon at the end of the statement 4" << std::endl;
                exit(0);
            }
            AST node_while_expr;
            node_while_expr.set_type(WHILE_EXPR);
            push_node(node_while_expr);

            AST node_while_end;
            node_while_end.set_type(WHILE_END);
            push_node(node_while_end);
            return;
        }

        // Break
        if (tokens[current].first == BREAK){
            inc_cur();
            if (tokens[current].first != SEMI){
                std::cout << "No semicolon at the end of the statement 5" << std::endl;
                exit(0);
            }

            AST node;
            node.set_type(SKIP);
            push_node(node);
            return;
        }

        // Continue
        if (tokens[current].first == CONTINUE){
            inc_cur();
            if (tokens[current].first != SEMI){
                std::cout << "No semicolon at the end of the statement 6" << std::endl;
                exit(0);
            }
            AST node;
            node.set_type(NEXT);
            push_node(node);

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
        AST valid_node;
        valid_node.set_type(FUNC_DECL_VALID);
        valid_node.set_func_type(tokens[current].second);
        inc_cur();

        if (tokens[current].first != IDENTIFIER){
            std::cout << "Illegal name of function" << std::endl;
            exit(0);
        }
        AST node;
        node.set_type(FUNC_DECL);
        node.set_func_name(tokens[current].second);
        push_node(node);
        valid_node.set_func_name(tokens[current].second);
        inc_cur();

        if (tokens[current].first != O_PRN){
            std::cout << "No open parentheses at function declaration" << std::endl;
            exit(0);
        }
        inc_cur();

        if (tokens[current].first == KEYWORD){
            valid_node.set_func_param_types(tokens[current].second);
            inc_cur();

            if (tokens[current].first != IDENTIFIER){
                std::cout << "Wrong function parameter name1" << std::endl;
                exit(0);
            }
            inc_cur();

            while (tokens[current].first == COMA){
                inc_cur();
                if (tokens[current].first != KEYWORD){
                    std::cout << "Wrong type of function parameter" << std::endl;
                    exit(0);
                }
                valid_node.set_func_param_types(tokens[current].second);
                inc_cur();

                if (tokens[current].first != IDENTIFIER){
                    std::cout << "Wrong function parameter name2" << std::endl;
                    exit(0);
                }
                inc_cur();
            }
        }

        push_node(valid_node);

        if (tokens[current].first != C_PRN){
            std::cout << "No close parentheses at function declaration" << std::endl;
            exit(0);
        }
        inc_cur();

        if (tokens[current].first == O_BRACE){
            AST o_node;
            o_node.set_type(O_BR);
            push_node(o_node);

            inc_cur();
            parse_block_item(tokens);
            inc_cur();
            while (tokens[current].first != C_BRACE){
                parse_block_item(tokens);
                inc_cur();
            }
            AST c_node;
            c_node.set_type(C_BR);
            push_node(c_node);
        } else {
            if (tokens[current].first != SEMI){
                std::cout << "No semicolon after function declaration" << std::endl;
                exit(0);
            }
        }
    }
};

int main (int argc, char ** argv){
    std::string input = read_file(R"(D:\Winderton\Compiler_cvv\stage9_tests\invalid\too_many_args.c)");
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

    int temp;
    int temp_label;
    int label = 0;
    int stack_index = -4;
    dvar_t decl_vars;
    std::stack<int> labels;
    std::stack<int> size_dv;
    std::vector<AST> functions;

    size_t current = 0;
    while (current < ast.size()){
        switch (ast[current].check_type()){
            case FUNC_DECL_VALID:
                if (find_func_name(functions, ast[current].check_func_name(), ast[current].check_type())){
                    std::cout << "2 definitions of the same function name" << std::endl;
                    exit(0);
                }

                if (find_func_num_param(functions, ast[current].check_func_name(), ast[current].check_func_num_param(), ast[current].check_type())){
                    std::cout << "2 declaration have different number of parameters" << std::endl;
                    exit(0);
                }

                functions.emplace_back(ast[current]);
                current++;
                break;

            case FUNC_CALL_VALID:
                if (find_call_num_param(functions, ast[current].check_func_name(), ast[current].check_func_num_param(), ast[current].check_type())){
                    std::cout << "Function is called with the wrong number of arguments" << std::endl;
                    exit(0);
                }
                current++;
                break;

            case O_BR:
                size_dv.push(decl_vars.size());
                current++;
                break;

            case C_BR:
                while (decl_vars.size() > size_dv.top()){
                    decl_vars.erase(decl_vars.begin() + decl_vars.size() - 1);
                }
                size_dv.pop();
                current++;
                break;

            case VARDECL:
                if (find_var(ast[current].check_var_name(), decl_vars).first != 0 &&
                    find_var(ast[current].check_var_name(), decl_vars).second >= size_dv.top()){
                    std::cout << "Variable is already defined" << std::endl;
                    exit(0);
                }
                decl_vars.emplace_back(ast[current].check_var_name(), stack_index);
                fprintf(pfile, "\tpush 0\n");
                stack_index -= 4;       // ???
                current++;
                break;

            case VARASSIGN:
                if (find_var(ast[current].check_var_name(), decl_vars).first == 0){
                    std::cout << "Variable is not defined1" << std::endl;
                    exit(0);
                }
                fprintf(pfile, "\tmov [ebp + %d], eax\n", find_var(ast[current].check_var_name(), decl_vars).first);
                current++;
                break;

            case VARREF:
                if (find_var(ast[current].check_var_name(), decl_vars).first == 0){
                    std::cout << "Variable is not defined2" << std::endl;
                    exit(0);
                }
                fprintf(pfile, "\tmov eax, [ebp + %d]\n", find_var(ast[current].check_var_name(), decl_vars).first);
                fprintf(pfile, "\tpush eax\n");
                stack_index -= 4;
                current++;
                break;

            case FUNC_DECL:
                fprintf(pfile, "%s:\n", ast[current].check_func_name().c_str());
                fprintf(pfile, "\tpush ebp\n");     //save old value of EBP
                fprintf(pfile, "\tmov ebp, esp\n"); //current top of stack is bottom of new stack frame
                current++;
                break;

            case WHILE_LABEL:
                fprintf(pfile, "label%d:\n", label);
                labels.push(label);
                current++;
                label++;
                break;

            case WHILE_EXPR:
                fprintf(pfile, "\tpop eax\n");
                stack_index += 4;
                fprintf(pfile, "\tcmp eax, 0\n");
                fprintf(pfile, "\tje label%d\n", label);
                labels.push(label);
                current++;
                label++;
                break;

            case WHILE_END:
                temp = labels.top();
                labels.pop();
                fprintf(pfile, "\tjmp label%d\n", labels.top());
                labels.pop();
                fprintf(pfile, "\tlabel%d:\n", temp);
                current++;
                break;

            case NEXT:
                fprintf(pfile, "\tjmp label%d\n", labels.top());
                current++;
                break;

            case SKIP:
                temp_label = labels.top();
                labels.pop();
                fprintf(pfile, "\tjmp label%d\n", labels.top());
                labels.push(temp_label);
                current++;
                break;

            case COND_QUEST:
                fprintf(pfile, "\tpop eax\n");
                stack_index += 4;
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
                stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tneg eax\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }
                if (ast[current].check_op() == "!"){
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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
                    stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
                    fprintf(pfile, "\tadd eax, ecx\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "*"){
                    fprintf(pfile, "\tpop ecx\n");
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
                    fprintf(pfile, "\timul eax, ecx\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "-"){
                    fprintf(pfile, "\tpop ecx\n");
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
                    fprintf(pfile, "\tsub eax, ecx\n");
                    fprintf(pfile, "\tpush eax\n");
                    stack_index -= 4;
                    current++;
                    break;
                }

                if (ast[current].check_op() == "/"){
                    fprintf(pfile, "\tpop ecx\n");
                    stack_index += 4;
                    fprintf(pfile, "\tpop eax\n");
                    stack_index += 4;
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

        // Coma
        if (symbol == ','){
            std::string value;
            value += symbol;
            tokens.emplace_back(COMA, value);
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
            if (value == "for") {
                tokens.emplace_back(FOR, value);
                continue;
            }
            if (value == "do") {
                tokens.emplace_back(DO, value);
                continue;
            }
            if (value == "while") {
                tokens.emplace_back(WHILE, value);
                continue;
            }
            if (value == "break") {
                tokens.emplace_back(BREAK, value);
                continue;
            }
            if (value == "continue") {
                tokens.emplace_back(CONTINUE, value);
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

std::pair<int, int> find_var(const std::string& key, const dvar_t& decl_vars){
    //if (!decl_vars.empty()){
        size_t i = decl_vars.size()-1;
        while (i >= 0 && i < decl_vars.size()){
            if (key == decl_vars[i].first){
                std::pair<int, int> ret = {decl_vars[i].second, i};
                return ret;
            }
            i --;
        }
    //}
    return {0, 0};
}

bool find_func_name(std::vector<AST> functions, std::string key, int type){
    bool flag = false;
    for (int i = 0; i < functions.size(); i++){
        if (type == FUNC_DECL_VALID && key == functions[i].check_func_name()){
            flag = true;
            break;
        }
    }
    if (flag){
        return true;
    } else return false;
}

bool find_func_num_param(std::vector<AST> functions, std::string name, int length, int type){
    bool flag = false;
    for (int i = 0; i < functions.size(); i++){
        if (functions[i].check_func_name() == name && length != functions[i].check_func_num_param() && functions[i].check_type() == type){
            flag = true;
            break;
        }
    }
    if (flag){
        return true;
    } else return false;
}

bool find_call_num_param(std::vector<AST> functions, std::string name, int length, int type){
    bool flag = false;
    for (int i = 0; i < functions.size(); i++){
        if (functions[i].check_func_name() == name && length != functions[i].check_func_num_param() && functions[i].check_type() != type){
            flag = true;
            break;
        }
    }
    if (flag){
        return true;
    } else return false;
}