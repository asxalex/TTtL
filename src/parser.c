/*
 * parser.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

static int parse_index = 0;
static lex *curtok;

ast_t *parse_expression();
ast_t *parse_primary();

void print_expression(expressions*, int);
expressions *gather_function_params();

static int get_precedence(lex *current) {
    if (!current)
        return -2;
    TOKEN token = current->token;
    if (token == OR || token == AND) 
        return 2;
    if (token == ASSIGN)
        return 5;
    if (token == LT || token == GT || token == EQUAL || token == LTE || token == GTE || token == NEQ)
        return 10;
    if (token == ADD || token == MINUS)
        return 20;
    if (token == MUL || token == DIV)
        return 40;
    return -1;
}

static void get_next_token() {
    if (parse_index == lex_index) {
        curtok = NULL;
        return;
    }
    curtok = &lex_list[parse_index++];
}

static void unget_token() {
    if (parse_index > 0)
        curtok = &lex_list[--parse_index];
}

expressions *gather_expression() {
    lex *current = curtok;
    expressions *exps = (expressions*)malloc(sizeof(expressions));
    init_expressions(exps);
    expression *old;
    while (current && current->token != RBRACE) {
        ast_t *new = parse_expression();
        expression *e = (expression*)malloc(sizeof(expression));
        e->ast = new;
        INIT_HLIST_NODE(&e->node);
        if (hlist_empty((const expressions*)exps)) {
            hlist_add_head(&e->node, exps);
        } else {
            hlist_add_after(&old->node, &e->node);
        }
        old = e;
        current = curtok;
    }
    return exps;
}

ast_t *parse_while() {
    while_ast_t *res;
    get_next_token(); // skip while
    lex *current = curtok;
    if (current->token != LPARAN) {
        ERRORF(current->line, expected LEFT PARENTHSIS)
    }
    ast_t *cond = parse_primary();
    new_while_ast(res, cond, current->line);

    current = curtok;
    if(current->token != LBRACE) {
        ERRORF(current->line, expected LEFT BRACE)
    }
    get_next_token(); // skip {
    res->body = gather_expression();
    
    current = curtok;
    if(current->token != RBRACE) {
        ERRORF(current->line, expected RIGHT BRACE)
    }
    get_next_token(); // skip }
    return (ast_t*)res;
}

ast_t *parse_if() {
    if_ast_t *res;
    get_next_token(); // skip if
    lex *current = curtok;
    if (current->token != LPARAN) {
        ERRORF(current->line, expected LEFT PARENTHSIS)
    }

    ast_t *cond = parse_primary();
    new_if_ast(res, cond, current->line);

    current = curtok;
    if (current->token != LBRACE) {
        ERRORF(current->line, expected LEFT BRACE)
    }
    get_next_token(); // skip {
    res->then = gather_expression();
    get_next_token(); // skip }

    current = curtok;
    if (current && current->token == ELSE) {
        get_next_token(); // skip else
        current = curtok;
        if (!current || current->token != LBRACE) {
            ERRORF(current->line, expected LEFT BRACE)
        }
        get_next_token(); // skip {
        res->els = gather_expression();
        get_next_token(); // skip }
    } else {
        res->els = NULL;
    }
    return (ast_t*)res;
}

ast_t *parse_identifier() {
    lex *current = curtok; 
    get_next_token(); // skip identifier
    lex *next = curtok;
    if (next && next->token == LPARAN) {
        // function call;
        call_ast_t *call;
        new_call_ast(call, current->line);
        call->name = current->value;
        current->value = NULL;
        get_next_token(); // consume LPARAN
        call->args = gather_function_params();
        //get_next_token(); // consume RPARAN
        return (ast_t*)call;
    } else {
        // symbol reference;
        variable_ast_t *variable;
        new_variable_ast(variable, current->value, current->line);
        current->value = NULL;
        return (ast_t*)variable;
    }
}

ast_t *parse_boolean() {
    boolean_ast_t *res;
    if (curtok->token == TRUE) {
        new_boolean_ast(res, 1, curtok->line);
    } else {
        new_boolean_ast(res, 0, curtok->line);
    }
    return (ast_t*)res;
}

ast_t *parse_number() {
    lex *current = curtok;
    long sum = 0;
    int length = strlen(current->value);
    for (int i = 0; i < length; i++) {
        sum = 10 * sum + current->value[i] - '0';
    }
    number_ast_t *number;
    new_number_ast(number, sum, current->line);
    return (ast_t*)number;
}

// parse (exp)
ast_t *parse_paran() {
    get_next_token(); // consume (
    ast_t *inside = parse_expression();
    if (curtok->token != RPARAN) {
        ERRORF(curtok->line, expected right parenthesis here);
    }
    //get_next_token(); // consume )
    return inside;
}

expressions* gather_function_params() {
    lex *current = curtok;
    expressions *exps = (expressions*)malloc(sizeof(expressions));
    init_expressions(exps);
    expression *old;
    while (current && current->token != RPARAN) {
        ast_t *new = parse_expression();
        expression *e = (expression*)malloc(sizeof(expression));
        e->ast = new;
        INIT_HLIST_NODE(&e->node);
        if (hlist_empty((const expressions*)exps)) {
            hlist_add_head(&e->node, exps);
        } else {
            hlist_add_after(&old->node, &e->node);
        }
        old = e;
        current = curtok;
        if (!current) {
            ERRORF(-1, unexpected NIL);
        }
        if (current->token != RPARAN && current->token != COMMA) {
            ERRORF(current->line, COMMA or RIGHT PARAN is expected);
        }
        if (current->token == COMMA) {
            get_next_token();
            current = curtok;
        }
    }
    get_next_token(); // skip )
    return exps;
}

ast_t *parse_character() {
    character_ast_t *s;
    new_character_ast(s, curtok->value[0], curtok->line);
    free(curtok->value);
    get_next_token();
    if(curtok->token != SINGLEQUOTE) {
        ERRORF(curtok->line, require single quote here);
    }
    get_next_token(); // skip DOUBLEQUOTE
    return (ast_t*)s;
}

ast_t *parse_string() {
    string_ast_t *s;
    new_string_ast(s, curtok->value, curtok->line);
    curtok->value = NULL;
    get_next_token();
    if(curtok->token != DOUBLEQUOTE) {
        ERRORF(curtok->line, require double quote here);
    }
    get_next_token(); // skip DOUBLEQUOTE
    return (ast_t*)s;
}

ast_t *parse_function() {
    lex *current;
    get_next_token(); // skip define 
    function_ast_t *function;
    new_function_ast(function, curtok->value, curtok->line);
    curtok->value = NULL;
    get_next_token(); // skip name
    current = curtok;
    if (current->token != LPARAN) {
        ERRORF(curtok->line, expected left parenthesis here);
    }
    get_next_token(); // skip (

    function->params = gather_function_params();

    current = curtok;
    if(current->token != LBRACE) {
        ERRORF(current->line, expected left brace);
    }
    get_next_token(); // skip {
    function->body = gather_expression();
    function->env = NULL;
    get_next_token(); // skip }
    return (ast_t*)function;
}

ast_t *parse_primary() {
    if (!curtok) {
        return NULL;
    }
    ast_t *res;
    switch(curtok->token) {
        case DEFINE:
            res = parse_function();
            return res;
        case IDENTIFIER:
            res = parse_identifier();
            //get_next_token(); // skip identifier;
            return res;
        case NUMBER:
            res = parse_number();
            get_next_token(); // skip number;
            return res;
        case LPARAN:
            res = parse_paran();
            get_next_token(); // skip right paran )
            return res;
        case IF:
            res = parse_if();
            //get_next_token(); the } token is handled already
            return res;
        case WHILE:
            res = parse_while();
            //get_next_token(); // skip right bracket }
            return res;
        case TRUE:
        case FALSE:
            res = parse_boolean();
            get_next_token();
            return res;
        case SEMICOLON:
            get_next_token(); // skip SEMICOLON
            return parse_primary();
        case DOUBLEQUOTE:
            get_next_token(); // skip double quote
            res = parse_string();
            return res;
        case SINGLEQUOTE:
            get_next_token(); // skip single quote
            res = parse_character();
            return res;
        default:
            printf("token is %d\n", curtok->token);
            ERRORF(curtok->line, unexpected token);
    }
}

ast_t *parse_binary_ops(int precedence, ast_t *lhs) {
    lex *cur;
    while(1) {
        int cur_prec = get_precedence(curtok);
        if (cur_prec < precedence) {
            return lhs;
        }
        string_ast_t *ops;
        new_string_ast(ops, curtok->value, curtok->line);
        cur = curtok;
        get_next_token(); // eat binop

        ast_t *rhs = parse_primary();
        if (!rhs) {
            return lhs;
        }

        //get_next_token();
        int next_prec = get_precedence(curtok);
        if (cur_prec < next_prec) {
            rhs = parse_binary_ops(cur_prec+1, rhs);
            if (!rhs) {
                // should parse error 2;
                ERRORF(-1, parse error 2);
                return lhs;
            }
        }

        binary_ast_t *n;
        new_binary_ast(n, (ast_t*)ops, lhs, rhs, cur->line);
        lhs = (ast_t *)n;
    }
}

ast_t *parse_expression() {
    ast_t *lhs = parse_primary();
    if (!lhs) {
        return NULL;
    }
    ast_t *res = parse_binary_ops(0, lhs);
    return res;
}

void pretty_format(int depth) {
    return;
    for(int i = 0; i < depth; i++) {
        printf(" ");
    }
}

void print_expression(expressions* exps, int depth) {
    if (!exps) {
        printf(" NIL ");
    }
    expression *nodes;
    ast_t *ast;
    hlist_node_t *iter;
    hlist_for_each(exps, iter) {
        nodes = hlist_entry(iter, expression, node);
        ast = nodes->ast;
        print_ast(ast, depth);
    }
}

void print_ast(ast_t *t, int depth) {
    if (!t) {
        printf("ast is NULL;\n");
        return;
    }

    //PRINTF_ENUM(t->type);
    switch (t->type) {
        case BOOLEANAST:
            pretty_format(depth);
            printf("%s ", ((boolean_ast_t*)t)->value ? "TRUE" : "FALSE");
            break;
        case NUMBERAST:
            pretty_format(depth);
            printf("%ld ", ((number_ast_t*)t)->value);
            break;
        case STRINGAST:
            pretty_format(depth);
            printf("%s ", ((string_ast_t*)t)->value);
            break;
        case BINARYAST:
            //binary_ast_t *temp = (binary_ast_t*)t;
            pretty_format(depth);
            printf("(");
            print_ast(((binary_ast_t*)t)->op, depth+1);
            print_ast(((binary_ast_t*)t)->lhs, depth+1);
            print_ast(((binary_ast_t*)t)->rhs, depth+1);
            pretty_format(depth);
            printf(") ");
            break;
        case VARIABLEAST:
            pretty_format(depth);
            printf("%s ", ((variable_ast_t*)t)->value);
            break;
        case CHARACTERAST:
            pretty_format(depth);
            printf("%c ", ((character_ast_t*)t)->value);
            break;
        case IFAST:
            pretty_format(depth);
            printf("if ");
            print_ast(((if_ast_t*)t)->condition, depth+1);
            printf("then ");
            print_expression(((if_ast_t*)t)->then, depth+1);
            printf("else ");
            print_expression(((if_ast_t*)t)->els, depth+1);
            break;
        case WHILEAST:
            pretty_format(depth);
            printf("while ");
            print_ast(((while_ast_t*)t)->condition, depth+1);
            printf("body ");
            print_expression(((while_ast_t*)t)->body, depth+1);
            break;
        case FUNCTIONAST:
            pretty_format(depth);
            printf("define ");
            printf("%s ", ((function_ast_t*)t)->name);
            printf("params ");
            print_expression(((function_ast_t*)t)->params, depth+1);
            printf("body ");
            print_expression(((function_ast_t*)t)->body, depth+1);
            break;
        case CALLAST:
            pretty_format(depth);
            printf("call ");
            printf("%s ", ((call_ast_t*)t)->name);
            printf("args ");
            print_expression(((call_ast_t*)t)->args, depth+1);
            break;
        default:
            printf("ast type is %d\n", t->type);
            ERRORF(t->line, no such ast type);
    }
}

expressions *parser() {
    get_next_token();
    expressions *ast = gather_expression();
    return ast;

    /*
    expression *exp;
    hlist_node_t *iter;
    hlist_for_each(ast, iter) {
        exp = hlist_entry(iter, expression, node);
        print_ast(exp->ast, 0);
    }
    */
    //print_expression(ast, 0);
    //printf("\n");
    /*
    ast_t *ast;
    while(1) {
        ast = parse_expression();
        if (!ast) 
            break;

        lex *current = curtok;
        // handle the end of expression
        if (current && current->token == SEMICOLON) {
            get_next_token();
        }
        print_ast(ast, 0);
        printf("\n");
    }
    return NULL;
    */
}
