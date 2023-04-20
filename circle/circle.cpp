#include <iostream>
#include <conio.h>
#include <graphics.h>
#include <cmath>

//画8个相对称的点,xc,yc为圆心(圆的八方向对称性)
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
 * 中点画圆算法
 * 考虑圆的8方向对称性，只用画0~45度的1/8圆
 * 每次x = x + 1或y = y - 1(屏幕坐标系就是如此)
 * 判别式 d = F(M) = F(xp + 1,yp - 0.5) = (xp + 1)^2 + (yp - 0.5)^2 - R^2;
 * 第一个像素是(0,R)(只考虑1/8圆弧),则初始d = F(x + 1,R - 0.5) = 1.25 - R
*/
void MidPointCircle(int xc,int yc,int r,int Color){
	int x = 0,y = r;
	double d = 1.25 - r;
	CirclePoints(xc,yc,x,y,Color);
	while (x < y) {
		if(d < 0){
			d += x * 2.0 + 3;
		}else{
			d += (x - y) * 2.0 + 5;
			y--;
		}
		x++;
		CirclePoints(xc,yc,x,y,Color);
	}
}

//改进后的中点画圆算法，令h = d - 0.25,将浮点数运算转换为整数运算
void _MidPointCircle(int xc,int yc,int r,int Color){
	int x = 0,y = r,h = 1 - r;
	CirclePoints(xc,yc,x,y,Color);
	while (x < y) {
		if(h < 0){
			h += x * 2 + 3;
		}else{
			h += (x - y) * 2 + 5;
			y--;
		}
		x++;
		CirclePoints(xc,yc,x,y,Color);
	}
	
}

int main(){
    initgraph(640,480);
    // CirclePoints(200,200,0,50,59999);//为什么画出来的就4个点
    // CirclePoints(200,200,25,25,59999);
    _MidPointCircle(200,200,100,59999);
    getch();
	closegraph();
    return 0;
}