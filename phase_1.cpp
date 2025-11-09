#include <iostream>
#include <string>
#include <string.h>
#include <regex>

using namespace std;


void trimNewline(string &s){
    while(!s.empty() && (s[s.size() -1] == '\n' || s[s.size() -1] == '\r')){
        s.pop_back();
    }
}

// we transform a string in uppercase
string makeUpper(const string &s){
    string r = s;
    for(size_t i = 0; i < r.size(); ++i){
        char c = r[i];
        if(c >= 'a' && c <= 'z'){
            r[i] = (char)(c - 'a' + 'A');
        }
    }
    return r;
}

// we check if a char is a null space
bool myIsSpace(char c){
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// we check if a char is a number
bool myIsDigit(char c){
    return c >= '0' && c <= '9';
}
 
// we check if a string is an integer
bool isIntLiteral(const string &s){
    if(s.size() == 0) 
        return false;
    
    int i = 0;
    if(s[0] == '+' || s[0] == '-'){
        if (s.size() == 1) 
            return false;
        i = 1;
    }

    for(i = 0; i < (int)s.size(); i++){
        if(!myIsDigit(s[i]))
            return false;
    }
    return true;
}

 // we check if a string is a float and the number of digits
bool isFloatLiteral(const string &s){
    if(s.size() == 0)
        return false;

    int i = 0;

    if(s[0] == '+' || s[0] == '-'){
        if(s.size() == 1)
            return false;
        i = 1;
    }

    bool hasDigit = false;
    bool hasDot = false;

    for(i = 0; i < (int)s.size(); i++){
        char c = s[i];
        if(c == '.'){
            if(hasDot) return false;
            hasDot = true;
        }else if(myIsDigit(c)){
            hasDigit = true;
        }else return false;
    }

    return hasDigit;
}

// we change a string into an integer
int stringToInt(const string &s){
    int i = 0;
    int semn = 1;
    if(s[0] == '-'){
        semn = -1;
        i = 1;
    }else if(s[0] == '+')
        i = 1;
    
    int val = 0;
    for(i = 0; i < (int)s.size(); i++){
        val = val * 10 + (s[i] - '0');
    }    

    return val * semn;
}

bool equalIgnoreCase(const string &a, const string &b){
    return makeUpper(a) == makeUpper(b);
}

// we definne the token type
enum TokenType{
    TK_WORD, // IF ITS A WORD
    TK_COMMA, // IF ITS A COMMA ","
    TK_LPAREN, // IF ITSA LEFT PAREN "("
    TK_RPAREN, // IF ITS A RIGHT PAREN ")"
    TK_EQUAL, // IF ITS AN EQUAL
    TK_STAR, // IF ITS A STAR "*"
    TK_STRING, //CHECKS IF ITS USING '' OR ""
    TK_END // END OF THE LINE
};

// token structure
struct Token{
    TokenType type;
    string text;
};

// static array
const int MAX_TOKENS = 256;

struct TokenBuffer {
    Token items[MAX_TOKENS];
    int count;

    TokenBuffer(){
        count = 0;
    }

    void add(TokenType t, const string &txt){
        if(count >= MAX_TOKENS)
            return;
        items[count].type = t;
        items[count].text = txt;
        count++;    
    }
};

//we use the tokenizer
void tokenize(const string &line, TokenBuffer &buf){
    buf.count = 0;
    int i = 0;
    int n = (int)line.size();

    while(i < n){
        char c = line[i];

        if(myIsSpace(c)){ //if we have null spaces we move forward
            i++;
            continue;
        }

        if(c == ','){
            buf.add(TK_COMMA, ",");
            i++;
            continue;
        }

        if(c == '('){
            buf.add(TK_LPAREN, "(");
            i++;
            continue;
        }

        if(c == ')'){
            buf.add(TK_RPAREN, ")");
            i++;
            continue;
        }

        if(c == '='){
            buf.add(TK_EQUAL, "=");
            i++;
            continue;
        }

        if(c == '*'){
            buf.add(TK_STAR, "*");
            i++;
            continue;
        }

        /*TK_STRING*/
        if (c == '"' || c == '\''){
            char q = c;
            i++;
            string val = "";

            while(i < n && line[i] != q){
                val += line[i];
                i++;
            }
            if(i < n && line[i] == q)
                i++;
            buf.add(TK_STRING, val);
            continue;
        }
        /*TK_WORD*/
        string word = "";
        while(i < n) {
            char d = line[i];
            if(myIsSpace(d) || d == ','|| d == '(' || d == ')' || d == '=' || d == '*' || d == '"' || d == '\'')
                break;
            word += d;
            i++;
        }

        if(!word.empty())
            buf.add(TK_WORD, word);
    }

    buf.add(TK_END, "");

}// end of tokenizer

int main(){
    return 0;
}
