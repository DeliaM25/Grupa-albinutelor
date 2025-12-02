#include "mysqlite.h"

using namespace std;

int main(){
    Database db;
    CommandProcessor proc(&db);

    string line;

    cout << "MySQLite Phase 1" << endl;
    cout << "Write SQL commands (CREATE TABLE, DROP TABLE, DISPLAY TABLE) or .exit to end the task." << endl;

    while (true) {
        cout << "SQL> ";

        if(!getline(cin, line))
            break;

        trimNewline(line);
        if(line == ".exit")
            break;

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