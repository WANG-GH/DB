#include <stdio.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<cstring>
#include <cassert>

#include "db/mydb.h"

using namespace std;

void TestMyDB() {
    cout << "/*²âÊÔPutºÍGet*/" << endl;
    MyDB *db;
    MyDB::Open("Database", &db);
    string key = "key";
    string val = "val----------------------------------------------------";
    for (int i = 0; i < 200000; ++i) {
        key = "key";
        db->Put(key + to_string(i), val + to_string(i));
    }
    for (int i = 0; i < 200000; ++i) {
        string value = "";
        db->Get(key + to_string(i), &value);
        cout << value << endl;
    }
    delete db;


    cout << "/*²âÊÔCallBackIndex*/" << endl;
    MyDB::Open("Database", &db);
    for (int i = 0; i < 200000; ++i) {
        string value = "";
        db->Get(key + to_string(i), &value);
        cout << value << endl;
    }
    delete db;

}

int main() {
    TestMyDB();
    return 0;

}

