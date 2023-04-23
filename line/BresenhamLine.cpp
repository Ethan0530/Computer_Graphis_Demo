#include <iostream>
#include <conio.h>
#include <cmath>
#include <graphics.h>

int Sign(int x){
    if(x == 0) return 0;
    else if(x < 0) return -1;
    else return 1;
}

//Bresenham划线法原理：根据直线的斜率，在x或y方向每次都只递增一个像素单位，另一个方向增量为0或1
void BresenhamLine(int x1,int y1,int x2,int y2,int Color){
    int x,y,dx,dy,dk,s1,s2,temp,swap;
    dx = abs(x2 - x1);
    dy = abs(y2 - y1);
    x = x1;
    y = y1;
    s1 = Sign(x2 - x1);
    s2 = Sign(y2 - y1);
    if(dy > dx){
        temp = dx;
        dx = dy;
        dy = temp;
        swap = 1;
    }else swap = 0;
    dk = 2 * dy - dx;
    for(int i = 1;i <= dx;i++){
        putpixel(x,y,Color);
        if(swap == 1) y += s2;
        else x += s1;
        dk += 2 * dy;
        if(dk >= 0){
            if(swap == 1) x += s1;
            else y += s2;
            dk -= 2 * dx;
        }
    }
}

int main(){
    initgraph(640,480);
    BresenhamLine(100,100,200,200,59999);
    BresenhamLine(100,400,200,100,59999);
    _getch();
    closegraph();
    return 0;
}