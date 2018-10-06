/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include <list>
#include <set>
#include "parser.h"
#include "Util.h"

using namespace std;

int counter = 0;

list<TokenObject> tList;
list<TokenObject>::iterator itr = tList.begin();

//overloading less than set operator to perform less than operation on REG_Node ID
bool operator<(const REG_node& node1, const REG_node& node2) {
	if (node1.nodeID < node2.nodeID)
    {
        return true;
    }
    else
    {
        return false;
    }
}

set <REG_node, less <REG_node> > match_one_char(set <REG_node, less <REG_node> > S, char c)
{

	set <REG_node, less <REG_node> > s1 = {};

	for (REG_node n : S) {
		if (n.first_label == c) {
			s1.insert(*n.first_neighbor);
		}
		if (n.second_label == c) {
			s1.insert(*n.second_neighbor);
		}
	}

	if (s1.size() == 0) {
		return s1;
	}

	bool changed = true;
	while (changed) {
		changed = false;
		for (REG_node n : s1) {
			if (n.first_label == '_' &&
				(s1.find(*n.first_neighbor) == s1.end())) {
				s1.insert(*n.first_neighbor);
				changed = true;
			}
			if (n.second_label == '_' &&
				(s1.find(*n.second_neighbor) == s1.end())) {
				s1.insert(*n.second_neighbor);
				changed = true;
			}
		}
	}

	return s1;
}

string match(REG reg, string str, int index) {

    string matched = "";
    bool isEmpty = false;
	int p = index;

	set <REG_node, less <REG_node> > setOfREG_Node;
	setOfREG_Node.insert(*reg.start);
	setOfREG_Node = match_one_char(setOfREG_Node, '_');
	setOfREG_Node.insert(*reg.start);

	while (index < str.size() && !isEmpty)
    {
		setOfREG_Node = match_one_char(setOfREG_Node, str[index]);
		if (setOfREG_Node.size() == 0) {
			isEmpty = true;
		}
		else if (setOfREG_Node.find(*reg.accept) != setOfREG_Node.end()) {
			matched = str.substr(p, index-p+1);
		}
		else {
			matched = "";
		}
		index++;
	}

	return matched;
}

foundToken my_GetToken(list <TokenObject> tlist, string s, int index)
{

	string longest = "";
	string current = "";
	string tName = "";

	for (TokenObject obj : tlist)
    {
		current = match(obj.regGraph, s, index);
		if (current > longest)
		{
			longest = current;
			tName = obj.tokenName.lexeme;
		}
	}

	return foundToken(tName, longest);
}

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

// this function simply checks the next token without
// consuming the input
// Written by Mohsen Zohrevandi
Token Parser::peek()
{
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}

// Parsing

void Parser::parse_input()
{
    //input -> tokens_section INPUT_TEXT

    parse_tokens_section();
    Token inputString = expect(INPUT_TEXT);

    int index = 0;
	while (index < inputString.lexeme.length()) {

		while (inputString.lexeme[index] == ' ') {
			index++;
		}

		foundToken fToken = my_GetToken(tList, inputString.lexeme, index);
		index = index + fToken.matchedString.length();

		if (fToken.matchedString != "") {
			cout << fToken.foundTokenName << " , \"" << fToken.matchedString << "\"" << "\n";
		}
		else if(fToken.matchedString.length() == 0 && inputString.lexeme[index+1] != '\004' && index < inputString.lexeme.length())
        {
            cout << "ERROR" << "\n";
            break;
        }
	}
}


void Parser::parse_tokens_section()
{
    // tokens_section -> token_list HASH


    parse_token_list();
    expect(HASH);
}

void Parser::parse_token_list()
{
    // token_list -> token
    // token_list -> token COMMA token_list

    parse_token();
    Token t = peek();
    if (t.token_type == COMMA)
    {
        // token_list -> token COMMA token_list
        expect(COMMA);
        parse_token_list();
    }
    else if (t.token_type == HASH)
    {
        // token_list -> token
    }
    else
    {
        syntax_error();
    }

}

