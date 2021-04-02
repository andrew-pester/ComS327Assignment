#ifndef DESC_H
# define DESC_H


#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "dungeon.h"

using namespace std;
class dice{
public:
int base;
int number;
int sides;
};

class desc
{
public:
    string name;
    char symb;
    string color;
    string description;
    dice speed;
    dice hp;
    dice dam;
    string abil;
    int rrty;
};
static vector<string> pos_colors = {"RED","GREEN","BLUE", "CYAN", "YELLOW", "MAGENTA", "WHITE","BLACK"};
static vector<string> pos_abil = {"SMART", "TELE", "TUNNEL","ERRATIC", "PASS","PICKUP","DESTROY","UNIQ","BOSS"};

int parse_monster_desc();
int parse_name(ifstream &f, string s, desc &d);
int parse_syb(ifstream &f, string s, desc &d);
int parse_color(ifstream &f, string s, desc &d);
int parse_desc(ifstream &f, string s, desc &d);
int parse_speed(ifstream &f, string s, desc &d);
int parse_dam(ifstream &f, string s, desc &d);
int parse_hp(ifstream &f, string s, desc &d);
int parse_abil(ifstream &f, string s, desc &d);
int parse_rrty(ifstream &f, string s, desc &d);
int parse_monster(ifstream &f, string s);
void print_desc(desc &d);

#endif