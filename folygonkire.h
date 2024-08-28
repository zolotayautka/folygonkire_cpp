#ifndef FOLYGONKIRE_H
#define FOLYGONKIRE_H

#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <tuple>
#include <fstream>
#include <iomanip>
#include "sqlite3.h"

struct tuple {
    std::string kotoba;
    std::string imi;
    std::string bikou;
    std::string kanji;
    int hinsi;

    bool operator<(const tuple& other) const {
        return std::tie(kotoba, imi, bikou, kanji, hinsi) < std::tie(other.kotoba, other.imi, other.bikou, other.kanji, other.hinsi);
    }
};

class dic_exec{
    private:
    sqlite3* db;
    std::vector<tuple> sagasu_list[5];
    int kotoba_count[7];
    std::vector<unsigned char> mp3_file;
    static int callback(void *data, int argc, char **argv, char **azColName) {
        std::vector<tuple>* sagasu_list = static_cast<std::vector<tuple>*>(data);
        sagasu_list->push_back({argv[0], argv[1], argv[2], argv[3], std::stoi(argv[4])});
        return 0;
    }
    static int callback_count(void *data, int argc, char **argv, char **azColName) {
        int* count = static_cast<int*>(data);
        *count = std::stoi(argv[0]);
        return 0;
    }
    bool kaburu_check(std::string kotoba);
    public:
    dic_exec(){
        sqlite3_open("dic.db", &db);
    }
    ~dic_exec(){
        sqlite3_close(db);
    }
    std::vector<tuple> search(std::string kotoba);
    int* count_kotoba();
    std::vector<unsigned char> mp3_load(std::string kotoba);
    bool add_kotoba(tuple add_dic, std::vector<unsigned char> mp3);
    bool add_kotoba(tuple add_dic);
    void modify_kotoba(tuple t, std::vector<unsigned char> mp3);
    void modify_kotoba(tuple t, bool flag);
    void del_kotoba(std::string kotoba);
};

class history {
    private:
    std::vector<std::string> history_list;
    public:
    history();
    history(std::string kotoba, std::string imi);
    std::vector<std::string> read_history();
};

class bookmark {
    private:
    sqlite3* db;
    std::vector<tuple> bookmark_list;
    static int callback(void *data, int argc, char **argv, char **azColName) {
        std::vector<tuple>* bookmark_list = static_cast<std::vector<tuple>*>(data);
        bookmark_list->push_back({argv[0], argv[1], argv[2], argv[3], std::stoi(argv[4])});
        return 0;
    }
    static int callback_count(void *data, int argc, char **argv, char **azColName) {
        int* count = static_cast<int*>(data);
        *count = std::stoi(argv[0]);
        return 0;
    }
    public:
    bookmark(){
        sqlite3_open("dic.db", &db);
    }
    ~bookmark(){
        sqlite3_close(db);
    }
    std::vector<tuple> bookmark_load();
    bool add_bookmark(tuple midasigo);
    void del_bookmark(std::string kotoba);
    bool kaburu_check(std::string kotoba);
};

void create_dic();

#endif // FOLYGONKIRE_H
