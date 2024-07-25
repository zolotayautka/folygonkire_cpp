#include "folygonkire.h"

std::vector<tuple> dic_exec::search(std::string kotoba){
    std::ostringstream sql[4];
    sql[0] << "SELECT kotoba, imi, bikou, kanji, hinsi FROM dic WHERE kotoba LIKE '%" << kotoba << "%';";
    sql[1] << "SELECT kotoba, imi, bikou, kanji, hinsi FROM dic WHERE imi LIKE '%" << kotoba << "%';";
    sql[2] << "SELECT kotoba, imi, bikou, kanji, hinsi FROM dic WHERE kanji LIKE '%" << kotoba << "%';";
    sql[3] << "SELECT kotoba, imi, bikou, kanji, hinsi FROM dic WHERE bikou LIKE '%" << kotoba << "%';";
    for(int i = 0; i < 4; i++){
        sqlite3_exec(db, sql[i].str().c_str(), callback, &sagasu_list[i], nullptr);
    }
    std::set<tuple> tmp;
    for(int i = 0; i < 4; i++){
        tmp.insert(sagasu_list[i].begin(), sagasu_list[i].end());
    }
    sagasu_list[4].assign(tmp.begin(), tmp.end());
    return sagasu_list[4];
}

int* dic_exec::count_kotoba(){
    const char* sql[] = {
        "SELECT count(kotoba) FROM dic;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=0;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=1;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=2;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=3;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=4;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=5;"
    };
    for(int i = 0; i < 7; i++){
        sqlite3_exec(db, sql[i], callback_count, &kotoba_count[i], 0);
    }
    return kotoba_count;
}

std::vector<unsigned char> dic_exec::mp3_load(std::string kotoba){
    sqlite3_stmt* stmt;
    std::ostringstream sql;
    sql << "SELECT hatsuon FROM dic WHERE kotoba='" << kotoba << "';";
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    const unsigned char* mp3 = static_cast<const unsigned char*>(sqlite3_column_blob(stmt, 0));
    int size = sqlite3_column_bytes(stmt, 0);
    mp3_file.assign(mp3, mp3 + size);
    sqlite3_finalize(stmt);
    return mp3_file;
}

bool dic_exec::add_kotoba(tuple add_dic, std::vector<unsigned char> mp3){
    if(kaburu_check(add_dic.kotoba)){
        return false;
    }
    std::ostringstream sql;
    sql << "INSERT INTO dic VALUES ('" << add_dic.kotoba << "', " << add_dic.hinsi << ", '" << add_dic.imi << "', X'";
    int l = mp3.size();
    for (int i = 0; i < l; ++i) {
            sql << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(mp3[i]);
        }
    sql << "', '" << add_dic.bikou << "', '" << add_dic.kanji << "');";
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    return true;
}

bool dic_exec::add_kotoba(tuple add_dic){
    if(kaburu_check(add_dic.kotoba)){
        return false;
    }
    std::ostringstream sql;
    sql << "INSERT INTO dic VALUES ('" << add_dic.kotoba << "', " << add_dic.hinsi << ", '" << add_dic.imi << "', " << NULL << ", '" << add_dic.bikou << "', '" << add_dic.kanji << "');";
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    return true;
}

void dic_exec::modify_kotoba(tuple t, std::vector<unsigned char> mp3){
    std::ostringstream sql;
    sql << "UPDATE dic SET imi = '" << t.imi << "', bikou = '" << t.bikou << "', kanji = '" << t.kanji << "', hinsi = '" << t.hinsi << "', hatsuon = X'";
    int l = mp3.size();
    for (int i = 0; i < l; ++i) {
        sql << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(mp3[i]);
    }
    sql << "' WHERE kotoba='" << t.kotoba << "';";
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    const char sql_[] = "VACUUM;";
    sqlite3_exec(db, sql_, 0, 0, 0);
}

