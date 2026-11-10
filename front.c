

/*
=============================================================================
Title       : front.c
Description : Lexical analyzer for the Cooke programming language.
              Reads a source file, identifies lexemes, and outputs
              their corresponding token classifications.
Author      : Ryanne Russell (R#11895444)
Date        : 10/22/2026
Version     : 1.0
Usage       : ./front <source_file>
Notes       : Requires front.h and a valid source file.
              Recognizes identifiers, literals, keywords,
              operators, and unknown tokens.
C Version   : C11
=============================================================================
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "front.h"
#include "parser.h"


/*
 * These variables keep track of the current scanning state.
 *
 * nextToken stores the token type found by the analyzer.
 * charClass identifies whether the current character is a
 * letter, digit, unknown character, or EOF.
 * nextChar stores the current character from the input file.
 * in_fp points to the source file being analyzed.
 *
 * lexeme stores the characters collected for the current token.
 * lexLen tracks its length, and lexCapacity tracks the amount
 * of memory allocated for it.
 */

int nextToken;
int lineNumber = 1;
static int currentLine = 1;
static int charClass;
static int nextChar;
static FILE *in_fp;
/*
 * Project 2: The current lexeme must be accessible to the
 * syntax parser so it can identify the token responsible
 * for a syntax error.
 *
 * Removing static allows parser.c to access this variable
 * through an extern declaration in front.h.
 */
char *lexeme = NULL;
static size_t lexLen = 0;
static size_t lexCapacity = 0;


static void getChar(void);
static void getNonBlank(void);
static void addChar(void);
static int lookup(int ch);
const char *tokenName(int token);


/*
 * Adds the current character to the lexeme being built.
 */
static void addChar(void) {

    // Expand the lexeme buffer when needed.
    // Doubling its capacity reduces repeated memory allocations.
    if (lexLen + 2 > lexCapacity) {
        size_t newCapacity = lexCapacity ? lexCapacity * 2 : 128;
        char *newLexeme = realloc(lexeme, newCapacity);

        if (newLexeme == NULL) {
            fprintf(stderr, "Error: Out of memory.\n");
            free(lexeme);

            if (in_fp != NULL) {
                fclose(in_fp);
            }

            exit(EXIT_FAILURE);
        }

        lexeme = newLexeme;
        lexCapacity = newCapacity;
    }

    lexeme[lexLen++] = (char)nextChar;
    lexeme[lexLen] = '\0';
}

// Reads the next character from the input file and
// classifies it as a letter, digit, unknown, or EOF.
static void getChar(void) {
    if (nextChar == '\n') {
        currentLine++;
    }

    nextChar = getc(in_fp);

    if (nextChar == EOF) {
        charClass = EOF;
    }
    else if ((nextChar >= 'a' && nextChar <= 'z') ||
             nextChar == '_') {
        charClass = LETTER;
    }
    else if (nextChar >= '0' && nextChar <= '9') {
        charClass = DIGIT;
    }
    else {
        charClass = UNKNOWN;
    }
}

// Skips whitespace until a non-whitespace character
// or the end of the input file is reached.
static void getNonBlank(void) {
    while (nextChar != EOF && isspace((unsigned char)nextChar)) {
        getChar();
    }
}


// Converts a token code into its corresponding
// token name for output.
const char *tokenName(int token) {
    switch (token) {
        case IDENT: return "IDENT";
        case INT_LITERAL: return "INT_LITERAL";
        case FLOAT_LITERAL: return "FLOAT_LITERAL";
        case BOOL_LITERAL: return "BOOL_LITERAL";
        case ASSIGNMENT_OP: return "ASSIGNMENT_OP";
        case LEFT_PAREN: return "LEFT_PAREN";
        case RIGHT_PAREN: return "RIGHT_PAREN";
        case LEFT_BRACE: return "LEFT_BRACE";
        case RIGHT_BRACE: return "RIGHT_BRACE";
        case SEMICOLON: return "SEMICOLON";
        case COLON: return "COLON";
        case INC_OP: return "INC_OP";
        case DEC_OP: return "DEC_OP";
        case ADD_OP: return "ADD_OP";
        case SUB_OP: return "SUB_OP";
        case MULT_OP: return "MULT_OP";
        case DIV_OP: return "DIV_OP";
        case MOD_OP: return "MOD_OP";
        case POWER_OP: return "POWER_OP";
        case LESSER_OP: return "LESSER_OP";
        case GREATER_OP: return "GREATER_OP";
        case EQUAL_OP: return "EQUAL_OP";
        case NEQUAL_OP: return "NEQUAL_OP";
        case LEQUAL_OP: return "LEQUAL_OP";
        case GEQUAL_OP: return "GEQUAL_OP";
        case BOOLEAN_AND: return "BOOLEAN_AND";
        case BOOLEAN_OR: return "BOOLEAN_OR";
        case BOOLEAN_NOT: return "BOOLEAN_NOT";
        case KEY_IF: return "KEY_IF";
        case KEY_ELIF: return "KEY_ELIF";
        case KEY_ELSE: return "KEY_ELSE";
        case KEY_FOR: return "KEY_FOR";
        case KEY_WHILE: return "KEY_WHILE";
        case KEY_READ: return "KEY_READ";
        case KEY_PRINT: return "KEY_PRINT";
        case UNKNOWN: return "UNKNOWN";
        case EOF: return "EOF";
        default: return "UNKNOWN";
    }
}


