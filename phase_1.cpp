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

enum CommandType{
    CMD_UNKNOWN,
    CMD_CREATE_TABLE,
};

class SqlCommand{
    string text;
    CommandType type;

public:
    static const int MAX_LEN = 511;

    SqlCommand(){
        text = "";
        type = CMD_UNKNOWN;
    }

    SqlCommand(const string &t, CommandType tp = CMD_UNKNOWN){
        text = t;
        type = tp;
    }

    SqlCommand(const SqlCommand &other){
        text = other.text;
        type = other.type;
    }

    SqlCommand& operator =(const SqlCommand &other){
        if(this != &other){
            text = other.text;
            type = other.type;
        }
        return *this;
    }

    string getText() const {
         return text;
     }

    CommandType getType() const{
        return type;
     } 

    void setType(CommandType t){
        type = t;
    }

    friend ostream& operator<<(ostream &out, const SqlCommand &c){
        out << "[CMD type = " << c.type << "]" << c.text;
        return out;
    }

}; // end of SqlCommand class

enum ColType{
    CT_TEXT,
    CT_INTEGER,
    CT_FLOAT,
    CT_UNKNOWN
};

class Column{
public:
    static const int MAX_NAME = 32;

private:
    string name;
    ColType type;
    int size;
    string defVal;

public:
    Column(){
        name = "";
        type = CT_UNKNOWN;
        size = 0;
        defVal = "";
    }

    Column(const string &n, ColType t, int s = 0, const string &dv = ""){
        name = n;
        type = t;
        size = s;
        defVal = dv;
    }

    Column(const Column &c){
        name = c.name;
        type = c.type;
        size = c.size;
        defVal = c.defVal;
    }

    Column& operator=(const Column &c){
        if(this != &c){
            name = c.name;
            type = c.type;
            size = c.size;
            defVal = c.defVal;
        }
        return *this;
    }

    string getName() const{
        return name;
    }

    ColType getType() const {
        return type;
    }

    int getSize() const {
        return size;
    }

    string getDefault() const {
        return defVal;
    }

    bool operator==(const Column &other) const{
        return equalIgnoreCase(name, other.name);
    }

    friend ostream & operator<<(ostream &out, const Column &c){
        out << c.name << ":";
        if(c.type == CT_TEXT) out << "TEXT(" << c.size << ")";
        if(c.type == CT_INTEGER) out << "INTEGER";
        if(c.type == CT_FLOAT) out << "FLOAT";
        if(!c.defVal.empty()) out << "Default Value = " << c.defVal;

        return out;
    }
};// end of Column class

class Table{
    string name;
    Column *columns;
    int colCount;
    int colCap;

public:
    static int nrTabele;

    Table(){
        name = "";
        columns = NULL;
        colCount = 0;
        colCap = 0;
        nrTabele++;
    }

    Table(const string &n){
        name = n;
        columns = NULL;
        colCount = 0;
        colCap = 0;
        nrTabele++;
    }

    Table(const Table &t){
        name = t.name;
        colCount = t.colCount;
        colCap = t.colCount;
        if(colCount > 0){
            columns = new Column[colCount];
            for(int i = 0; i < colCount; i++){
                columns[i] = t.columns[i];
            }
        }else
            columns = NULL;

        nrTabele++;
    }

    Table& operator=(const Table &t){
        if(this != &t){
            delete[] columns;
            name = t.name;
            colCount = t.colCount;
            colCap = t.colCap;
            if(colCount > 0){
                columns = new Column[colCap];
                for(int i = 0; i < colCount; i++){
                    columns[i] = t.columns[i];
                }
            }else
                columns = NULL;
        }

        return *this;
    }

    ~Table(){
        delete[] columns;
        nrTabele--;
    }

    string getName() const {
        return name;
    } 

    int getColCount() const {
        return colCount;
    }

    Column getColumn(int i) const{
        return columns[i];
    }

    void addColumn(const Column &c){
        if(colCount == colCap){
            int newCap = (colCap == 0 ? 4 : colCap * 2);
            Column *nc = new Column[newCap];

            delete[] columns;
            columns = nc;
            colCap = newCap;
        }
        columns[colCount] = c;
        colCount++;
    }

    friend ostream& operator<<(ostream &out, const Table &t){
        out << "TABLE " << t.name << "(" << t.colCount << " columns)\n";
        for(int i = 0; i < t.colCount; i++){
            out << " " << t.columns[i] << "\n";
        }
        return out;
    }
}; //end of class table

int Table::nrTabele = 0;

//we are using it for sqlcommands like create index and drop index
class Index{
    string name;
    string table;
    string column;

public:
    Index(){
        name = "";
        table = "";
        column = "";
    }

    Index(const string &n, const string &t, const string &c){
        name = n;
        table = t;
        column = c;
    }
}; //end of index class

// it holds all the tales
class Database{
    Table *tables;
    int tableCount;
    int tableCap;
    
public:
    Database(){
        tables = NULL;
        tableCount = 0;
        tableCap = 0;
    }

    ~Database(){
        delete[] tables;
    }

    int findTable(const string &name) const{
        for(int i = 0; i < tableCount; i++){
            if(equalIgnoreCase(tables[i].getName(), name)) 
                return i;
        }
        return -1;
    }

