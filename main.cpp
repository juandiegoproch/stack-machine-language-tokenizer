#include <iostream>
#include <fstream>
#include <sstream>
#include "token.h"


class Scanner {
private:
/*
    'first' points to the start of the lexeme
    'current' points to currently seen character

    int test_variable = 0;
        ^first   ^current
*/
    std::string input;
    int first, current;
    int state;

    char nextChar(){ // advance
        char c = input[current];
        if (c != '\0') {
            ++current;
        }
        return c;
    }

    void rollBack(){ // spit
        if (input[current] != '\0'){
            --current;
        }
    }

    void startLexeme(){ 
        first = current - 1;
    }

    void incrementStartLexeme() {
        ++first;
    }

    std::string getLexeme(){ // cut and return lexeme
        std::string a = input.substr(first, current - first);
        for (auto& i:a) // I hate CPP and strings...
            i = tolower(i);
        return a;
    }

    static Token* verifyKeywords(const std::string& lexeme) {
        auto it = hashtable.find(lexeme);
        if (it != hashtable.end()) {
            return new Token(it->second);
        }
        return new Token(ID, lexeme);
    }

public:

    explicit Scanner(const char* buffer): first(0), current(0), state(0){
        input = buffer;
    }

    Token* nextToken(){
        Token* token;
        char c = nextChar();
        state = 0;

        startLexeme();

        // Implemented automata:

        while (1) {
            switch (state) {
                case 0: {
                    if (c == ' ' || c == '\t') { // skip whitespace
                        incrementStartLexeme();
                        c = nextChar(); // advance over the whitespace
                        state = 0;
                    }
                    else if (isalpha(c)) {
                        state = 1;
                    }
                    else if (isdigit(c)) {
                        state = 4;
                    }
                    else if (c == '\n') {
                        state = 6;
                    }
                    else if (c == '\0') {
                        state = 8;
                    }
                    break;
                }
                case 1: {
                    c = nextChar(); // Keep consuming stuff.
                    if (c == ':') {
                        state = 3; // label
                    }
                    else if (isalnum(c) || c == '_') {
                        state = 1;
                    }
                    else {
                        state = 2; // id
                    }
                    break;
                }
                case 2: {
                    // Discriminate keywords, Return ID if not a keyword
                    rollBack();
                    return verifyKeywords(getLexeme());
                }
                case 3: {
                    return new Token(LABEL,getLexeme());
                    break;
                }
                case 4: {
                    c = nextChar(); // keep conusming stuff
                    if (isdigit(c))
                        state = 1;
                    else
                        state = 5;
                    break;
                }
                case 5: {
                    // emmit a number
                    rollBack();
                    return new Token(NUM,getLexeme());

                    break;
                }
                case 6: {
                    c = nextChar();
                    if (c != '\n')
                    {
                        rollBack();
                        return new Token(EOL);
                    }
                    break;
                }
                case 7: {

                    break;
                }
                case 8: {
                    return new Token(END);
                }
            }
        }
    }

    ~Scanner() = default;

};

int main(int argc, char** argv) {
    std::fstream file(argv[1], std::ios::in);
    std::stringstream stream;
    stream << file.rdbuf();

    Scanner scanner(stream.str().c_str());

    Token* tk = scanner.nextToken();
    while (tk->type != END) {
        std::cout << "Next Token " << *tk << std::endl;
        delete tk;
        tk =  scanner.nextToken();
    }

    return 0;
}
