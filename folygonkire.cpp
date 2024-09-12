#include "folygonkire.h"

static int callback(void *data, int argc, char **argv, char **azColName) {
    std::vector<tuple>* sagasu_list = static_cast<std::vector<tuple>*>(data);
    sagasu_list->push_back({argv[0], argv[1], argv[2], argv[3], std::stoi(argv[4])});
    return 0;
}

std::vector<tuple> search(std::string kotoba){
    std::vector<tuple> sagasu_list[5];
    std::ostringstream sql[4];
    sql[0] << "SELECT kotoba, imi, bikou, kanji, hinsi FROM dic WHERE kotoba LIKE '%" << kotoba << "%';";
    sql[1] << "SELECT kotoba, imi, bikou, kanji, hinsi FROM dic WHERE imi LIKE '%" << kotoba << "%';";
    sql[2] << "SELECT kotoba, imi, bikou, kanji, hinsi FROM dic WHERE kanji LIKE '%" << kotoba << "%';";
    sql[3] << "SELECT kotoba, imi, bikou, kanji, hinsi FROM dic WHERE bikou LIKE '%" << kotoba << "%';";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    for(int i = 0; i < 4; i++){
        sqlite3_exec(db, sql[i].str().c_str(), callback, &sagasu_list[i], nullptr);
    }
    sqlite3_close(db);
    std::set<tuple> tmp;
    for(int i = 0; i < 4; i++){
        tmp.insert(sagasu_list[i].begin(), sagasu_list[i].end());
    }
    sagasu_list[4].assign(tmp.begin(), tmp.end());
    return sagasu_list[4];
}

static int callback_count(void *data, int argc, char **argv, char **azColName) {
    int* count = static_cast<int*>(data);
    *count = std::stoi(argv[0]);
    return 0;
}

int* count_kotoba(){
    static int kotoba_count[7];
    const char* sql[] = {
        "SELECT count(kotoba) FROM dic;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=0;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=1;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=2;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=3;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=4;",
        "SELECT count(kotoba) FROM dic WHERE hinsi=5;"
    };
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    for(int i = 0; i < 7; i++){
        sqlite3_exec(db, sql[i], callback_count, &kotoba_count[i], 0);
    }
    sqlite3_close(db);
    return kotoba_count;
}

std::vector<unsigned char> mp3_load(std::string kotoba){
    std::vector<unsigned char> mp3_file;
    sqlite3_stmt* stmt;
    std::ostringstream sql;
    sql << "SELECT hatsuon FROM dic WHERE kotoba='" << kotoba << "';";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    const unsigned char* mp3 = static_cast<const unsigned char*>(sqlite3_column_blob(stmt, 0));
    int size = sqlite3_column_bytes(stmt, 0);
    mp3_file.assign(mp3, mp3 + size);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return mp3_file;
}

bool kaburu_check(std::string kotoba){
    int t;
    std::ostringstream sql;
    sql << "SELECT COUNT(*) FROM dic WHERE kotoba='" << kotoba << "';";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql.str().c_str(), callback_count, &t, 0);
    sqlite3_close(db);
    if (t == 0){
        return false;
    } else {
        return true;
    }
}

bool add_kotoba(tuple add_dic, std::vector<unsigned char> mp3){
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
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    sqlite3_close(db);
    return true;
}

bool add_kotoba(tuple add_dic){
    if(kaburu_check(add_dic.kotoba)){
        return false;
    }
    std::ostringstream sql;
    sql << "INSERT INTO dic VALUES ('" << add_dic.kotoba << "', " << add_dic.hinsi << ", '" << add_dic.imi << "', " << NULL << ", '" << add_dic.bikou << "', '" << add_dic.kanji << "');";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    sqlite3_close(db);
    return true;
}

void modify_kotoba(tuple t, std::vector<unsigned char> mp3){
    std::ostringstream sql;
    sql << "UPDATE dic SET imi = '" << t.imi << "', bikou = '" << t.bikou << "', kanji = '" << t.kanji << "', hatsuon = X'";
    int l = mp3.size();
    for (int i = 0; i < l; ++i) {
        sql << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(mp3[i]);
    }
    sql << "' WHERE kotoba='" << t.kotoba << "';";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    const char sql_[] = "VACUUM;";
    sqlite3_exec(db, sql_, 0, 0, 0);
    sqlite3_close(db);
}

