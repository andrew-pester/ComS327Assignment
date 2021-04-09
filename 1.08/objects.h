#ifndef OBJECT_H
# define OBJECT_H

#include <string>
#include "descriptions.h"
#include "dims.h"
#include "utils.h"

class object {
    public:
    
    std::string name;
    std::string desc;
    object_type_t type;
    uint32_t color;
    //why the order matters i don't know all i know is this works which is dumb
    dice dam;
    int32_t hit;
    int32_t dodge;
    int32_t def;
    int32_t weight;
    int32_t speed;
    int32_t attr;
    int32_t val;
    pair_t pos;
    char symbol;
    object_description &od;
    object(object_description &od); 
    ~object(){od.exists_x();}
};
void gen_object(dungeon *d);
void gen_objects(dungeon *d);
int16_t *object_get_pos(object *o);

#endif