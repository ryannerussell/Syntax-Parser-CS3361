
/*
 * CS3361 - Project 2
 * Cooke Syntax Parser
 *
 * Recursive-descent parser for the Cooke language.
 */

#include <stdio.h>
#include <stdlib.h>
#include "front.h"
#include "parser.h"

/* Token and lexeme are provided by front.c */
extern int nextToken;
extern char *lexeme;

/* Internal function declarations */
static void error(void);
static void match(int expectedToken);

static void statement(void);
static void condition(void);
static void B(void);
static void R(void);

static void W(void);
static void F(void);

static void parseAssignment(void);
static void parseUpdate(void);
static void parseBlock(void);
static void parseElif(void);
static void parseFor(void);
static void parseWhile(void);
static void parseIf(void);

/*
 * Reports a syntax error and terminates parsing.
 * Line-number reporting will be connected to the
 * lexer's line counter.
 */
static void error(void) {
    printf("Error encountered on line %d: "
           "The next lexeme was %s and the next token was %s\n",
           lineNumber,
           lexeme != NULL ? lexeme : "EOF",
           nextToken == EOF ? "EOF" : tokenName(nextToken));

    exit(1);
}
/*
 * match
 *
 * Checks that the current token matches the
 * expected token, then advances the lexer.
 */
static void match(int expectedToken) {
    if (nextToken == expectedToken) {
        lex();
    }
    else {
        error();
    }
}

/*
 * P ::= S
 *
 * Starting rule for the Cooke grammar.
 */
void P(void) {
    S();
}

/*
 * S ::= statement { ; statement }
 *
 * Handles one or more statements separated
 * by semicolons.
 */
void S(void) {
    statement();

    while (nextToken == SEMICOLON) {
        match(SEMICOLON);
        statement();
    }
}

/*
 * Parses a Cooke statement.
 * Supported forms:
 * V := E
 * V ++
 * V --
 * read(V)
 * print(E)
 * for (...)
 * while (...)
 * if (...)
 */
static void statement(void) {
    if (nextToken == IDENT) {
        match(IDENT);

        if (nextToken == ASSIGNMENT_OP) {
            match(ASSIGNMENT_OP);
            expr();
        }
        else if (nextToken == INC_OP) {
            match(INC_OP);
        }
        else if (nextToken == DEC_OP) {
            match(DEC_OP);
        }
        else {
            error();
        }
    }
    else if (nextToken == KEY_READ) {
        match(KEY_READ);
        match(LEFT_PAREN);
        match(IDENT);
        match(RIGHT_PAREN);
    }
    else if (nextToken == KEY_PRINT) {
        match(KEY_PRINT);
        match(LEFT_PAREN);
        expr();
        match(RIGHT_PAREN);
    }
    else if (nextToken == KEY_IF) {
        parseIf();
    }
    else if (nextToken == KEY_WHILE) {
        parseWhile();
    }
    else if (nextToken == KEY_FOR) {
        parseFor();
    }
    else {
        error();
    }
}

/*
 * Parses V := E.
 */
static void parseAssignment(void) {
    match(IDENT);
    match(ASSIGNMENT_OP);
    expr();
}

/*
 * Parses V ++ or V --.
 */
static void parseUpdate(void) {
    match(IDENT);

    if (nextToken == INC_OP) {
        match(INC_OP);
    }
    else if (nextToken == DEC_OP) {
        match(DEC_OP);
    }
    else {
        error();
    }
}

/*
 * Parses : { S }
 *
 * Used by if, elif, else, while, and for.
 */
static void parseBlock(void) {
    match(COLON);
    match(LEFT_BRACE);
    S();
    match(RIGHT_BRACE);
}

/*
 * Parses zero or more elif branches.
 *
 * M ::= elif ( C ) : { S } M | epsilon
 */
static void parseElif(void) {
    while (nextToken == KEY_ELIF) {
        match(KEY_ELIF);
        match(LEFT_PAREN);
        condition();
        match(RIGHT_PAREN);
        parseBlock();
    }
}

/*
 * Parses an if statement:
 *
 * if ( C ) : { S } M
 * if ( C ) : { S } M else : { S }
 */