void modify_kotoba(tuple t, bool flag){
    std::ostringstream sql;
    if (flag){
        sql << "UPDATE dic SET imi = '" << t.imi << "', bikou = '" << t.bikou << "', kanji = '" << t.kanji << "', hatsuon = " << NULL << " WHERE kotoba='" << t.kotoba << "';";
    } else {
        sql << "UPDATE dic SET imi = '" << t.imi << "', bikou = '" << t.bikou << "', kanji = '" << t.kanji << "' WHERE kotoba='" << t.kotoba << "';";
    }
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    const char sql_[] = "VACUUM;";
    sqlite3_exec(db, sql_, 0, 0, 0);
    sqlite3_close(db);
}

void del_kotoba(std::string kotoba){
    std::ostringstream sql;
    sql << "DELETE FROM dic WHERE kotoba='" << kotoba << "';";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    const char sql_[] = "VACUUM;";
    sqlite3_exec(db, sql_, 0, 0, 0);
    sqlite3_close(db);
}

std::vector<std::string> read_history(){
    std::vector<std::string> history_list;
    std::string temp;
    std::ifstream rfile("history.txt");
    while (std::getline(rfile, temp)){
        history_list.push_back({temp});
    }
    rfile.close();
    return history_list;
}

void write_history(std::string kotoba, std::string imi){
    std::vector<std::string> history_list = read_history();
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

std::vector<tuple> bookmark_load(){
    std::vector<tuple> bookmark_list;
    const char sql[] = "SELECT kotoba, imi, bikou, kanji, hinsi FROM bookmark;";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql, callback, &bookmark_list, nullptr);
    sqlite3_close(db);
    return bookmark_list;
}

bool add_bookmark(tuple midasigo){
    if (kaburu_check_book(midasigo.kotoba)){
        return true;
    }
    std::ostringstream sql;
    sql << "INSERT INTO bookmark VALUES ('" << midasigo.kotoba << "', " << midasigo.hinsi << ", '" << midasigo.imi << "', '" << midasigo.bikou << "', '" << midasigo.kanji << "');";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    sqlite3_close(db);
    return false;
}

void del_bookmark(std::string kotoba){
    std::ostringstream sql;
    sql << "DELETE FROM bookmark WHERE kotoba='" << kotoba << "';";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql.str().c_str(), 0, 0, 0);
    const char sql_[] = "VACUUM;";
    sqlite3_exec(db, sql_, 0, 0, 0);
    sqlite3_close(db);
}

bool kaburu_check_book(std::string kotoba){
    int t;
    std::ostringstream sql;
    sql << "SELECT COUNT(*) FROM bookmark WHERE kotoba='" << kotoba << "';";
    sqlite3* db;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql.str().c_str(), callback_count, &t, 0);
    sqlite3_close(db);
    if (t == 0){
        return false;
    } else {
        return true;
    }
}

void create_dic(std::string lang){
    sqlite3* db;
    const char sql1[] = "CREATE TABLE dic (\
                kotoba CHAR(20),\
                hinsi INT,\
                imi CHAR(80),\
                hatsuon BLOB,\
                bikou CHAR(50),\
                kanji CHAR(20),\
                PRIMARY KEY(kotoba)\
            );";
    const char sql2[] = "CREATE TABLE bookmark (\
                kotoba CHAR(20),\
                hinsi INT,\
                imi CHAR(80),\
                bikou CHAR(50),\
                kanji CHAR(20),\
                PRIMARY KEY(kotoba)\
            );";
    const char sql3[] = "CREATE TABLE flag (\
                lang TEXT\
            );";
    std::ostringstream sql4;
    sql4 << "INSERT INTO flag VALUES ('" << lang << "');";
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql1, 0, 0, 0);
    sqlite3_exec(db, sql2, 0, 0, 0);
    sqlite3_exec(db, sql3, 0, 0, 0);
    sqlite3_exec(db, sql4.str().c_str(), 0, 0, 0);
    sqlite3_close(db);
}

static int lang_callback(void *data, int argc, char **argv, char **azColName) {
    std::string* lang = static_cast<std::string*>(data);
    *lang = argv[0];
    return 0;
}

bool koutyakugo(){
    if ((!load_lang().compare("ja")) || (!load_lang().compare("ko"))){
        return true;
    } else {
        return false;
    }
}

std::string load_lang(){
    sqlite3* db;
    const char sql[] = "SELECT lang FROM flag;";
    std::string lang;
    sqlite3_open("dic.db", &db);
    sqlite3_exec(db, sql, lang_callback, &lang, 0);
    sqlite3_close(db);
    return lang;
}
