#include "mysqlite.h"

CommandResult CommandProcessor::execDropTable(const string &line){
    TokenBuffer tb;
    tokenize(line, tb);
    int i = 0;

    if(!(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "DROP")){
        return parseError("DROP expected");
    }
    i++;

    if(!(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "TABLE")){
        return parseError("TABLE expected");
    }
    i++;

    if(tb.items[i].type != TK_WORD){
        return parseError("Table name expected after DROP TABLE");
    }
    string tableName = tb.items[i].text;
    i++;

    bool ok = db->dropTable(tableName);

    if(!ok){
        CommandResult r;
        r.code = 3;
        r.msg = "Table " + tableName + " does not exist";
        return r;
    }

    CommandResult r;
    r.code = 0;
    r.msg = "Table " + tableName + " dropped succesfully.";
    return r;
}