static void parseIf(void) {
    match(KEY_IF);
    match(LEFT_PAREN);
    condition();
    match(RIGHT_PAREN);

    parseBlock();

    parseElif();

    if (nextToken == KEY_ELSE) {
        match(KEY_ELSE);
        parseBlock();
    }
}

/*
 * Parses:
 *
 * while ( C ) : { S }
 */
static void parseWhile(void) {
    match(KEY_WHILE);
    match(LEFT_PAREN);
    condition();
    match(RIGHT_PAREN);

    parseBlock();
}

/*
 * Parses:
 *
 * for ( V := E ; C ; V U ) : { S }
 */
static void parseFor(void) {
    match(KEY_FOR);
    match(LEFT_PAREN);

    parseAssignment();

    match(SEMICOLON);

    condition();

    match(SEMICOLON);

    parseUpdate();

    match(RIGHT_PAREN);

    parseBlock();
}

/*
 * C ::= B { || B }
 *
 * Logical OR has lower precedence than AND.
 */
static void condition(void) {
    B();

    while (nextToken == BOOLEAN_OR) {
        match(BOOLEAN_OR);
        B();
    }
}

/*
 * B ::= R { && R }
 *
 * Handles logical AND.
 */
static void B(void) {
    R();

    while (nextToken == BOOLEAN_AND) {
        match(BOOLEAN_AND);
        R();
    }
}

/*
 * R ::= ! R | E Q
 *
 * Q is an optional comparison:
 * < E
 * > E
 * == E
 * != E
 * <= E
 * >= E
 */
static void R(void) {
    if (nextToken == BOOLEAN_NOT) {
        match(BOOLEAN_NOT);
        R();
    }
    else {
        expr();

        switch (nextToken) {
            case LESSER_OP:
                match(LESSER_OP);
                expr();
                break;

            case GREATER_OP:
                match(GREATER_OP);
                expr();
                break;

            case EQUAL_OP:
                match(EQUAL_OP);
                expr();
                break;

            case NEQUAL_OP:
                match(NEQUAL_OP);
                expr();
                break;

            case LEQUAL_OP:
                match(LEQUAL_OP);
                expr();
                break;

            case GEQUAL_OP:
                match(GEQUAL_OP);
                expr();
                break;

            default:
                /* Q ::= epsilon */
                break;
        }
    }
}

/*
 * E ::= T { (+ | -) T }
 *
 * Handles addition and subtraction.
 */
void expr(void) {
    term();

    while (nextToken == ADD_OP ||
           nextToken == SUB_OP) {

        if (nextToken == ADD_OP) {
            match(ADD_OP);
        }
        else {
            match(SUB_OP);
        }

        term();
    }
}

/*
 * T ::= W { (* | / | %) W }
 *
 * Handles multiplication, division, and modulo.
 */
void term(void) {
    W();

    while (nextToken == MULT_OP ||
           nextToken == DIV_OP ||
           nextToken == MOD_OP) {

        if (nextToken == MULT_OP) {
            match(MULT_OP);
        }
        else if (nextToken == DIV_OP) {
            match(DIV_OP);
        }
        else {
            match(MOD_OP);
        }

        W();
    }
}

/*
 * W ::= F | F ** W
 *
 * Exponentiation is right-associative.
 */
static void W(void) {
    F();

    if (nextToken == POWER_OP) {
        match(POWER_OP);
        W();
    }
}

/*
 * F ::= ( C ) | I | L | V | true | false
 *
 * The lexer recognizes integers, floating-point
 * literals, identifiers, and boolean literals.
 */
static void F(void) {
    if (nextToken == LEFT_PAREN) {
        match(LEFT_PAREN);
        condition();
        match(RIGHT_PAREN);
    }
    else if (nextToken == IDENT) {
        match(IDENT);
    }
    else if (nextToken == INT_LITERAL) {
        match(INT_LITERAL);
    }
    else if (nextToken == FLOAT_LITERAL) {
        match(FLOAT_LITERAL);
    }
    else if (nextToken == BOOL_LITERAL) {
        match(BOOL_LITERAL);
    }
    else {
        error();
    }
}

/*
 * factor
 *
 * Retained for compatibility with parser.h
 * and the original starter parser.
 */
void factor(void) {
    F();
}