void dic_exec::modify_kotoba(tuple t, bool flag){
    std::ostringstream sql;
    if (flag){
        sql << "UPDATE dic SET imi = '" << t.imi << "', bikou = '" << t.bikou << "', kanji = '" << t.kanji << "', hinsi = '" << t.hinsi << "', hatsuon = " << NULL << " WHERE kotoba='" << t.kotoba << "';";
    } else {
        sql << "UPDATE dic SET imi = '" << t.imi << "', bikou = '" << t.bikou << "', kanji = '" << t.kanji << "', hinsi = '" << t.hinsi << "' WHERE kotoba='" << t.kotoba << "';";
    }
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    const char sql_[] = "VACUUM;";
    sqlite3_exec(db, sql_, 0, 0, 0);
}

void dic_exec::del_kotoba(std::string kotoba){
    std::ostringstream sql;
    sql << "DELETE FROM dic WHERE kotoba='" << kotoba << "';";
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    const char sql_[] = "VACUUM;";
    sqlite3_exec(db, sql_, 0, 0, 0);
}

bool dic_exec::kaburu_check(std::string kotoba){
    int t;
    std::ostringstream sql;
    sql << "SELECT COUNT(*) FROM dic WHERE kotoba='" << kotoba << "';";
    sqlite3_exec(db, sql.str().c_str(), callback_count, &t, 0);
    if (t == 0){
        return false;
    } else {
        return true;
    }
}

history::history(){
    std::string temp;
    std::ifstream rfile("history.txt");
    while (std::getline(rfile, temp)){
        history_list.push_back({temp});
    }
    rfile.close();
}

history::history(std::string kotoba, std::string imi){
    std::string temp;
    std::ifstream rfile("history.txt");
    while (std::getline(rfile, temp)){
        history_list.push_back({temp});
    }
    rfile.close();
    int t = history_list.size();
    if (t>0){
        std::string t1 = history_list.at(t-1);
        std::string t2 = kotoba + "  " + imi;
        if (!(t1.compare(t2))){
            return;
        }
    }
    std::ofstream ofile("history.txt", std::ios_base::app);
    ofile << kotoba << "  " << imi << "\n";
    ofile.close();
}

std::vector<std::string> history::read_history(){
    return history_list;
}

std::vector<tuple> bookmark::bookmark_load(){
    const char sql[] = "SELECT kotoba, imi, bikou, kanji, tb FROM bookmark;";
    sqlite3_exec(db, sql, callback, &bookmark_list, nullptr);
    return bookmark_list;
}

int bookmark::bookmark_count(){
    int t = bookmark_list.size();
    return t;
}

int bookmark::add_bookmark(tuple midasigo){
    if (kaburu_check(midasigo.kotoba)){
        return 0;
    }
    std::ostringstream sql;
    sql << "INSERT INTO bookmark VALUES ('" << midasigo.kotoba << "', " << std::to_string(midasigo.hinsi) << ", '" << midasigo.imi << "', '" << midasigo.bikou << "', '" << midasigo.kanji << "');";
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    return 1;
}

void bookmark::del_bookmark(std::string kotoba){
    std::ostringstream sql;
    sql << "DELETE FROM bookmark WHERE kotoba='" << kotoba << "';";
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    const char sql_[] = "VACUUM;";
    sqlite3_exec(db, sql_, 0, 0, 0);
}

bool bookmark::kaburu_check(std::string kotoba){
    int t;
    std::ostringstream sql;
    sql << "SELECT COUNT(*) FROM bookmark WHERE kotoba='" << kotoba << "';";
    sqlite3_exec(db, sql.str().c_str(), callback_count, &t, 0);
    if (t == 0){
        return false;
    } else {
        return true;
    }
}

void create_dic(){
    sqlite3* db;
    char sql1[] = "CREATE TABLE dic (\
                kotoba CHAR(20),\
                hinsi INT,\
                imi CHAR(80),\
                hatsuon BLOB,\
                bikou CHAR(50),\
                kanji CHAR(20),\
                PRIMARY KEY(kotoba)\
            );";
    char sql2[] = "CREATE TABLE bookmark (\
                kotoba CHAR(20),\
                tb CHAR(20),\
                imi CHAR(80),\
                bikou CHAR(50),\
                kanji CHAR(20),\
                PRIMARY KEY(kotoba)\
            );";
    char sql3[] = "CREATE TABLE flag (\
                lang TEXT\
            );";
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql1, 0, 0, 0);
    sqlite3_exec(db, sql2, 0, 0, 0);
    sqlite3_exec(db, sql3, 0, 0, 0);
    sqlite3_close(db);
}
