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

std::vector<tuple> search(std::string kotoba);
int* count_kotoba();
std::vector<unsigned char> mp3_load(std::string kotoba);
bool add_kotoba_(tuple add_dic, std::vector<unsigned char> mp3);
bool add_kotoba_(tuple add_dic);
void modify_kotoba_(tuple t, std::vector<unsigned char> mp3);
void modify_kotoba_(tuple t, bool flag);
void del_kotoba(std::string kotoba);

void write_history(std::string kotoba, std::string imi);
std::vector<std::string> read_history();

std::vector<tuple> bookmark_load();
bool add_bookmark(tuple midasigo);
void del_bookmark(std::string kotoba);
bool kaburu_check_book(std::string kotoba);

void create_dic(std::string lang);

bool koutyakugo();
std::string load_lang();

#endif // FOLYGONKIRE_H
