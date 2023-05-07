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

typedef struct Node{
    int val;
    Node* next;
}Node,*List;

//活性边表(AET)(针对一条固定的扫描线来说)
typedef struct AET{
    int ymax;//与该边所交的的最高扫描线的Y值(从ymin到ymax依次扫描填充)
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
}Polygon_Point;

//多边形结构体(边表示)
typedef struct{
    int line_num;
    const Edge *edges;
}Polygon_Edge;

//展示数据函数
void displayPolygon(const Polygon_Point& polygon) {
    std::cout << "Polygon with " << polygon.point_num << " points:\n";
    for (int i = 0; i < polygon.point_num; i++) {
        const Point& point = polygon.points[i];
        std::cout << "(" << point.x << "," << point.y << ")";
        if (i != polygon.point_num - 1) {
            std::cout << " -> ";
        } else {
            std::cout << " -> (" << polygon.points[0].x << "," << polygon.points[0].y << ")";
        }
    }
    std::cout << "\n";
}

//展示数据函数
void displayAET(const AET* aet) {
    std::cout << "Active Edge Table (AET):\n";
    const AET* edge = aet;
    while (edge != NULL) {
        std::cout << " ymax: " << edge->ymax
                  << " x: " << edge->x
                  << " dx: " << edge->dx
                  << "\n";
        edge = edge->next;
    }
}

//展示数据函数
void displayNET(const AET** net, int ymin, int ymax) {
    std::cout << "New Edge Table (NET):\n";
    for (int y = ymin; y <= ymax; y++) {
        std::cout << " Scanline " << y << ": ";
        const AET* edge = net[y - ymin];
        while (edge != NULL) {
            std::cout << " ymax: " << edge->ymax
                      << " x: " << edge->x
                      << " dx: " << edge->dx
                      << " ->";
            edge = edge->next;
        }
        std::cout << " NULL\n";
    }
}

