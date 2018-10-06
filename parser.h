/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <list>
#include "lexer.h"

class Parser {
  private:
    LexicalAnalyzer lexer;

    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();


    void parse_input();
    void parse_char_list();
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    struct REG * parse_expr();

  public:
    void ParseProgram();
};

class set_of_nodes {
public:

};

#endif