// Identifies operators and punctuation, checking
// for multi-character operators when applicable.
static int lookup(int ch) {
    switch (ch) {
        case '(':
            addChar(); nextToken = LEFT_PAREN; getChar(); break;
        case ')':
            addChar(); nextToken = RIGHT_PAREN; getChar(); break;
        case '{':
            addChar(); nextToken = LEFT_BRACE; getChar(); break;
        case '}':
            addChar(); nextToken = RIGHT_BRACE; getChar(); break;
        case ';':
            addChar(); nextToken = SEMICOLON; getChar(); break;
        case '/':
            addChar(); nextToken = DIV_OP; getChar(); break;
        case '%':
            addChar(); nextToken = MOD_OP; getChar(); break;
        case '!':
            addChar(); getChar();
            if (nextChar == '=') {
                addChar(); nextToken = NEQUAL_OP; getChar();
            } else nextToken = BOOLEAN_NOT;
            break;
        case '+':
            addChar(); getChar();
            if (nextChar == '+') {
                addChar(); nextToken = INC_OP; getChar();
            } else nextToken = ADD_OP;
            break;
        case '-':
            addChar(); getChar();
            if (nextChar == '-') {
                addChar(); nextToken = DEC_OP; getChar();
            } else nextToken = SUB_OP;
            break;
        case '*':
            addChar(); getChar();
            if (nextChar == '*') {
                addChar(); nextToken = POWER_OP; getChar();
            } else nextToken = MULT_OP;
            break;
        case ':':
            addChar(); getChar();
            if (nextChar == '=') {
                addChar(); nextToken = ASSIGNMENT_OP; getChar();
            } else nextToken = COLON;
            break;
        case '=':
            addChar(); getChar();
            if (nextChar == '=') {
                addChar(); nextToken = EQUAL_OP; getChar();
            } else nextToken = UNKNOWN;
            break;
        case '<':
            addChar(); getChar();
            if (nextChar == '=') {
                addChar(); nextToken = LEQUAL_OP; getChar();
            } else nextToken = LESSER_OP;
            break;
        case '>':
            addChar(); getChar();
            if (nextChar == '=') {
                addChar(); nextToken = GEQUAL_OP; getChar();
            } else nextToken = GREATER_OP;
            break;
        case '&':
            addChar(); getChar();
            if (nextChar == '&') {
                addChar(); nextToken = BOOLEAN_AND; getChar();
            } else nextToken = UNKNOWN;
            break;
        case '|':
            addChar(); getChar();
            if (nextChar == '|') {
                addChar(); nextToken = BOOLEAN_OR; getChar();
            } else nextToken = UNKNOWN;
            break;
        case '.':
            addChar(); getChar();
            if (charClass == DIGIT) {
                do {
                    addChar(); getChar();
                } while (charClass == DIGIT);
                nextToken = FLOAT_LITERAL;
            } else nextToken = UNKNOWN;
            break;
        
        default:
            addChar();
            nextToken = UNKNOWN;
            getChar();
            break;

    }
    return nextToken;
}



// Collects characters into lexemes, determines
// their token classifications, and prints results.
int lex(void) {
    lexLen = 0;
    if (lexeme != NULL) lexeme[0] = '\0';
    getNonBlank();
    lineNumber = currentLine;
    switch (charClass) {
        case LETTER:
            do {
                addChar(); getChar();
            } while (charClass == LETTER || charClass == DIGIT);
            
            // Compare the completed lexeme against reserved keywords
            // and boolean literals before classifying it as an identifier.

            if (strcmp(lexeme, "if") == 0) nextToken = KEY_IF;
            else if (strcmp(lexeme, "elif") == 0) nextToken = KEY_ELIF;
            else if (strcmp(lexeme, "else") == 0) nextToken = KEY_ELSE;
            else if (strcmp(lexeme, "for") == 0) nextToken = KEY_FOR;
            else if (strcmp(lexeme, "while") == 0) nextToken = KEY_WHILE;
            else if (strcmp(lexeme, "read") == 0) nextToken = KEY_READ;
            else if (strcmp(lexeme, "print") == 0) nextToken = KEY_PRINT;
            else if (strcmp(lexeme, "true") == 0 || strcmp(lexeme, "false") == 0)
                nextToken = BOOL_LITERAL;
            else nextToken = IDENT;
            break;
        case DIGIT:
            do {
                addChar(); getChar();
            } while (charClass == DIGIT);
            if (nextChar == '.') {
                addChar(); getChar();
                while (charClass == DIGIT) {
                    addChar(); getChar();
                }
                nextToken = FLOAT_LITERAL;
            } else nextToken = INT_LITERAL;
            break;
        case UNKNOWN:
            lookup(nextChar);
            break;
        case EOF:
            nextToken = EOF;
            break;
        default:
            nextToken = UNKNOWN;
            addChar(); getChar();
            break;
    }

/*
 * Project 2: Return the recognized token to the syntax parser
 * instead of printing each lexeme/token pair.
 *
 * The recursive-descent parser uses nextToken to determine
 * which grammar rule to apply next.
 */
return nextToken;
    return nextToken;
}

/*
 * Main entry point for the Cooke parser.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: No input file provided.\n");
        return 2;
    }

    in_fp = fopen(argv[1], "r");

    if (in_fp == NULL) {
        fprintf(stderr,
                "Error: Cannot open file %s\n",
                argv[1]);
        return 3;
    }

    printf("Cooke Parser :: R11895444\n");

    /* Initialize the lexer. */
    nextChar = 0;
    currentLine = 1;
    lineNumber = 1;

    getChar();
    lex();

    /* Parse the complete Cooke program. */
    P();

    /* Reject extra tokens after the program. */
    if (nextToken != EOF) {
        fprintf(stderr,
                "Error encountered on line %d: "
                "The next lexeme was %s and the next token was %s\n",
                lineNumber,
                lexeme != NULL ? lexeme : "EOF",
                tokenName(nextToken));

        fclose(in_fp);
        free(lexeme);
        return 1;
    }

    printf("Syntax Validated\n");

    fclose(in_fp);
    free(lexeme);

    return 0;
}

