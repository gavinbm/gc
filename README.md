# EZ Lang!

This is the repo for the EZ programming language, made by a college senior who finally has some free time now that the semester is ending. ez started as a very small subset of C
but after conversing with some of my friends who don't study computer science or do programming, I found that some language constructs I hadn't considered were more intuitive
for them. Since I want people to be able to use this abomination of a language, I made those changes and now it's a bit more interesting. It'll still hopefully be, at heart, a
systems language (i.e. gives you access to pointers) but likely with more going on under the hood than C. As of right now this will likely end up as an interpreted language
(yes I understand that conflicts with the systems language thing, I'll cover that in a second) with the eventual goal of it compiling to the THUMB subset of ARM assembly (see? 
told you I'd cover it). All in all this is really just a continuation of my study of compilers and a way for me to improve.

I'm going to break down how each major component was designed and what concepts I tried implementing and how they went. Hopefully someone learns from this as much or more than I
did.

# The Language and Grammar

My goal is to make a simple language that people find intuitive to use for general purpose tasks, though later it will be specialized to be intuitive for systems programming, targeting ARM processors or some ARM processor in particular. Here is the full grammar and some code examples

```
<program>    ::= <stmnt>
<stmnt>      ::= "set" <id> "=" <expr> <nl>
               | "if" <paren_expr> "{" <stmnt> "}" 
               | "if" <paren_expr> "{" <stmnt> "}" "else" "{" <stmnt> "{"
               | "do" "{" <stmnt> "}" "until" <paren_expr>
               | "def" <id> "(" <argv> ")" "{" <stmnt> "}"
               | <nl>
<paren_expr> ::= "(" <expr> ")"
<argv>       ::= list of comma separated identifiers
<expr>       ::= <test> | <id> "=" <expr>
<test>       ::= <sum> | <sum> "<" <sum>
<sum>        ::= <term> | <sum> "+" <term> | <sum> "-" <term>
<term>       ::= <id> | <int> | <paren_expr>
<id>         ::= "a" | "b" | "c" | "d" | ... | "z"
<int>        ::= <an_unsigned_decimal_integer>
<nl>         ::= newline

--------------------------------------------------------
 --- Examples of doing basic things in the language ---
--------------------------------------------------------
variables: we're using the set keyword
    set x = y

functions: we're using def like this
    def myfunc(argv) {
        code...
    }

conditionals: basic if-else structure
    if (condition) then {
        code a...
    } else {
        code b...
    }

call funcs: standard, nothing changes
    myfunc(argv)

iteration: we're using do loops of the following syntax:
    do {
        code...
    } until (condition)
```

I decided on this style from a few conversations I had with friends of mine who have no programming or computer science experience. Honestly I was most surprised that the curly braces were preferred and didn't confuse anyone, I was told they did a good job of compartmentalizing the code into sections.

# The Lexer

This was fun, I used what's called an "immutable lexer" which as far as I can tell from my research doesn't exist outside of [this](https://stackoverflow.com/questions/44336831/why-should-strtok-be-deprecated/) stack overflow post.
The basic idea is that you use a struct to keep track of what would otherwise be the global state of your lexer. In previous compilers I've made the lexer generate a linked list
of tokens that gets passed to the parser. Don't do that, that's gross, it's inefficient, you'll learn a lot about linked lists, but it takes up a lot of space. With this new
immutable lexer, you free the lexer as you go, reducing the amount of space you're taking up.

The immutable lexer struct itself looks like this in the code

```C
struct lexer {
    char *tok;
    char *pos;
    int type;
} typedef lexer;
```

Where tok is the current token we've extracted from the string, pos is our position in the string, and type is the type of token we've extracted. Typically this could all be
done via globals and tbh with you, it's likely easier to do it that way. It also may be more efficient since you're not malloc'ing the lexer struct all the time, idk really,
I just thought this was a cool idea when I read that stack overflow post so I ran with it.

The actual lexing is done via the next() function in our code. The way this thing will work is it'll take in the old lexer, use that state to get a position, then create and 
return a new lexer with the next token, the updated position, and the new type information. As I'm writing this I'm realizing that this is just a lexeme struct with a pointer
to the string position slammed in it, which seems a bit lame but whatever, the code is written.
```C
lexer *next(lexer *old) {

    lexer *new = malloc(sizeof(lexer));
    new->tok = NULL;

    char *peek = old->pos, *substr;
    int len = 0;

    switch (*peek) {
        case EOF: break;
        case ' ': peek++; break;
        case '\t': peek++; break;
        case '\n': initlexer(&new, "\\n", NLN, 2); peek++; break;
        case '{': initlexer(&new, peek, LBR, 1); peek++; break;
        case '}': initlexer(&new, peek, RBR, 1); peek++; break;
        case '(': initlexer(&new, peek, LPA, 1); peek++; break;
        case ')': initlexer(&new, peek, RPA, 1); peek++; break;
        case '+': initlexer(&new, peek, PLS, 1); peek++; break;
        case '-': initlexer(&new, peek, MIN, 1); peek++; break;
        case '*': initlexer(&new, peek, STR, 1); peek++; break;
        case '<': initlexer(&new, peek, LES, 1); peek++; break;
        case '>': initlexer(&new, peek, GRT, 1); peek++; break;
        case '=': initlexer(&new, peek, EQL, 1); peek++; break;
        default:
            // if it's a letter
            if(*peek >= 'a' && *peek <= 'z') {
                while(peek[len] >= 'a' && peek[len] <= 'z') {
                    len++;
                }
         
                substr = malloc(len + 1);
                memcpy(substr, peek, len);
                substr[len] = '\0';
                initlexer(&new, peek, iskey(substr), len);
                free(substr);
            }
            // if it's a decimal number
            else if(*peek >= '0' && *peek <= '9') {
                while(peek[len] >= '0' && peek[len] <= '9') {
                    len++;
                }

                // floating points aren't supported
                if(peek[len] == '.') {
                    puts("No floating points...");
                    exit(4);
                }

                initlexer(&new, peek, NUM, len);
            }
            // invalid character encounter
            else {
                printf("invalid char [%c]\n", *peek);
                exit(1);
            }

            peek = peek + len + 1;
            break;
    }

    free(old->tok);
    free(old);
    new->pos = peek;
    return new;
}
```

All in all the immutable lexer technique seems pretty cool. I think this would be better suited to a language like Java which pretty much enforces the use of structures (classes 
are just structs with function pointers and you can't convince me I'm wrong about that) but it's still been a neat way to really think about how my lexer is working.
