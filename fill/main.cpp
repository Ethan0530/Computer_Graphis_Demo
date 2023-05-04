#include <iostream>
#include <conio.h>
#include <cmath>
#include <graphics.h>
#define MAX_SIZE 20

/**
 * 对每一条扫描线填充的过程可分为4个步骤
 * a:求交，计算扫描线与多边形各个边的交点
 * b:排序，把所有交点按x坐标递增进行排序
 * c:配对(奇偶性，交点取舍(交点数应为偶数))解决方案:若共享顶点的两条边分别落在扫描线两边，就只算一个交点;若在扫描线同一边，算0个或2个(看这两条边另外两端点y值是大于(2)交点还是小于(0)交点)
 * d:区间填色
*/

typedef struct Edge{
    float ymax;//扫描线所交边的最高Y值(从ymin到ymax依次扫描填充)
    float ymin;//扫描线所交边的最低Y值
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