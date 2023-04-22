#include <iostream>
#include <conio.h>
#include <cmath>
#include <graphics.h>

int Sign(int x){
    if(x == 0) return 0;
    else if(x < 0) return -1;
    else return 1;
}

void MidPointLine(int x1,int y1,int x2,int y2,int Color){
    int x = x1,y = y1,a,b,d,d1,d2,s1,s2,temp,swap;
    a = -abs(y2 - y1);
    b = abs(x2 - x1);
    s1 = Sign(x2 - x1);
    s2 = Sign(y2 - y1);
    if(-a > b){
        temp = b;
        b = -a;
        a = -temp;
        swap = 1;
    }else swap = 0;
    d = 2 * a + b;
    d1 = 2 * a;
    d2 = 2 * (a + b);
    putpixel(x,y,Color);
    for(int i = 1;i <= b;i++){
        if(swap == 1) y += s2;
        else x += s1;
        if(d < 0){
            if(swap == 1) x += s1;
            else y += s2;
            d += d2;
        }else d += d1;
        putpixel(x,y,Color);
    }
}

int main(){
    initgraph(640,480);
    MidPointLine(100,100,200,200,59999);
    MidPointLine(100,400,200,100,59999);
    _getch();
    closegraph();
    return 0;
}