//展示数据函数
void displayList(List list){
    Node* p = list->next;
    while(p != NULL){
        std::cout<<p->val<<" -> ";
        p = p->next;
    }
    std::cout<<std::endl;
}

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
void polyfill(Polygon_Point polygon,int Color){
    //将多边形转换为边表示法
    Polygon_Edge polygon_Edge = convert_to_edge_representation(polygon.points,polygon.point_num);
    int ymax = -1,ymin = 10000;
    for(int i = 0;i < polygon.point_num;i++){
        // putpixel(polygon.points[i].x,polygon.points[i].y,Color);
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
        float dx = (float)(edge->p2.x - edge->p1.x)/(float)(edge->p2.y - edge->p1.y); //计算增量
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
            }else{
                prev->next = node;
                node->next = curr;
            }
        }
    }

    // //test
    // displayNET(const_cast<const AET**>(NET), ymin, ymax);

    //初始化活性边表，增加一个空的头结点
    AET header = {
        .ymax = -1,
        .x = -1,
        .dx = -1,
        .next = NULL,
    };
    //配对交点，扫描填色，增量更新
    for(int i = ymin;i <= ymax;i++){
        AET *node1 = NET[i - ymin];
        AET *node2 = header.next;
        AET *temp = &header;
        while(node1 != NULL && node2 != NULL){//合并这两个有序链表
            if(node1->x < node2->x){
                temp->next = node1;
                node1 = node1->next;
            }else{
                temp->next = node2;
                node2 = node2->next;
            }
            temp = temp->next;
        }
        temp->next = (node1 != NULL) ? node1 : node2;

        //删除ymax = i的结点
        temp = header.next;
        AET *prev = &header;
        while(temp != NULL){
            if(temp->ymax == i){
                prev->next = temp->next;
            }else{
                prev = temp;
            }
            temp = temp->next;
        }

        // //test
        // if(i >= 600){
        //     std::cout<<"i = "<<i<<std::endl;
        //     displayAET(header.next);
        // }

        //遍历AET表，配对交点区间
        temp = header.next;
        List list = (List)malloc(sizeof(Node));
        Node* p = list;
        while(temp != NULL){
            int sign = 1,count = 0;//sign判断是否进行取舍端点,count统计另一端点y值在上方的数目
            for(int j = 0;j < polygon_Edge.line_num;j++){
                if(polygon_Edge.edges[j].p1.x == temp->x && polygon_Edge.edges[j].p1.y == i){
                    sign = 0;//需要进行取舍
                    if(polygon_Edge.edges[j].p2.y > i){
                        count++;
                    }
                }else if(polygon_Edge.edges[j].p2.x == temp->x && polygon_Edge.edges[j].p2.y == i){
                    sign = 0;
                    if(polygon_Edge.edges[j].p1.y > i){
                        count++;
                    }
                }
            }
            if(sign == 1){
                Node *s = (Node*)malloc(sizeof(Node));
                s->val = temp->x;
                s->next = NULL;
                p->next = s;
                p = p->next;
                temp = temp->next;
                continue;
            }else{//取舍
                if(count == 0){//全落在下方，舍
                    temp = temp->next;
                    continue;
                }else if(count == 1){//一边一个，正常取
                    Node *s = (Node*)malloc(sizeof(Node));
                    s->val = temp->x;
                    s->next = NULL;
                    p->next = s;
                    p = p->next;
                    temp = temp->next;
                    continue;
                }else{//全在上方，取2个
                    Node *s1 = (Node*)malloc(sizeof(Node));
                    Node *s2 = (Node*)malloc(sizeof(Node));
                    s1->val = temp->x;
                    s2->val = temp->x;
                    s1->next = s2;
                    s2->next = NULL;
                    p->next = s1;
                    p = p->next->next;
                    temp = temp->next;
                    continue;
                }
            }
        }

        // //test
        // displayList(list);

        //已配对好交点，下面填色
        p = list->next;
        while(p != NULL && p->next != NULL){
            for(int n1 = p->val,n2 = p->next->val;n1 <= n2;n1++){
                putpixel(n1,i,Color);
            }
            p = p->next->next;
        }

        //把ymax > i的结点的x值进行增量更新
        temp = header.next;
        // prev = &header;
        while(temp != NULL){     
            temp->x += temp->dx; 
            temp = temp->next;
        }
        
        //将活性边表重新排序
        temp = header.next;
        prev = &header;
        if(temp != NULL){
            AET *list = temp->next;
            temp->next = NULL;//先把链表断开,避免成环
            while(list != NULL){
                temp = header.next;
                prev = &header;
                while(temp != NULL && temp->x < list->x){
                    prev = temp;
                    temp = temp->next;
                }
                AET *save = list->next;
                prev->next = list;
                list->next = temp;
                list = save; 
            }
        }
        // //test
        // displayAET(header.next);
    }
}

