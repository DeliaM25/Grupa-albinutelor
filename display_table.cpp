#include "mysqlite.h"

CommandResult CommandProcessor::execDisplayTable(const string &line){
    TokenBuffer tb;
    tokenize(line, tb);
    int i = 0;

    if(!(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "DISPLAY")){
        return parseError("DISPLAY expected");
    }
    i++;

    if(!(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "TABLE")){
        return parseError("TABLE expected");
    }
    i++;

    if(tb.items[i].type != TK_WORD){
        return parseError("Table name expected after DISPLAY TABLE");
    }
    string tableName = tb.items[i].text;
    i++;

    if(tb.items[i].type != TK_END){
        return parseError("Too many arguments for DISPLAY TABLE");
    }

    Table *t = db->getTable(tableName);
    CommandResult r;

    if (t == NULL) {
        r.code = 3;
        r.msg  = "Table " + tableName + " does not exist";
        return r;
    }

    cout << "Table: " << t->getName() << "\n";
    cout << "Columns: " << t->getColCount() << "\n";
    for (int j = 0; j < t->getColCount(); j++) {
        cout << "  Column " << (j+1) << ": " << t->getColumn(j) << "\n";
    }

    r.code = 0;
    r.msg  = "DISPLAY TABLE executed.";
    return r;
}