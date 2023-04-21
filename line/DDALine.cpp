#include <iostream>
#include <conio.h>
#include <cmath>
#include <graphics.h>

//数字微分分析器算法
void DDALine(int x1,int y1,int x2,int y2,int Color){
    int i,length;
    float dx,dy,x = x1,y = y1;
    if(abs(x2 - x1) > abs(y2 - y1)){
        length = abs(x2 - x1);
    }else{
        length = abs(y2 - y1);
    }
    dx = (float)(x2 - x1)/length;
    dy = (float)(y2 - y1)/length;
    putpixel((int)(x + 0.5),(int)(y + 0.5),Color);
    for(int i = 1;i <= length;i++){
        x += dx;
        y += dy;
        putpixel((int)(x + 0.5),(int)(y + 0.5),Color);
    }
}

int main(){
    initgraph(640,480);
    DDALine(100,100,200,200,59999);
    DDALine(100,400,200,100,59999);
    _getch();
    closegraph();
    return 0;
}