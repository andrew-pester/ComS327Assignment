#include "objects.h"
#include "dungeon.h"
#include <cstring>

object::object(object_description &od) : name(od.get_name()), desc(od.get_description()),
                                                   type(od.get_type()), color(od.get_color()), dam(od.get_damage()), hit(od.get_hit().roll()),
                                                   dodge(od.get_dodge().roll()), def(od.get_defence().roll()), weight(od.get_weight().roll()),
                                                   speed(od.get_speed().roll()), attr(od.get_attribute().roll()), val(od.get_value().roll()), od(od)
{
    symbol = object_symbol[od.get_type()];
};

void gen_object(dungeon *d)
{
    object *o;
    std::vector<object_description> &od = d->object_descriptions;
    int tmp = 1;
    int i = 0;
    while (tmp)
    {
        //choose a random object index
        i = rand() % od.size();
        //check if it is able to spawn i.e. has to do with artifact
        if (check_if_eligable_o(&od.at(i)))
        {
            tmp = 0;
        }
        else if (od.at(i).rarity >= (unsigned)rand() % 100)
        {
            tmp = 0;
        }
    }
    od.at(i).exists_();
    pair_t p;
    uint32_t room;
    do
    {
        room = rand_range(1, d->num_rooms - 1);
        p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                              (d->rooms[room].position[dim_y] +
                               d->rooms[room].size[dim_y] - 1));
        p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                              (d->rooms[room].position[dim_x] +
                               d->rooms[room].size[dim_x] - 1));
    } while (d->character_map[p[dim_y]][p[dim_x]]);
    o = new object(od.at(i));
    o->pos[dim_x] = p[dim_x];
    o->pos[dim_y] = p[dim_y];
    d->object_map[p[dim_y]][p[dim_x]] = o;
    
}
void gen_objects(dungeon *d)
{
    
    int i;
    for(i =0; i<12;i++){
        gen_object(d);
    }
    // got symbols working
    // for(i = 0; i<DUNGEON_Y;i++){
    //     for(int j = 0; j<DUNGEON_X;j++){
    //         if(d->object_map[i][j]){
    //             printf("%c",d->object_map[i][j]->symbol);
    //         }
    //     }
    // }
}
int16_t *object_get_pos(object *o)
{
    return o->pos;
}