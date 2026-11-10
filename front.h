#ifndef FRONT_H
#define FRONT_H

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99


/* Identifiers and literals */
#define INT_LITERAL 10
#define IDENT 11
#define FLOAT_LITERAL 12
#define BOOL_LITERAL 13

/* Assignment and arithmetic operators */
#define ASSIGNMENT_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define MOD_OP 27
#define POWER_OP 28
#define INC_OP 29
#define DEC_OP 30

/* Parentheses, braces, and punctuation */
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define LEFT_BRACE 31
#define RIGHT_BRACE 32
#define SEMICOLON 33
#define COLON 34

/* Comparison operators */
#define LESSER_OP 35
#define GREATER_OP 36
#define EQUAL_OP 37
#define NEQUAL_OP 38
#define LEQUAL_OP 39
#define GEQUAL_OP 40

/* Boolean operators */
#define BOOLEAN_AND 41
#define BOOLEAN_OR 42
#define BOOLEAN_NOT 43

/* Keywords */
#define KEY_IF 44
#define KEY_ELIF 45
#define KEY_ELSE 46
#define KEY_FOR 47
#define KEY_WHILE 48
#define KEY_READ 49
#define KEY_PRINT 50


/*
 * Project 2: Share lexical analyzer information
 * with the recursive-descent parser.
 *
 * nextToken contains the current token classification.
 * lexeme contains the characters of the current token.
 */
extern int nextToken;
extern char *lexeme;

/* Retrieves the next token from the source file. */
int lex(void);

#endif
