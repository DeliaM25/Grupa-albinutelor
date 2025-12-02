#include "mysqlite.h"

CommandResult CommandProcessor::execCreateTable(const string &line){
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
        if(!(tb.items[i].type == TK_WORD && makeUpper(tb.items[i].text) == "EXISTS")){
            return parseError("EXISTS expected after IF NOT");
        }
        i++;
        ifNotExists = true;
    }

    if(db->findTable(tableName) >= 0){
        CommandResult r;
        if(ifNotExists){
            r.msg = "Table " + tableName + " already exists (ignored)";
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
            return parseError("Column name expected");
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