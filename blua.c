#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ======================= State ======================= */
enum {LPAREN, RPAREN, PLUS, MINUS, STAR, SLASH, POWER, GREAT,
      LESS, EQUAL, NOT, INT, FLOAT, IDENT, COMMA, 
      VAR, IF, THEN, ELSE, WHILE, DO, GOTO, FUNCTION, END};

char *pos, tok[32];
int type, line = 0, val;
/* ======================================================= */
/* ======================= Utility ======================= */
char errors[24][10] = {
    "LPAREN", "RPAREN", "PLUS", "MINUS", "STAR", "SLASH", "POWER", "GREAT",
    "LESS", "EQUAL", "NOT", "INT", "FLOAT", "IDENT", "COMMA", "VAR", "IF",
    "THEN", "ELSE", "WHILE", "DO", "GOTO", "FUNCTION", "END"
};

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

int iskey(char *s) {
    char words[9][6] = {"var", "if", "then", "else", "while", "do", "goto", 
                        "func", "end"};
    
    for(int i = 0; i < 9; i++) {
        if(strcmp(words[i], s) == 0)
            return i + VAR;
    }

    return 0;
}

/* ===================================================== */
/* ======================= Lexer ======================= */
void next() {
    int i = 0, key;
    switch(*pos) {
        case '\0': case EOF: type = EOF; break;
        case ' ': case '\n': case '\t': pos++; next(); break;
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
                if(*pos == '.') {
                    while(*pos >= '0' && *pos <= '9') {
                        val = val * 10 + (*pos - '0'); pos++;
                    }

                    type = FLOAT;
                } else {type = INT;}

                tok[i] = '\0';
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
                printf("invalid char [%c]\n", *pos);
                exit(1);
            }
    }
    
}

/* ===================================================== */
/* ======================= Parser ======================= */
void match(int t) {if(type != t) {
    printf("match: expected %s got %s\n", errors[t], errors[type]); exit(2);} 
    next();
}

void argv() { // argv ::= ident {"," ident}
    puts("ARGV");
    match(IDENT); 
    while(type == COMMA) {
        next(); match(IDENT);
    }
}

void primary() { // primary ::= number | ident
    puts("PRIMARY");
    if(type == IDENT || type == INT || type == FLOAT)
        next();
    else {
        puts("huh?"); exit(3);
    }
}

void unary() { // unary ::= ["+" | "-"] primary
    puts("UNARY");
    if(type == PLUS || type == MINUS) {
        next();
    }
     primary();
}

void term() { // term ::= unary {( "/" | "*" ) unary}
    puts("TERM");
    unary();
    while(type == SLASH || type == STAR) {
        next(); unary();
    }
}

void expr() { // expr ::= term {( "-" | "+" ) term}
    puts("EXPR");
    term();
    while(type == PLUS || type == MINUS) {next(); term();}
}

void comp() { // comp ::= expr (("=" | "!" | ">" | "<") expr)+
    puts("COMP");
    expr();
    if(type == NOT || type == GREAT || type == LESS || type == EQUAL) {
        next(); expr();
    }

    while(type == NOT || type == GREAT || type == LESS || type == EQUAL) {
        next(); expr();
    }
}

void stmnt() {
    switch(type) {
        case VAR: // "VAR" ident {"=" expr}
            puts("VAR DEC");
            next();
            match(IDENT);
            if(type == EQUAL) { next(); expr(); }
            break;
        case FUNCTION: // "FUNC" ident "(" argv ")" {stmnt} "END" 
            puts("FUNC DEC");
            next(); match(IDENT); match(LPAREN); argv(); match(RPAREN);
            while(type != END)
                stmnt();
            match(END);
            break;
        case IDENT: // ident "=" expr | ident "(" argv ")"
            puts("IDENT");
            next();
            if(type == EQUAL) {
                next(); expr();
            } else if(type == LPAREN) {
                next(); argv(); match(RPAREN);
            }
            break;
        case IF: // "IF" comp "THEN" {stmnt} "END" 
                 // "IF" comp "THEN" {stmnt} "ELSE" {stmnt} "END"
            puts("IF");
            next(); comp(); match(THEN);
            while(type != END && type != ELSE)
                stmnt();
            
            if(type == ELSE) {
                while(type != END)
                    stmnt();
            }
            match(END);
            break;
        case WHILE: // "WHILE" comp "DO" {stmnt} "END"
            puts("WHILE");
            next(); comp(); match(DO);
            while(type != END)
                stmnt();
            match(END);
            break;
        case GOTO: // "GOTO" expr
            puts("GOTO");
            next(); expr();
            break;
        default:
            printf("parse error: [%d] -- [%s]\n", type, tok); exit(4);
            break;
    }

}

void prog() {
    next();
    while(type != EOF)
        stmnt();
}

/* ====================================================== */
int main(int argc, char **argv) {
    char *input = readfile(argv[1]);
    pos = input;
    printf("%s",pos);
    prog();
    // while(type != EOF) {
    //     next(); printf("%d -- [%s]\n", type, tok);
    // }
    free(input);
    return 0;
}