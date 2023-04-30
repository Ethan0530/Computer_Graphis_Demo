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
    //扫描多边形各条线，初始化新边表
    for(int i = 0;i < polygon.line_nums;i++){
        //初始化新边表头指针NET[i]
        //把ymin = i的边放进新编表NET[i]
    }
    //y = 最低扫描线号
    //初始化活性边表AET为空

    for(int i = 0;;i++){

    }
    //活性边表增量更新
}

int main(){
    initgraph(640,480);
    circle(200,200,100);
    _getch();
    closegraph();
    return 0;
}