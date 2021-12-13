#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lexeme {
    char *tok;
    int type;
} typedef lexeme;
char *pos;

enum {SET, IF, ELSE, DO, UNTIL, DEF, RET, LBR, RBR, LPA, RPA,
      PLS, MIN, STR, SLH, LES, GRT, EQL, NLN, IDE, NUM, EOI};

int vars[26] = {0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0};

int iskey(char *src) {
    char keywords[7][7] = {"set", "if", "else", "do", 
                        "until", "def", "return"};

    for(int i = 0; i < 7; i++) {
        if(strcmp(keywords[i], src) == 0)
            return i;
    }

    return IDE;
}

char *readfile(char *filename, int *len) {
    FILE *fp = fopen(filename, "r");
    char *buffer = NULL;
    long length;

    if(fp) {
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if(buffer) {
            fread(buffer, 1, length, fp);
        }
        fclose(fp);
    }
    *len = length;
    return buffer;
} 

void initlexeme(lexeme **lexer, char *tok, int type, int size) {
    (*lexer) = malloc(sizeof(lexeme));
    (*lexer)->tok = malloc(size + 1);
    memcpy((*lexer)->tok, tok, size);
    (*lexer)->tok[size] = '\0';
    (*lexer)->type = type;
}

lexeme *next() {

    lexeme *new = NULL;

    char *peek = pos;
    int len = 0;

    switch (*pos) {
        case EOF: break;
        case ' ': pos++; break;
        case '\t': pos++; break;
        case '\n': initlexeme(&new, "\\n", NLN, 2); pos++; break;
        case '{': initlexeme(&new, pos, LBR, 1); pos++; break;
        case '}': initlexeme(&new, pos, RBR, 1); pos++; break;
        case '(': initlexeme(&new, pos, LPA, 1); pos++; break;
        case ')': initlexeme(&new, pos, RPA, 1); pos++; break;
        case '+': initlexeme(&new, pos, PLS, 1); pos++; break;
        case '-': initlexeme(&new, pos, MIN, 1); pos++; break;
        case '*': initlexeme(&new, pos, STR, 1); pos++; break;
        case '/': initlexeme(&new, pos, SLH, 1); pos++; break;
        case '<': initlexeme(&new, pos, LES, 1); pos++; break;
        case '>': initlexeme(&new, pos, GRT, 1); pos++; break;
        case '=': initlexeme(&new, pos, EQL, 1); pos++; break;
        default:
            // if it's a letter
            if(*pos >= 'a' && *pos <= 'z') {
                while(*peek >= 'a' && *peek <= 'z') {
                    peek++; len++;
                }
                pos[len] = '\0';
         
                initlexeme(&new, pos, iskey(pos), len + 1);
            }
            // if it's a decimal number
            else if(*pos >= '0' && *pos <= '9') {
                while(*peek >= '0' && *peek <= '9') {
                    peek++; len++;
                }

                // floating points aren't supported
                if(*peek == '.') {
                    puts("No floating points...");
                    exit(4);
                }

                initlexeme(&new, pos, NUM, len);
            }
            // invalid character encounter
            else {
                printf("invalid char [%c]\n", *pos);
                exit(1);
            }

            pos = peek + 1;

            break;
    }

    return new;
}

int main(int argc, char **argv) {

    int length;
    char *input = readfile(argv[1], &length);

    if(input) {
        input[length] = '\0';
        pos = input;
        lexeme *lex;

        while(*pos != '\0') {
            lex = next(lex);
            if(lex) {
                printf("[%s] -- [%d]\n", lex->tok, lex->type);
                free(lex->tok);
                free(lex);
            }
        }

        free(input);
    }

    return 0;
}
