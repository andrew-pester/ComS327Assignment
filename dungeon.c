#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char dung[80][21];

int main(int argc, char *argv[]){
srand(time(NULL));

makeRooms();
}
int makeRooms()
{
    //return 1 if true 0 if false for checking intersections
    struct room{
        int x;//x coord in array
        int y;//y coord in array
        int x_size;//random x size
        int y_size;//random y size
    };
    struct room rooms[6];
    for(int i =0;i<6;i++){
        rooms[i].x = rand()%75 +1;
        rooms[i].y = rand()%17 +1;
        rooms[i].x_size = rand()%10 +4;
        rooms[i].y_size = rand()%10 +3;

    }
    //need to test more
    for(int i = 0; i<6;i++){
        for(int j = i+1; j<6; j++){
            if (rooms[i].x  >= rooms[j].x || rooms[i].x+rooms[i].x_size >= rooms[j].x+rooms[j].x_size){
                return 0;
            } 
  
            if (rooms[i].y <= rooms[j].y || rooms[i].y +rooms[i].y_size <= rooms[j].y+rooms[j].y_size){
                return 0;
            }
        }
    }
    for (int i = 0; i < 6; i++)
    {
        fillArrayRoom(rooms[i].x, rooms[i].y, rooms[i].x+rooms[i].x_size, rooms[i].y+rooms[i].y_size);
    }
    //put rooms in array here
    return 1; 

}
void fillArrayRoom(int topX, int topY,int botX,int botY){
    for(topX; topX<=botX;topX++){
        for(topY; topY<=botY;topY++){
            dung[topX][topY] = '.';
        }
    }
}