    bool createTable(const Table &t){
        if(findTable(t.getName()) >= 0)
            return false;
        
        if(tableCount == tableCap){
            int newCap = (tableCap == 0 ? 2 : tableCap * 2);
            Table *nt = new Table[newCap];

            for(int i = 0; i < tableCount; i++){
                nt[i] = tables[i];
            }

            delete[] tables;
            tables = nt;
            tableCap = newCap;
        }
        tables[tableCount] = t;
        tableCount++;

        return true;
    }

    Table* gatTable(const string &name){
        int index = findTable(name);

        if(index < 0)
            return NULL;

        return &tables[index];
    }
}; //end of database class

struct CommandResult{
    int code;
    string msg;

    CommandResult(){
        code = 0;
        msg = "";
    }
};

ColType parseColType(const string &s){
    string up = makeUpper(s);
    if(up == "TEXT")
        return CT_TEXT;
    
    if(up == "INTEGER")
        return CT_INTEGER;

    if(up == "FLOAT")
        return CT_FLOAT;
    
    return CT_UNKNOWN;
}

class CommandProcessor{
    Database *db;

public:
    CommandProcessor(Database *d){
        db = d;
    }

    SqlCommand detect(const string &line){
        TokenBuffer tb;
        tokenize(line, tb);

        if(tb.count == 0){
            return SqlCommand(line, CMD_UNKNOWN);
        }

        string keywrd1 = makeUpper(tb.items[0].text);

        if(keywrd1 == "CREATE"){
            if(tb.count > 1){
                string keywrd2 = makeUpper(tb.items[1].text);
                if(keywrd2 == "TABLE") {
                    return SqlCommand(line, CMD_CREATE_TABLE);
                }
            }
        }

        return SqlCommand(line, CMD_UNKNOWN);
    }

    CommandResult execute(const SqlCommand &cmd){
        if(cmd.getType() == CMD_CREATE_TABLE)
            return execCreateTable(cmd.getText());

        CommandResult r;
        r.code = 2;
        r.msg = "Error: unknown or not implemented yet.";
        return r;
    }

private:
    CommandResult parseError(const string &m){
        CommandResult r;
        r.code = 1;
        r.msg = "Err_Parse" + m;
        return r;
    }

    // we implement CREATE TABLE
    CommandResult execCreateTable(const string &line){
        TokenBuffer tb;
        tokenize(line, tb);
        int i = 0;

        if(!(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "CREATE")){
            return parseError("CREATE expected");
        }
        i++;

        if(!(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "TABLE")) {
            return parseError("TABLE expected");
        }
        i++;

        if(tb.items[i].type != TK_WORD){
            return parseError("Table name expected... ");
        }
        string tableName = tb.items[i].text;
        i++;

        bool ifNotExists = false;
        if(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "IF"){
            i++;
            if(!(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "NOT")){
                return parseError("NOT expected after IF");
            }
            i++;
            if(!(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "EXISTTS")){
                return parseError("EXISTS expected after IF NOT");
            }
            i++;
            ifNotExists = true;
        }

        if(db->findTable(tableName) >= 0){
            CommandResult r;
            if(ifNotExists){
                r.msg = "Table " + tableName + " alreaduexists (ignored)";
                return r;
            }else{
                r.code = 4;
                r.msg = "ERR: table " + tableName + " already exists";
                return r;
            }
        }

        if(tb.items[i].type != TK_LPAREN){
            return parseError(" '(' expected");
        }
        i++;

        Table t(tableName);
        bool hasColumn = false;

        while(tb.items[i].type != TK_RPAREN && tb.items[i].type != TK_END ){
            if(tb.items[i].type != TK_WORD){
                return parseError("Columne name expected");
            }
            string colName = tb.items[i].text;
            i++;

            if(tb.items[i].type != TK_WORD){
                return parseError("Column type expected");
            }

            ColType ct = parseColType(tb.items[i].text);
            if(ct == CT_UNKNOWN){
                return parseError("Invalid column type. Please use INTEGER, FLOAT OR TEXT");
            }
            i++;

            int colSize = 0;
            string defVal = "";

            if(ct == CT_TEXT && colSize <= 0){
                colSize = 250;
            }

            if(ct == CT_INTEGER && !defVal.empty() && !isIntLiteral(defVal)){
                return parseError("Default int invalid");
            }

            if(ct == CT_FLOAT && !defVal.empty() && !isFloatLiteral(defVal)){
                return parseError("Default float invalid");
            }

            Column c(colName, ct, colSize, defVal);
            t.addColumn(c);
            hasColumn = true;

            if(tb.items[i].type == TK_COMMA){
                i++;
            }else if(tb.items[i].type == TK_RPAREN){
                break;
            }else
                return parseError("',' or ')' expected after column.");
        }

        if(!hasColumn){
            return parseError("At least one column required.");
        }

        if(tb.items[i].type != TK_RPAREN){
            return parseError("')' expected.");
        }

        db->createTable(t);

        CommandResult r;
        r.msg = "Table " + tableName + " created succesfully.";
        return r;
    }
    
}; //end of CommandProcessor class


int main(){
    Database db;
    CommandProcessor proc(&db);

    string line;

    while (true) {
        if(!getline(cin, line))
            break;

        trimNewline(line);
        if(line.size() == 0)
            continue;

        SqlCommand cmd = proc.detect(line);
        CommandResult res = proc.execute(cmd);

        if(res.code == 0){
            cout << "OK: " << res.msg << endl;
        }else{
            cout << res.msg << endl;
        }
    }
    return 0;
}
