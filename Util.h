/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef __UTIL__H__
#define __UTIL__H__

#include <string>
#include "lexer.h"

using namespace std;

struct REG_node {
    int nodeID;
    struct REG_node *first_neighbor;
    char first_label;
    struct REG_node *second_neighbor;
    char second_label;
};

struct REG {
    struct REG_node *start;
    struct REG_node *accept;
};

struct TokenObject {
    Token tokenName;
    struct REG regGraph;
};

struct foundToken {
	string foundTokenName;
	string matchedString;

	foundToken(string name, string lexeme) : foundTokenName(name), matchedString(lexeme) {}
};

#endif
