#include <iostream>
#include <conio.h>
#include <graphics.h>

void CirclePoints(int xc,int yc,int x,int y,int Color){
	putpixel(xc + x,yc + y,Color);
	putpixel(xc + x,yc - y,Color);
	putpixel(xc - x,yc + y,Color);
	putpixel(xc - x,yc - y,Color);
	putpixel(xc + y,yc + x,Color);
	putpixel(xc + y,yc - x,Color);
	putpixel(xc - y,yc + x,Color);
	putpixel(xc - y,yc - x,Color);
}

/**
 * x每次+1,y = y || y = y - 1
 * 选择的原则是考察精确值是靠近y还是y - 1
*/
void BresenhamCircle(int xc,int yc,int r,int Color){
    int x = 0,y = r,d = 3 - 2 * r;
    while (x < y) {
        CirclePoints(xc,yc,x,y,Color);
        if(d >= 0){
            d += 4 * (x - y) + 10;
            y--;
        }else{
            d += 4 * x + 6;
        }
        x++;
    }
    if(x == y) CirclePoints(xc,yc,x,y,Color);
    
}

int main(){
    initgraph(640,480);
    BresenhamCircle(200,200,100,59999);
    getch();
    closegraph();
    return 0;
}