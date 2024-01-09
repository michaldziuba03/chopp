#include <vector>
#include <string>
#include <set>
#include "utf8.cc"

#include <iostream>

enum TokenType {
    KEYWORD,
    WORD,
    VARIABLE,
    STRING,
    OPERATOR,
    DIGIT,
    COMMENT,
    MACRO,
};

struct Token {
    TokenType type;
    std::string value;
    size_t pos = 0;
    size_t len = 1;
};

// for now just take c++ keywords here
const std::set<std::string> keywords = {
    "using",
    "inline",
    "namespace",
    "if",
    "else",
    "return",
    "const",
    "enum",
    "struct",
    "class",
    "public",
    "private",
    "virtual",
    "switch",
    "case",
    "break",
    "continue",
    "for",
    "while",
    "do",
    "void",
    "int",
    "char",
    "long",
    "double",
    "float",
    "short",
    "bool",
    "template",
    "auto",
    "throw",
    "try",
    "catch",
    "extern",
    "typedef",
    "unsigned",
    "signed",
    "union",
    "sizeof",
    "size_t",
    "wchar_t",
    "and"
};

const std::set<char> operators = { '+', '-', '/', '*', '&', '|', '%', '=', '<', '>' };

inline bool isStringQuote(char c) {
    return c == '"';
}

inline bool isStartWord(char c) {
    return isalpha(c) || c == '_';
}

inline bool isWord(char c) {
    return isalnum(c) || c == '_';
}

inline bool isOperator(char c) {
    return operators.count(c) > 0;
}

inline bool isMacroStart(char c) {
    return c == '#';
}

std::set<std::string> macros = {"#include", "#define"};
bool isMacro(const std::string& str) {
    return macros.count(str) > 0;
}

void tokenize(std::vector<Token>& tokens, const std::string& code) {
    size_t offset = 0;
    while (offset < code.length()) {
        char c = code[offset];

        if (c == ' ') {
            offset++;
            continue;
        }

        if(isdigit(c)) {
            std::string val = std::string(1, c);
            tokens.push_back(Token { DIGIT, val, offset });
            offset++;
            continue;
        }

        if(isOperator(c)) {
            std::string val = std::string(1, c);
            tokens.push_back(Token {  OPERATOR, val, offset });
            offset++;
            continue;
        }

        if (isMacroStart(c)) {
            size_t pos = offset;
            std::string word = std::string(1, c);
            offset++;
            while (offset < code.length()) {
                c = code[offset];
                if (!isWord(c)) {
                    break;
                }
                offset++;
                word.push_back(c);
            }

            if (isMacro(word)) {
                size_t wordLen = word.length();
                tokens.push_back(Token { MACRO, word, pos, wordLen });
                continue;
            }
        }

        if (isStartWord(c)) {
            size_t pos = offset;
            std::string word = std::string(1, c);
            offset++;
            while (offset < code.length()) {
                c = code[offset];
                if (!isWord(c)) {
                    break;
                }
                offset++;
                word.push_back(c);
            }

            size_t wordLen = word.length(); //columnLen(word);
            if (keywords.count(word)) {
                tokens.push_back(Token { KEYWORD, word, pos, wordLen });
            } else {
                tokens.push_back(Token { WORD, word, pos, wordLen });
            }

            continue;
        }

        if (isStringQuote(c)) {
            size_t pos = offset;
            std::string val = std::string(1, c);
            offset++;
            while(offset < code.length()) {
                c = code[offset];
                if (isStringQuote(c)) {
                    val.push_back(c);
                    offset++;
                    break;
                }
                offset++;
                val.push_back(c);
            }

            size_t strLen = val.length();
            tokens.push_back(Token { STRING, val, pos, strLen });
            continue;
        }

        if (c == '\'') {
            size_t pos = offset;
            std::string val = std::string(1, c);
            offset++;
            while(offset < code.length()) {
                c = code[offset];
                if (c == '\'') {
                    val.push_back(c);
                    offset++;
                    break;
                }
                offset++;
                val.push_back(c);
            }

            size_t strLen = val.length();
            tokens.push_back(Token { STRING, val, pos, strLen });
            continue;
        }

        offset++;
    }
}
