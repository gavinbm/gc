#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

char *readfile(char *filename) {
    FILE *fp = fopen(filename, "r");
    char *buffer = NULL, c;
    int length = 0, i = 0;

    while((c = fgetc(fp)) != EOF)
        length++;  
    
    if(length) {
        fseek(fp, 0L, SEEK_SET);
        buffer = malloc(length + 1);
        
        while((c = fgetc(fp)) != EOF)
            buffer[i++] = c;
        
        buffer[length] = '\0';
        fclose(fp);
    } else {
        puts("couldn't read file...");
        exit(50);
    }

    return buffer;
} 

enum {LPAREN, RPAREN, PLUS, MINUS, STAR, SLASH, POWER, GREAT,
      LESS, EQUAL, NOT, INT, FLOAT, IDENT, COMMA, 
      LOCAL, GLOBAL, IF, THEN, ELSE, WHILE, DO, GOTO, FUNCTION, END};

char *pos, tok[64];
int type, line = 0, val;

/*  lexer  */
void lexical_error() {
    printf("lexical error: bad char [%c] in line [%d]...\n", *pos, line);
    exit(1);
}

int iskey(char *s) {
    char words[10][9] = {"local", "global", "if", "then", "else", "while", "do", "goto", 
                        "function", "end"};
    
    for(int i = 0; i < 10; i++) {
        if(strcmp(words[i], s) == 0)
            return i + LOCAL;
    }

    return 0;
}

void skipws() {
    while(*pos == ' ' || *pos == '\n') {
        if(*pos == '\n') { line++;}
        pos++;
    }
}

void next() {
    int i = 0, key;
    skipws();
    switch(*pos) {
        case '\0': type = EOF; break;
        case '#': while(*pos != '\n') {pos++;} pos++; line++; break;
        case '(': type = LPAREN; pos++; break;
        case ')': type = RPAREN; pos++; break;
        case '+': type = PLUS; pos++; break;
        case '-': type = MINUS; pos++; break;
        case '*': type = STAR; pos++; break;
        case '/': type = SLASH; pos++; break;
        case ',': type = COMMA; pos++; break;
        case '>': type = GREAT; pos++; break;
        case '<': type = LESS; pos++; break;
        case '=': type = EQUAL; pos++; break;
        case '!': type = NOT; pos++; break;
        default:
            if(*pos >= '0' && *pos <= '9') {
                val = 0;
                while(*pos >= '0' && *pos <= '9') {
                    val = val * 10 + (*pos - '0'); pos++;
                }

                tok[i] = '\0'; type = INT;
            }
            else if(isalpha(*pos)) {
                while(isalpha(*pos) || *pos == '_') {
                    tok[i++] = *pos; pos++;     
                }

                tok[i] = '\0';
                if(key = iskey(tok))
                    type = key;
                else
                    type = IDENT;

            }  else {
                lexical_error();
            }
    }
}

/* symbol table */
typedef struct sym {
    char name[64];
    int value;
    int scope;
} sym;

int isvar(char *s) {

}

int isfunc(char *s) {

}

/*  parser  */
void syntax_error() {
    printf("syntax error: bad token [%s -- %d] at line [%d]...\n", tok, type, line);
    exit(3);
}

void match(int t) {
    if(t != type) { printf("match: expected %d and got %d...\n", t, type); exit(3);}
    next();
}

void primary() {
    puts("PRIMARY");
    if(type == IDENT) {
        puts("here");
        next();
    }
    else if(type == INT || type == FLOAT) {
        next();
        printf("[%s] -- %d\n", tok, type);
    } else {
        syntax_error();
    }
}

void unary() {
    puts("UNARY");
    if(type == MINUS || type == PLUS) {next();} 
    
    primary();
}

void term() {
    puts("TERM");
    unary();
    while(type == STAR || type == SLASH) {next(); unary();}
}

void expr() {
    puts("EXPR");
    term();
    while(type == PLUS || type == MINUS) {next(); term();}
}

void comp() {
    puts("COMP");

    expr();
    if(type == GREAT || type == LESS || type == NOT || type == EQUAL) {
        next(); expr();
    } else {
        syntax_error();
    }

    while(type == GREAT || type == LESS || type == NOT || type == EQUAL) {
        next(); expr();
    }
}

void argv() {
    puts("ARGV");
    match(IDENT);
    while(type == COMMA) {next(); match(IDENT);}
}

void stmnt() {
    
    switch(type) {
        case GLOBAL:
        case LOCAL:
            puts("VARDECL");
            next();

            if(type == FUNCTION) { // func decl
                next(); match(IDENT);
                // add ident and argv to sym table
                match(LPAREN); argv(); match(RPAREN);
                while(type != END)
                    stmnt();
                match(END);
            } 
            else if(type == IDENT) { // var decl
                next();
                if(type == EQUAL) {
                    // add var to sym table, set equal to expr
                    next();
                    expr();
                } else {
                    // add var to sym table, set value to NULL
                }
            } else {
                syntax_error();
            }

            break;
        case IDENT:
            puts("IDENT -- what could it be??");
            if(/*isvar(tok)*/1) { // var assign
                next(); match(EQUAL); expr();
            } 
            else if(isfunc(tok)) { // func call
                next(); match(LPAREN); argv(); match(RPAREN);
            } else {
                syntax_error();
            }

            break;
        case IF:
            puts("IF");
            next(); comp(); match(THEN);
            while(type != END || type != ELSE)
                stmnt();
            
            if(type == ELSE) {
                while(type != END)
                    stmnt();
            }

            match(END);
            break;
        case WHILE:
            puts("WHILE");
            next(); comp(); match(DO);
            while(type != END)
                stmnt();
            
            match(END);

            break;
        case GOTO:
            puts("GOTO");
            next(); expr();
            break;
        default:
            puts("UNRECOGNIZED");
            syntax_error();
            break;
    }
    next();
}

void prog() {
    skipws();
    next();
    while(type != EOF)
        stmnt();
}

/*  main    */
int main(int argc, char **argv) {
    char *input = readfile(argv[1]);
    pos = input;

    prog();

    free(input);
    return 0;
}