void Parser::parse_token()
{
    // token -> ID expr

    //Create an object that will hold name and REG of current token
    struct TokenObject *tokenobj = new struct TokenObject;
    tokenobj->tokenName = expect(ID);
    tokenobj->regGraph = *parse_expr();

    //Add the token name and corresponding REG to list
    tList.insert(itr, *tokenobj);
    //cout << "Item added to list";
}

struct REG * Parser::parse_expr()
{
    // expr -> CHAR
    // expr -> LPAREN expr RPAREN DOT LPAREN expr RPAREN
    // expr -> LPAREN expr RPAREN OR LPAREN expr RPAREN
    // expr -> LPAREN expr RPAREN STAR
    // expr -> UNDERSCORE
    struct REG * completeREG = new struct REG;

    struct REG_node * finalStart = new REG_node;
    counter++;
    finalStart->nodeID = counter;

    struct REG_node * finalAccept = new REG_node;
    counter++;
    finalAccept->nodeID = counter;

    Token t = lexer.GetToken();
    if (t.token_type == CHAR)
    {
        // expr -> CHAR
        finalStart->first_neighbor = finalAccept;
        finalStart->first_label = t.lexeme[0];

        completeREG->start = finalStart;
        completeREG->accept = finalAccept;
    }
    else if (t.token_type == UNDERSCORE)
    {
        // expr -> UNDERSCORE
        finalStart->first_neighbor = finalAccept;
        finalStart->first_label = '_';

        completeREG->start = finalStart;
        completeREG->accept = finalAccept;
    }
    else if (t.token_type == LPAREN)
    {
        // expr -> LPAREN expr RPAREN DOT LPAREN expr RPAREN
        // expr -> LPAREN expr RPAREN OR LPAREN expr RPAREN
        // expr -> LPAREN expr RPAREN STAR
        struct REG * r1 = parse_expr();
        expect(RPAREN);
        Token t2 = lexer.GetToken();
        if (t2.token_type == DOT)
        {
            expect(LPAREN);
            struct REG * r2 = parse_expr();
            expect(RPAREN);

            //connecting both r1 and r2 where accept of r1 will connect to start of r2
            r1->accept->first_neighbor = r2->start;
            r1->accept->first_label = '_';

            //combining both r1 and r2 to make a new complete REG
            //where start will be start of r1 and accept will be accept of r2
            completeREG->start = r1->start;
            completeREG->accept = r2->accept;
        }
        else if (t2.token_type == OR)
        {
            expect(LPAREN);
            struct REG * r2 = parse_expr();
            expect(RPAREN);

            completeREG->start = finalStart;

            completeREG->start->first_neighbor = r1->start;
            completeREG->start->first_label = '_';

            completeREG->start->second_neighbor = r2->start;
            completeREG->start->second_label = '_';

            completeREG->accept = finalAccept;

            r1->accept->first_neighbor = completeREG->accept;
            r1->accept->first_label = '_';

            r2->accept->first_neighbor = completeREG->accept;
            r2->accept->first_label = '_';
        }
        else if (t2.token_type == STAR)
        {
            finalStart->first_neighbor = r1->start;
            finalStart->first_label = '_';

            r1->accept->first_neighbor = finalAccept;
            r1->accept->first_label = '_';

            r1->accept->second_neighbor = r1->start;
            r1->accept->second_label = '_';

            finalStart->second_neighbor = finalAccept;
            finalStart->second_label = '_';

            completeREG->start = finalStart;
            completeREG->accept = finalAccept;
        }
    }
    else
    {
        syntax_error();
    }

    return completeREG;
}

void Parser::ParseProgram()
{
    parse_input();
    expect(END_OF_FILE);
}

int main()
{
    Parser parser;
    parser.ParseProgram();
}
