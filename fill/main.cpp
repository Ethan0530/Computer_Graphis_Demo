#include <iostream>
#include <conio.h>
#include <cmath>
#include <graphics.h>

typedef struct Edge{
    float ymax;//扫描线所交边的最高Y值
    float x;//当期扫描线与边的交点
    float dx;//边斜率的倒数 dx = 1/k
    struct Edge* next;
}Edge;

int main(){
    initgraph(640,480);
    circle(200,200,100);
    _getch();
    closegraph();
    return 0;
}