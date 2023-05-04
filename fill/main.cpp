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

//新边表(NET)(为了方便活性边表的建立与更新)(用来存放多边形边的信息)

typedef struct{
    int x,y;
}Point;

typedef struct{
    Point p1,p2;
}Edge;

//多边形结构体(顶点表示)(points数组需要按顺序相连构成封闭图形(最后一个点要和第一个点连接))
typedef struct{
    int point_num;
    const Point *points;
}Polygon;

//多边形结构体(边表示)
typedef struct{
    int line_num;
    const Edge *edges;
}Polygon_Edge;

//将多边形从点表示法转换为边表示法
Polygon_Edge convert_to_edge_representation(const Point *points, int point_num){
    Edge *edges = (Edge*)malloc(point_num*sizeof(Edge));
    for(int i = 0;i < point_num - 1;i++){
        edges[i].p1 = points[i];
        edges[i].p2 = points[i + 1];
    }
    edges[point_num - 1].p1 = points[point_num - 1];
    edges[point_num - 1].p2 = points[0];
    Polygon_Edge polygon = {
        .line_num = point_num,
        .edges = edges,
    };
    return polygon;
}

//顶点表示法的多边形填充
void polyfill(Polygon polygon,int Color){
    //将多边形转换为边表示法
    Polygon_Edge polygon_Edge = convert_to_edge_representation(polygon.points,polygon.point_num);
    int ymax = -1,ymin = 10000;
    for(int i = 0;i < polygon.point_num;i++){
        int temp = polygon.points[i].y;
        if(temp < ymin) ymin = temp;
        if(temp > ymax) ymax = temp;
    }
    //拿到了ymax和yminx才能建立新边表NET
    AET **NET = new AET*[ymax - ymin + 1];
    for(int i = 0;i < ymax - ymin + 1;i++){
        NET[i] = NULL;
    }
    //扫描多边形各条线，初始化新边表NET
    for(int i = 0;i < polygon_Edge.line_num;i++){
        const Edge* edge = &(polygon_Edge.edges[i]);
        int yTop = fmax(edge->p1.y,edge->p2.y);
        int yBottom = fmin(edge->p1.y,edge->p2.y);
        if(yTop == yBottom) continue;//忽略水平边
        float x = (yBottom == edge->p1.y)? edge->p1.x : edge->p2.x;//计算与yBottom相交的x值
        float dx = (edge->p2.x - edge->p1.x)/(edge->p2.y - edge->p1.y); //计算增量
        AET* node = new AET{yTop,x,dx,NULL};
        int index = yBottom - ymin;
        if(NET[index] == NULL){//如果该位置为空，直接插入
            NET[index] = node;
        }else{ //否则按照x值从小到大排序插入(对有序链表进行插入操作)
            AET* curr = NET[index];
            AET* prev = NULL;
            while(curr != NULL && curr->x < x){
                prev = curr;
                curr = curr->next;
            }
            if(prev == NULL){
                node->next = curr;
                NET[index] = node;
            } else {
                prev->next = node;
                node->next = curr;
            }
        }
    }
    //扫描填充，增量更新
    for(int i = ymin;i <= ymax;i++){
        //初始化活性边表
        AET* active = NET[i - ymin];
        //按x递增顺序排列
        while(active != NULL && active->next != NULL){
            if(active->x > active->next->x){
                std::swap(active->x, active->next->x);
            }
            active = active->next;
        }
        AET* curr = NET[i - ymin];
        AET* prev = NULL;
        while(curr != NULL){
            if(curr->ymax == i){//删除ymax = i的边
                if(prev != NULL){
                    prev->next = curr->next;
                }else{
                    NET[i - ymin] = curr->next;
                }
                curr = curr->next;
                continue;
            }
            prev = curr;
            curr->x += curr->dx;//更新dx值
            curr = curr->next;
        }
        //按x递增顺序重新排列
        while(active != NULL && active->next != NULL){
            if(active->x > active->next->x){
                std::swap(active->x, active->next->x);
            }
            active = active->next;
        }
    }
    //释放内存
    for(int i = 0; i < ymax - ymin + 1; i++){
        AET* curr = NET[i];
        while(curr != NULL){
            AET* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }
    delete[] NET;
}

//边表示法的多边形填充
void polyfill(Polygon_Edge polygon,int Color){
    // find_min_max(polygon.edges);
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
    Point points[] = {{0,0},{1,1},{2,1}};
    //C99结构体初始化语法
    Polygon polygon = {
        .point_num = sizeof(points) / sizeof(Point),//计算顶点数量
        .points = points,//设置顶点数组
    };
    _getch();
    closegraph();
    return 0;
}