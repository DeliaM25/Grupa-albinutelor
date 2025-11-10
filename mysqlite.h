#pragma once
#include <iostream>
#include <string.h>
#include <string>
#include <regex>

using namespace std;

void trimNewline (string &s);
string makeUpper(const string &s);
bool myIsSpace(char c);
bool myIsDigit(char c);
bool isIntLiteral(const string &s);
bool isFloatLiteral(const string &s);
int stringToInt(const string &s);
bool equalIgnoreCase(const string &a, const string &b);


enum TokenType{
    TK_WORD,
    TK_COMA,
    TK_LPAREN,
    TK_RPAREN,
    TK_EQUAL,
    TK_STAR,
    TK_STRING,
    TK_END
};

struct Token{
    TokenType type;
    string text;
};

const int MAX_TOKENS = 256;

struct TokenBuffer{
    Token items[MAX_TOKENS];
    int count;
    
    TokenBuffer();
    void add(TokenType t, const string &txt);
};

void tokenize(const string &line, TokenBuffer &buf);


enum CommandType{
    CMD_UNKNOWN,
    CMD_CREATE_TABLE
};

enum ColType{
    CT_TEXT,
    CT_INTEGER,
    CT_FLOAT,
    CT_UNKNOWN
};

class SqlCommand{
    string text;
    CommandType type;

public:
    static const int MAX_LEN = 511;
    
    SqlCommand();
    SqlCommand(const string &t, CommandType tp = CMD_UNKNOWN);
    SqlCommand(const SqlCommand &other);
    SqlCommand& operator=(const SqlCommand &other);

    friend ostream& operator<<(ostream &out, const SqlCommand &c);
};

class Column{
public:
    static const int MAX_NAME = 32;

private:
    string name;
    ColType type;
    int size;
    string defValue;

public:
    Column();
    Column(const string &n, ColType t, int s = 0, const string &dv = "");
    Column(const Column &c);
    Column& operator=(const Column &c);

    string getName() const;
    ColType getType() const;
    int getSize() const;
    string getDefault() const;

    bool operator==(const Column &other) const;

    friend ostream & operator<<(ostream &out, const Column &c);
};

class Table{
    string name;
    Column *columns;
    int colCount;
    int colCap;

public:
    static int nrTabele;

    Table();
    Table(const string &n);
    Table(const Table &t);
    Table& operator=(const Table &t);
    ~Table();

    string getName() const;
    int getColCount() const;
    Column getColumn(int i) const;

    void addColumn(const Column &c);

    friend ostream& operator<<(ostream &out, const Table &t);
};

class Index{
    string name;
    string table;
    string column;

public:
    Index();
    Index(const string &n, const string &t, const string &c);
};

class Database{
    Table *tables;
    int tableCount;
    int tableCap;

public:
    Database();
    ~Database();

    int findTable(const string &name) const;
    bool createTable(const Table &t);
    Table* getTable(const string &name);
};

struct CommandResult{
    int code;
    string msg;

    CommandResult();
};

ColType parseColType(const string &s);

class CommandProcessor{
    Database *db;

public:
    CommandProcessor(Database *d);

    SqlCommand detect(const string &line);
    CommandResult execute(const SqlCommand &cmd);

private:
    CommandResult parseError(const string &m);
    CommandResult execCreateTable(const string &line);
};