#include <iostream>
#include <conio.h>
#include <cmath>
#include <graphics.h>
#define MAX_SIZE 20

/**
 * 对每一条扫描线填充的过程可分为4个步骤
 * a:求交，计算扫描线与多边形各个边的交点(计算量的重点，所以需要通过算法进行改进)
 * b:排序，把所有交点按x坐标递增进行排序
 * c:配对(奇偶性，交点取舍(交点数应为偶数))解决方案:若共享顶点的两条边分别落在扫描线两边，就只算一个交点;若在扫描线同一边，算0个或2个(看这两条边另外两端点y值是大于(2)交点还是小于(0)交点)
 * d:区间填色
 * 
 * 构造新边表步骤
 * a:首先构造一个纵向链表，链表的长度为多边形所占有的最大扫描线数，链表的每一个结点称为一个吊桶，对应多边形覆盖的每一条扫描线
*/

//活性边表(AET)(针对一条固定的扫描线来说)
typedef struct AET{
    float ymax;//与该边所交的的最高扫描线的Y值(从ymin到ymax依次扫描填充)
    float x;//当前边与扫描线的交点的x值
    float dx;//边斜率的倒数 dx = 1/k(当前扫描线到下一条扫描线间x的增量)
    struct AET* next;
}AET;

//新边表(为了方便活性边表的建立与更新)(用来存放多边形边的信息)
typedef struct NET{
    int line_num;
}NET;

//多边形每个顶点(必须是封闭图形)
typedef struct{
    int x,y;
}Point;

//多边形每条边
typedef struct{
    int x1,y1,x2,y2;
}Edge;

//多边形结构体(顶点表示)
typedef struct{
    int num;
    const Point *points;
}Polygon;

//多边形结构体(边表示)
typedef struct{
    int line_num;
    const Edge *edges;
}Polygon_Edge;

//顶点表示法的多边形填充
void polyfill(Polygon polygon,int Color){

}

//边表示法的多边形填充
void polyfill(Polygon_Edge polygon,int Color){
    //扫描多边形各条线，初始化新边表
    for(int i = 0;i < polygon.line_num;i++){
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
    Point points[] = {{0,0},{1,1},{2,1},{0,0}};
    //C99结构体初始化语法
    Polygon polygon = {
        .num = sizeof(points) / sizeof(Point),
        .points = points
    };
    _getch();
    closegraph();
    return 0;
}