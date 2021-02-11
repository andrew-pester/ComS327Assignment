#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char dung[80][21];

int main(int argc, char *argv[]){
srand(time(NULL));

makeMap();
}
int makeMap()
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
        int temp = 1;
        fillArrayRoom(rooms[i].x, rooms[i].y, rooms[i].x+rooms[i].x_size, rooms[i].y+rooms[i].y_size);
	if (i == 5){
	  temp = 0;
	}
	fillArrayCorridor(rooms[i].x, rooms [i].y, room[temp].x, room[temp].y);
	temp ++;
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

void fillArrayCorridor(int rm1x, int rm1y, int rm2x, int rm2y)
{
  int tempx =0;
  int tempy =0;
  bool right;
  bool up;

  if(rm1y > rm2y){
    tempy = rm1y - rm2y;
    up = true;
  } if(rm1y < rm2y){
    tempy = rm2y - rm1y;
    up = false;
  } if(rm1y == rm2y){
    tempy = 0;
    up = true;
  }

  if(rm1x > rm2x){
    tempx = rm1x - rm2x;
    right = true;
  } if(rm1y < rm2y){
    tempx = rm2x - rm1x;
    right = false;
  } if(rm1y == rm2y){
    tempx = 0;
    right = true;
  }

  if (right){
    if (up){
      for(tempx; tempx > 0; tempx--){
	for (tempy; tempy > 0; tempy--){
	  if(dung[rm1x + tempx][rm1y + tempy] != ' '){
	  } else {
	    dung[rm1x + tempx][rm1y + tempy] = '#';
	  }
	}
  }
    } else {
      for(tempx; tempx > 0; tempx--){
	for (tempy; tempy > 0; tempy--){
	  if(dung[rm1x + tempx][rm1y - tempy] != ' '){
	  } else {
	    dung[rm1x + tempx][rm1y - tempy] = '#';
	  }
	}
      }
    }
  } else {
    if(up){
      for(tempx; tempx > 0; tempx--){
	for (tempy; tempy > 0; tempy--){
	  if(dung[rm1x - tempx][rm1y + tempy] != ' '){
	  } else {
	    dung[rm1x - tempx][rm1y + tempy] = '#';
	  }
	}
      }
      
    } else {
      for(tempx; tempx > 0; tempx--){
	for (tempy; tempy > 0; tempy--){
	  if(dung[rm1x - tempx][rm1y - tempy] != ' '){
	  } else {
	    dung[rm1x - tempx][rm1y - tempy] = '#';
	  }
	}
      }
    }
  }


  for(tempx; tempx > 0; tempx--){
    for (tempy; tempy > 0; tempy--){
      if(dung[rm1x - tempx][rm1y - tempy] != ' '){
      } else {
	dung[rm1x - tempx][rm1y - tempy] = '#';
      }
    }
  }
  
}