void draw(){
    Point points[] = {{364,1080},{378,1041},{431,1029},{426,1080}};
    Polygon_Point polygon = {
        .point_num = sizeof(points) / sizeof(Point),//计算顶点数量
        .points = points,//设置顶点数组
    };
    polyfill(polygon,RGB(48,67,45));
    Point points2[] = {{663,1080},{725,1080},{711,1042},{658,1029}};
    Polygon_Point polygon2 = {
        .point_num = sizeof(points2) / sizeof(Point),//计算顶点数量
        .points = points2,//设置顶点数组
    };
    polyfill(polygon2,RGB(48,67,45));
    Point points3[] = {{430,1080},{436,1027},{545,1011},{653,1027},{660,1080}};
    Polygon_Point polygon3 = {
        .point_num = sizeof(points3) / sizeof(Point),//计算顶点数量
        .points = points3,//设置顶点数组
    };
    polyfill(polygon3,RGB(255,251,242));

    setcolor(RGB(99,85,74));
    //裙边
    line(426,1080,431,1028);
    line(427,1080,432,1028);
    line(428,1080,433,1028);
    line(429,1080,434,1028);
    line(662,1080,657,1028);
    line(661,1080,656,1028);
    line(660,1080,655,1028);
    line(659,1080,654,1028);

    //手臂
    line(549,1000,787,1055);
    line(549,999,787,1054);
    line(549,998,787,1053);
    line(549,997,787,1052);
    line(549,996,787,1051);

    line(549,1000,301,1055);
    line(549,999,301,1054);
    line(549,998,301,1053);
    line(549,997,301,1052);
    line(549,996,301,1051);

    line(301,1055,258,1030);
    line(301,1054,259,1030);
    line(301,1053,260,1030);
    line(301,1052,261,1030);
    line(301,1051,262,1030);

    line(258,1030,249,992);
    line(259,1030,250,992);
    line(260,1030,251,992);
    line(261,1030,252,992);
    line(262,1030,253,992);

    line(249,992,248,970);
    line(250,992,249,970);
    line(251,992,250,970);
    line(252,992,251,970);
    line(253,992,252,970);

    line(248,970,251,938);
    line(249,970,252,938);
    line(250,970,253,938);
    line(251,970,254,938);
    line(252,970,255,938);

    //衣袖与手臂边缘
    line(229,928,267,943);
    line(229,927,267,942);
    line(229,926,267,941);
    line(228,925,267,940);
    line(228,924,267,940);

    line(279,940,281,847);
    line(280,940,282,846);
    line(281,940,283,846);
    line(282,940,283,846);
    line(283,940,284,847);

    line(228,926,257,827);
    line(229,926,257,828);
    line(229,926,258,828);
    line(230,926,258,829);
    line(231,926,259,829);

    line(231,913,210,889);
    line(231,912,211,889);
    line(231,911,212,889);
    line(231,910,213,889);
    line(231,909,214,889);

    line(210,888,249,821);
    line(210,888,250,821);
    line(211,888,249,821);
    line(211,888,249,820);
    line(212,888,249,821);
    line(212,888,250,822);
    line(213,888,250,823);
    line(213,888,251,823);

    line(267,943,283,943);
    line(267,942,283,942);
    line(267,941,283,941);
    line(267,940,283,940);
    line(267,939,283,939);

    line(280,943,286,950);
    line(281,943,286,949);
    line(282,943,286,948);
    line(283,943,286,947);
    line(284,943,286,946);

    line(286,950,295,950);
    line(286,949,295,949);
    line(286,948,295,948);
    line(286,947,295,947);
    line(286,946,295,946);

    line(295,950,345,920);
    line(295,949,345,919);
    line(295,948,345,918);
    line(295,947,345,917);
    line(295,946,345,916);

    line(345,920,354,920);
    line(345,919,354,919);
    line(345,918,354,918);
    line(345,917,354,917);

    line(354,920,363,927);
    line(354,919,364,927);
    line(354,918,365,927);
    line(354,917,366,927);

    line(346,935,549,889);
    line(346,934,549,888);
    line(346,933,549,887);
    line(346,932,549,886);

    //上半部分衣袖
    line(323,834,316,834);
    line(323,833,316,833);
    line(323,832,316,832);
    line(323,831,316,831);

    line(316,834,287,826);
    line(316,833,287,825);
    line(316,832,287,824);
    line(316,831,287,823);

    line(287,826,266,814);
    line(287,825,266,813);
    line(287,824,266,812);
    line(287,823,266,811);

    line(266,815,232,784);
    line(266,814,232,783);
    line(266,813,232,782);
    line(266,812,232,781);
    line(266,811,232,780);

    line(232,784,222,772);
    line(232,783,222,771);
    line(232,782,222,770);
    line(232,781,222,769);
    line(232,780,222,768);
    line(232,780,222,767);
    line(232,780,222,766);

    line(222,770,219,763);
    line(222,769,219,763);
    line(222,769,220,763);
    line(222,768,221,763);
    line(222,767,222,763);
    line(222,765,222,763);
    line(222,764,222,763);

    line(218,763,218,741);
    line(219,763,219,741);
    line(220,763,220,741);
    line(221,763,221,741);

    line(218,741,225,721);
    line(219,741,226,721);
    line(220,741,227,721);
    line(221,741,228,721);
    line(222,741,229,721);

    line(225,721,328,575);
    line(226,721,328,576);
    line(227,721,328,577);
    line(228,721,328,578);
    line(229,721,328,579);
    
    line(328,579,347,561);
    line(328,578,347,560);
    line(328,577,347,559);
    line(328,576,347,558);

    line(347,561,357,557);
    line(347,560,357,556);
    line(347,559,357,555);
    line(347,558,357,554);

    line(357,557,365,557);
    line(357,556,365,556);
    line(357,555,365,555);
    line(357,554,365,554);

    line(365,557,390,561);
    line(365,556,390,560);
    line(365,555,390,559);
    line(365,554,390,558);

    line(390,561,447,544);
    line(390,560,447,543);
    line(390,559,447,542);
    line(390,558,447,541);
    line(390,558,447,540);

    //衣领
    line(446,544,446,529);
    line(447,544,447,529);
    line(448,544,448,529);
    line(449,544,449,529);

    line(446,529,484,485);
    line(447,529,485,485);
    line(448,529,486,485);
    line(449,529,487,485);
    line(449,529,488,485);

    line(486,488,505,486);
    line(486,487,505,485);
    line(486,486,505,484);
    line(486,485,505,483);

    line(446,544,476,577);
    line(446,543,476,576);
    line(446,542,476,575);
    line(446,541,476,574);

    line(476,577,534,505);
    line(476,576,533,505);
    line(476,575,532,505);
    line(476,574,531,505);

    line(534,505,482,491);
    line(534,504,482,490);
    line(534,503,482,489);
    line(534,502,482,488);

    line(534,505,544,531);
    line(534,504,544,532);
    line(534,503,544,533);
    line(534,502,544,534);



    line(372,695,336,775);
    line(373,695,337,775);
    line(374,695,338,775);
    line(375,695,339,775);

    line(336,775,324,822);
    line(337,775,325,822);
    line(338,775,326,822);
    line(339,775,327,822);

    line(324,822,325,847);
    line(325,822,326,847);
    line(326,822,327,847);
    line(327,822,328,847);

    line(325,847,337,883);
    line(326,847,338,883);
    line(327,847,339,883);
    line(328,847,340,883);

    line(337,883,378,925);
    line(338,883,379,925);
    line(339,883,380,925);
    line(340,883,381,925);
    
    //袖子上的装饰环
    Point points4[] = {{306,832},{324,833},{324,847},{305,846},{248,818},{248,812},{253,807},{255,807}};
    Polygon_Point polygon4 = {
        .point_num = sizeof(points4) / sizeof(Point),//计算顶点数量
        .points = points4,//设置顶点数组
    };
    polyfill(polygon4,RGB(136,227,134));
    //


}

