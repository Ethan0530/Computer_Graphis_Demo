#include <iostream>
#include <conio.h>
#include <cmath>
#include <graphics.h>
#define MAX_SIZE 20

typedef struct Edge{
    float ymax;//扫描线所交边的最高Y值
    float x;//当期扫描线与边的交点
    float dx;//边斜率的倒数 dx = 1/k
    struct Edge* next;
}Edge;

typedef struct Polygon{
    int line_num;
    int x1[MAX_SIZE];
    int y1[MAX_SIZE];
    int x2[MAX_SIZE];
    int y2[MAX_SIZE];
}Polygon;

void polyfill(Polygon polygon,int Color){

}

int main(){
    initgraph(640,480);
    circle(200,200,100);
    _getch();
    closegraph();
    return 0;
}