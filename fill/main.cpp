#include <iostream>
#include <conio.h>
#include <cmath>
#include <graphics.h>
#define MAX_SIZE 20

typedef struct Edge{
    float ymax;//扫描线所交边的最高Y值
    float x;//当前扫描线与边的交点的x值
    float dx;//边斜率的倒数 dx = 1/k
    struct Edge* next;
}Edge;

//多边形每条边
typedef struct{
    int x1,y1,x2,y2;
}PolygonEdge;

//多边形结构体
typedef struct{
    int line_nums;
    PolygonEdge polygonEdge[MAX_SIZE];
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