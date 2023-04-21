#include <iostream>
#include <conio.h>
#include <graphics.h>

void CirclePoints(int xc,int yc,int x,int y,int Color){
	//目标x2 + x = 2^xc,则x2 = 2 *xc - x
    putpixel(x,y,Color);
    putpixel(x,2 * yc - y,Color);
    putpixel(2 * xc - x,y,Color);
    putpixel(2 * xc - x,2 * yc - y,Color);
}

//正负法画圆
//可以画出1/4圆弧,需要修改CirclePoints函数画出完整的圆
void PNMCircle(int xc,int yc,int r,int Color){
    int x = xc,y = yc + r;
    int f = 0;
    while (y > yc) {
        CirclePoints(xc,yc,x,y,Color);
        if(f > 0){
            f += 2 * (yc - y) + 1;
            y--;
        }else{
            f += 2 * (x - xc) + 1;
            x++;
        }
        if(y == yc) CirclePoints(xc,yc,x,y,Color);
    }
}

int main(){
    initgraph(640,480);
    PNMCircle(200,200,100,59999);
    _getch();
    closegraph();
    return 0;
}