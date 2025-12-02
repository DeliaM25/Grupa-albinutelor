#include "mysqlite.h"

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


TokenBuffer::TokenBuffer(){
    count = 0;
}

void TokenBuffer::add(TokenType t, const string &txt){
    if(count >= MAX_TOKENS)
        return;
    items[count].type = t;
    items[count].text = txt;
    count++;
}

void tokenize(const string &line, TokenBuffer &buf){
    buf.count = 0;
    int i = 0;
    int n = (int)line.size();

    while(i < n){
        char c = line[i];

        if(myIsSpace(c)){ 
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
}

SqlCommand::SqlCommand(){
    text = "";
    type = CMD_UNKNOWN;
}

SqlCommand::SqlCommand(const string &t, CommandType tp){
    text = t;
    type = tp;
}

SqlCommand::SqlCommand(const SqlCommand &other){
    text = other.text;
    type = other.type;
}

SqlCommand& SqlCommand::operator=(const SqlCommand &other){
    if(this != &other){
        text = other.text;
        type = other.type;
    }
    return *this;
}

string SqlCommand::getText() const{
    return text;
}

CommandType SqlCommand::getType() const{
    return type;
}

void SqlCommand::setType(CommandType t){
    type = t;
}

ostream& operator<<(ostream &out, const SqlCommand &c){
    out << "[CMD type = " << c.type << "]" << c.text;
    return out;
}

Column::Column(){
    name = "";
    type = CT_UNKNOWN;
    size = 0;
    defVal = "";
}

Column::Column(const string &n, ColType t, int s, const string &dv){
    name = n;
    type = t;
    size = s;
    defVal = dv;
}

Column::Column(const Column &c){
    name = c.name;
    type = c.type;
    size = c.size;
    defVal = c.defVal;
}

Column& Column::operator=(const Column &c){
    if(this != &c){
        name = c.name;
        type = c.type;
        size = c.size;
        defVal = c.defVal;
    }
    return *this;
}

string Column::getName() const{
    return name;
}

ColType Column::getType() const{
    return type;
}

int Column::getSize() const{
    return size;
}

string Column::getDefault() const{
    return defVal;
}

bool Column::operator==(const Column &other) const{
    return equalIgnoreCase(name, other.name);
}

ostream & operator<<(ostream &out, const Column &c){
    out << c.name << ":";
    if(c.type == CT_TEXT) out << "TEXT(" << c.size << ")";
    if(c.type == CT_INTEGER) out << "INTEGER";
    if(c.type == CT_FLOAT) out << "FLOAT";
    if(!c.defVal.empty()) out << " Default Value = " << c.defVal;

    return out;
}

int Table::nrTabele = 0;

Table::Table(){
    name = "";
    columns = NULL;
    colCount = 0;
    colCap = 0;
    nrTabele++;
}

Table::Table(const string &n){
    name = n;
    columns = NULL;
    colCount = 0;
    colCap = 0;
    nrTabele++;
}

Table::Table(const Table &t){
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

Table& Table::operator=(const Table &t){
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

Table::~Table(){
    delete[] columns;
    nrTabele--;
}

string Table::getName() const{
    return name;
}

int Table::getColCount() const{
    return colCount;
}

Column Table::getColumn(int i) const{
    return columns[i];
}

void Table::addColumn(const Column &c){
    if(colCount == colCap){
        int newCap = (colCap == 0 ? 4 : colCap * 2);
        Column *nc = new Column[newCap];

        for(int i = 0; i < colCount; i++){
            nc[i] = columns[i];
        }

        delete[] columns;
        columns = nc;
        colCap = newCap;
    }
    columns[colCount] = c;
    colCount++;
}

ostream& operator<<(ostream &out, const Table &t){
    out << "TABLE " << t.name << "(" << t.colCount << " columns)\n";
    for(int i = 0; i < t.colCount; i++){
        out << " " << t.columns[i] << "\n";
    }
    return out;
}

Index::Index(){
    name = "";
    table = "";
    column = "";
}

Index::Index(const string &n, const string &t, const string &c){
    name = n;
    table = t;
    column = c;
}

Database::Database(){
    tables = NULL;
    tableCount = 0;
    tableCap = 0;
}

Database::~Database(){
    delete[] tables;
}

int Database::findTable(const string &name) const{
    for(int i = 0; i < tableCount; i++){
        if(equalIgnoreCase(tables[i].getName(), name))
            return i;
    }
    return -1;
}

bool Database::createTable(const Table &t){
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

bool Database::dropTable(const string &name){
    int index = findTable(name);
    if(index < 0){
        return false;
    }

    for(int i = index; i < tableCount - 1; i++){
        tables[i] = tables[i + 1];
    }

    tableCount --;

    return true;
}

Table* Database::getTable(const string &name){
    int index = findTable(name);

    if(index < 0)
        return NULL;

    return &tables[index];
}

CommandResult::CommandResult(){
    code = 0;
    msg = "";
}

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

CommandProcessor::CommandProcessor(Database *d){
    db = d;
}

SqlCommand CommandProcessor::detect(const string &line){
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

    if(keywrd1 == "DROP"){
        if(tb.count > 1){
            string keywrd2 = makeUpper(tb.items[1].text);
            if(keywrd2 == "TABLE"){
                return SqlCommand(line, CMD_DROP_TABLE);
            }
        }
    }

    if(keywrd1 == "DISPLAY"){
        if(tb.count > 1){
            string keywrd2 = makeUpper(tb.items[1].text);
            if(keywrd2 == "TABLE"){
                return SqlCommand(line, CMD_DISPLAY_TABLE);
            }
        }
    }

    return SqlCommand(line, CMD_UNKNOWN);
}

CommandResult CommandProcessor::execute(const SqlCommand &cmd){
    if(cmd.getType() == CMD_CREATE_TABLE)
        return execCreateTable(cmd.getText());

    if(cmd.getType() == CMD_DROP_TABLE)
        return execDropTable(cmd.getText());

    if(cmd.getType() == CMD_DISPLAY_TABLE)
        return execDisplayTable(cmd.getText());

    CommandResult r;
    r.code = 2;
    r.msg = "Error: unknown or not implemented yet.";
    return r;
}

CommandResult CommandProcessor::parseError(const string &m){
    CommandResult r;
    r.code = 1;
    r.msg = "Err_Parse " + m;
    return r;
}