/**
 * 这是一个对称图形，对称轴为x = 540
 * 则(x1 + x2) / 2.0 = 540
 * x2 = 1080 - x1 
*/

void draw2(){

}

int main(){
    initgraph(1080,1080);
    setbkcolor(RGB(255,239,206));
    cleardevice();
    // circle(200,200,100);
    // Point points[] = {{200,200},{500,100},{900,300},{1100,300},{1100,800},{700,600},{500,700},{400,600},{200,650}};
    //星星
    // Point points1[] = {{88,173},{88,186},{94,186},{94,178},{109,178},{109,171},{124,171},{124,164},{133,164},{133,171},{148,171},{148,178},{163,178},{163,186},{169,186},{169,174},{161,174},{161,158},{154,158},{154,141},{161,141},{161,134},{169,134},{169,127},{176,127},{176,121},{146,121},{146,113},{139,113},{139,98},{131,98},{131,83},{126,83},{126,98},{118,98},{118,113},{110,113},{110,120},{80,120},{80,126},{88,126},{88,133},{95,133},{95,141},{102,141},{102,158},{95,158},{95,173}};
    // Polygon_Point polygon1 = {
    //     .point_num = sizeof(points1) / sizeof(Point),//计算顶点数量
    //     .points = points1,//设置顶点数组
    // };
    // polyfill(polygon1,RED);

    //三角形
    Point points[] = {{200,200},{200,300},{250,250}};
    // polygon((POINT*)points,3);

    // Point points[] = {{}};
    //C99结构体初始化语法
    Polygon_Point polygon = {
        .point_num = sizeof(points) / sizeof(Point),//计算顶点数量
        .points = points,//设置顶点数组
    };
    // displayPolygon(polygon);
    // polyfill(polygon,0xFF55FF);
    // polyfill(polygon,RGB(250,212,223));

    draw();


    // _getch();

    // 将绘制的图形保存为.jpg格式图像
    IMAGE img;
    getimage(&img, 0, 0, 1080, 1080);  // 获取整个屏幕区域的图像
    saveimage("E:\\git_workplace\\Computer_Graphis_Demo\\test.jpg",&img);

    closegraph();
    return 0;
}