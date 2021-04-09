#include "objects.h"

object::object(object_description &od, pair_t p) : name(od.get_name()), desc(od.get_description()),
    type(od.get_type()), color(od.get_color()), dam(od.get_damage()), hit(od.get_hit().roll()),
    dodge(od.get_dodge().roll()), def(od.get_defence().roll()), weight(od.get_weight().roll()),
    speed(od.get_speed().roll()), attr(od.get_attribute().roll()), val(od.get_value().roll()), od(od){
        pos[dim_x] = p[dim_y];
        pos[dim_x] = p[dim_x];
        od.exists_();
    };

void gen_object(dungeon *d){
    
}
void gen_objects(dungeon *d){
    
}