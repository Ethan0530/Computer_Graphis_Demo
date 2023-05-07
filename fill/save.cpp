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

void draw_Left(){
    setcolor(RGB(99,85,74));
    //裙边
    line(426,1080,431,1025);
    line(427,1080,432,1025);
    line(428,1080,433,1025);
    line(429,1080,434,1025);

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

    //竖直方向的多条手臂线段
    line(301,1055,294,1055);
    line(301,1054,294,1054);
    line(301,1053,294,1053);
    line(301,1052,294,1052);
    line(301,1051,294,1051);

    line(294,1055,275,1048);
    line(294,1054,275,1047);
    line(294,1053,275,1046);
    line(294,1052,275,1045);
    line(294,1051,275,1044);

    line(275,1048,258,1030);
    line(275,1047,259,1030);
    line(275,1046,260,1030);
    line(275,1045,261,1030);
    line(275,1044,262,1030);

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

    line(346,935,540,889);
    line(346,934,540,888);
    line(346,933,540,887);
    line(346,932,540,886);

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

    line(534,505,540,531);
    line(534,504,540,532);
    line(534,503,540,533);
    line(534,502,540,534);



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
    
    //胸前绿色装饰带
    setcolor(RGB(10,54,5));
    line(454,555,399,796);
    line(455,555,400,795);
    line(456,555,401,795);
    line(457,555,402,795);

    line(399,796,396,818);
    line(400,796,397,818);
    line(401,796,398,818);
    line(402,796,399,818);

    line(396,818,396,839);
    line(397,818,397,839);
    line(398,818,398,839);
    line(399,818,399,839);

    line(396,839,401,870);
    line(397,839,402,870);
    line(398,839,403,870);
    line(399,839,404,870);

    line(401,870,424,914);
    line(402,870,425,914);
    line(403,870,426,914);
    line(404,870,427,914);

    line(471,573,420,796);
    line(472,573,421,796);
    line(473,573,422,796);
    line(474,573,423,796);

    line(420,796,416,816);
    line(421,796,417,816);
    line(422,796,418,816);
    line(423,796,419,816);

    line(416,816,416,840);
    line(417,816,417,840);
    line(418,816,418,840);
    line(419,816,419,840);

    line(416,840,421,870);
    line(417,840,422,870);
    line(418,840,423,870);
    line(419,840,424,870);

    line(421,870,440,910);
    line(422,870,441,910);
    line(423,870,442,910);
    line(424,870,443,910);

    //扣排
    setcolor(RGB(99,85,74));
    line(525,577,525,890);
    line(526,577,526,890);
    line(527,577,527,890);
    line(528,577,528,890);
}

/**
 * 这是一个对称图形，对称轴为x = 540
 * 则(x1 + x2) / 2.0 = 540
 * x2 = 1080 - x1 
*/

void draw_Right(){
    setcolor(RGB(99,85,74));
    //裙边
    line(654,1080,649,1025);
    line(653,1080,648,1025);
    line(652,1080,647,1025);
    line(651,1080,646,1025);

    //手臂
    line(531,1000,293,1055);
    line(531,999,293,1054);
    line(531,998,293,1053);
    line(531,997,293,1052);
    line(531,996,293,1051);

    line(531,1000,779,1055);
    line(531,999,779,1054);
    line(531,998,779,1053);
    line(531,997,779,1052);
    line(531,996,779,1051);

    line(779,1055,786,1055);
    line(779,1054,786,1054);
    line(779,1053,786,1053);
    line(779,1052,786,1052);
    line(779,1051,786,1051);

    line(786,1055,805,1048);
    line(786,1054,805,1047);
    line(786,1053,805,1046);
    line(786,1052,805,1045);
    line(786,1051,805,1044);

    line(805,1048,822,1030);
    line(805,1047,821,1030);
    line(805,1046,820,1030);
    line(805,1045,819,1030);
    line(805,1044,818,1030);

    line(822,1030,831,992);
    line(821,1030,830,992);
    line(820,1030,829,992);
    line(819,1030,828,992);
    line(818,1030,827,992);

    line(831,992,832,970);
    line(830,992,831,970);
    line(829,992,830,970);
    line(828,992,829,970);
    line(827,992,828,970);

    line(832,970,829,938);
    line(831,970,828,938);
    line(830,970,827,938);
    line(829,970,826,938);
    line(828,970,825,938);

    //衣袖与手臂边缘
    line(851,928,813,942);
    line(851,927,813,942);
    line(851,926,813,941);
    line(852,925,813,940);
    line(852,924,813,940);

    line(801,940,799,847);
    line(800,940,798,846);
    line(799,940,797,846);
    line(798,940,797,846);
    line(797,940,796,847);

    line(852,926,823,827);
    line(851,926,823,828);
    line(850,926,822,828);
    line(850,926,822,829);
    line(849,926,821,829);

    line(849,913,870,889);
    line(849,912,869,889);
    line(849,911,868,889);
    line(849,910,867,889);
    line(849,909,866,889);

    line(870,888,831,821);
    line(870,888,830,821);
    line(869,888,831,821);
    line(869,888,831,820);
    line(868,888,831,821);
    line(868,888,830,822);
    line(867,888,830,823);
    line(867,888,829,823);

    line(813,943,797,943);
    line(813,942,797,942);
    line(813,941,797,941);
    line(813,940,797,940);
    line(813,939,797,939);

    line(800,943,794,950);
    line(799,943,794,949);
    line(798,943,794,948);
    line(797,943,794,947);
    line(796,943,794,946);

    line(794,950,785,950);
    line(794,949,785,949);
    line(794,948,785,948);
    line(794,947,785,947);
    line(794,946,785,946);

    line(785,950,735,920);
    line(785,949,735,919);
    line(785,948,735,918);
    line(785,947,735,917);
    line(785,946,735,916);

    line(735,920,726,920);
    line(735,919,726,919);
    line(735,918,726,918);
    line(735,917,726,917);

    line(726,920,717,927);
    line(726,919,716,927);
    line(726,918,715,927);
    line(726,917,714,927);

    line(734,935,540,889);
    line(734,934,540,888);
    line(734,933,540,887);
    line(734,932,540,886);

    //上半部分衣袖
    line(757,834,764,834);
    line(757,833,764,833);
    line(757,832,764,832);
    line(757,831,764,831);

    line(764,834,793,826);
    line(764,833,793,825);
    line(764,832,793,824);
    line(764,831,793,823);

    line(793,826,814,814);
    line(793,825,814,813);
    line(793,824,814,812);
    line(793,823,814,811);

    line(814,815,848,784);
    line(814,814,848,783);
    line(814,813,848,782);
    line(814,812,848,781);
    line(814,811,848,780);

    line(848,784,858,772);
    line(848,783,858,771);
    line(848,782,858,770);
    line(848,781,858,769);
    line(848,780,858,768);
    line(848,780,858,767);
    line(848,780,858,766);

    line(858,770,861,763);
    line(858,769,861,763);
    line(858,769,860,763);
    line(858,768,859,763);
    line(858,767,858,763);
    line(858,765,858,763);
    line(858,764,858,763);

    line(862,763,862,741);
    line(861,763,861,741);
    line(860,763,860,741);
    line(859,763,859,741);

    line(862,741,855,721);
    line(861,741,854,721);
    line(860,741,853,721);
    line(859,741,852,721);
    line(858,741,851,721);

    line(855,721,752,575);
    line(854,721,752,576);
    line(853,721,752,577);
    line(852,721,752,578);
    line(851,721,752,579);
    
    line(752,579,733,561);
    line(752,578,733,560);
    line(752,577,733,559);
    line(752,576,733,558);

    line(733,561,723,557);
    line(733,560,723,556);
    line(733,559,723,555);
    line(733,558,723,554);

    line(723,557,715,557);
    line(723,556,715,556);
    line(723,555,715,555);
    line(723,554,715,554);

    line(715,557,690,561);
    line(715,556,690,560);
    line(715,555,690,559);
    line(715,554,690,558);

    line(690,561,633,544);
    line(690,560,633,543);
    line(690,559,633,542);
    line(690,558,633,541);
    line(690,558,633,540);

    //衣领
    line(633,544,634,529);
    line(633,544,633,529);
    line(632,544,632,529);
    line(631,544,631,529);

    line(634,529,596,485);
    line(633,529,595,485);
    line(632,529,594,485);
    line(631,529,593,485);
    line(631,529,592,485);

    line(594,488,575,486);
    line(594,487,575,485);
    line(594,486,575,484);
    line(594,485,575,483);

    line(634,544,604,577);
    line(634,543,604,576);
    line(634,542,604,575);
    line(634,541,604,574);

    line(604,577,546,505);
    line(604,576,547,505);
    line(604,575,548,505);
    line(604,574,549,505);

    line(546,505,598,491);
    line(546,504,598,490);
    line(546,503,598,489);
    line(546,502,598,488);

    line(546,505,540,531);
    line(546,504,540,532);
    line(546,503,540,533);
    line(546,502,540,534);



    line(708,695,744,775);
    line(707,695,743,775);
    line(706,695,742,775);
    line(705,695,741,775);

    line(744,775,756,822);
    line(743,775,755,822);
    line(742,775,754,822);
    line(741,775,753,822);

    line(756,822,755,847);
    line(755,822,754,847);
    line(754,822,753,847);
    line(753,822,752,847);

    line(755,847,743,883);
    line(754,847,742,883);
    line(753,847,741,883);
    line(752,847,740,883);

    line(743,883,702,925);
    line(742,883,701,925);
    line(741,883,700,925);
    line(740,883,699,925);
    
    //袖子上的装饰环
    // Point points4[] = {{306,832},{324,833},{324,847},{305,846},{248,818},{248,812},{253,807},{255,807}};

    Point points4[] = {{774,832},{756,833},{756,847},{775,846},{832,818},{832,812},{827,807},{825,807}};
    Polygon_Point polygon4 = {
        .point_num = sizeof(points4) / sizeof(Point),//计算顶点数量
        .points = points4,//设置顶点数组
    };
    polyfill(polygon4,RGB(136,227,134));

    //胸前绿色装饰带
    setcolor(RGB(10,54,5));
    line(626,555,681,796);
    line(625,555,680,795);
    line(624,555,679,795);
    line(623,555,678,795);

    line(681,796,684,818);
    line(680,796,683,818);
    line(679,796,682,818);
    line(678,796,681,818);

    line(684,818,684,839);
    line(683,818,683,839);
    line(682,818,682,839);
    line(681,818,681,839);

    line(684,839,679,870);
    line(683,839,678,870);
    line(682,839,677,870);
    line(681,839,676,870);

    line(679,870,656,914);
    line(678,870,655,914);
    line(677,870,654,914);
    line(676,870,653,914);

    line(609,573,660,796);
    line(608,573,659,796);
    line(607,573,658,796);
    line(606,573,657,796);

    line(660,796,664,816);
    line(659,796,663,816);
    line(658,796,662,816);
    line(657,796,661,816);

    line(664,816,664,840);
    line(663,816,663,840);
    line(662,816,662,840);
    line(661,816,661,840);

    line(664,840,659,870);
    line(663,840,658,870);
    line(662,840,657,870);
    line(661,840,656,870);

    line(659,870,640,910);
    line(658,870,639,910);
    line(657,870,638,910);
    line(656,870,637,910);

    //扣排
    setcolor(RGB(99,85,74));
    line(555,577,555,890);
    line(554,577,554,890);
    line(553,577,553,890);
    line(552,577,552,890);
}

void fillArm(){
    Point points01[] = {{255,941},{279,943},{287,950},{295,950},{346,919},{350,919},{362,927},{345,933},{347,935},{540,890},{742,935},{744,932},{727,927},{738,919},{744,919},{794,951},{802,950},{810,942},{834,940},{836,999},{824,1035},{803,1051},{789,1054},{540,996},{300,1056},{280,1048},{264,1031},{252,993}};
    Polygon_Point polygon01 = {
        .point_num = sizeof(points01) / sizeof(Point),//计算顶点数量
        .points = points01,//设置顶点数组
    };
    polyfill(polygon01,RGB(252,238,231));
}

void fillShirtLace_Left(){
    Point points02[] = {{231,909},{214,888},{251,824},{257,828}};
    Polygon_Point polygon02 = {
        .point_num = sizeof(points02) / sizeof(Point),//计算顶点数量
        .points = points02,//设置顶点数组
    };
    polyfill(polygon02,RGB(239,239,239));

    Point points03[] = {{279,940},{265,939},{232,925},{239,891},{260,830},{281,841}};
    Polygon_Point polygon03 = {
        .point_num = sizeof(points03) / sizeof(Point),//计算顶点数量
        .points = points03,//设置顶点数组
    };
    polyfill(polygon03,RGB(239,239,239));

    Point points04[] = {{281,841},{281,846},{285,846},{286,842}};
    Polygon_Point polygon04 = {
        .point_num = sizeof(points04) / sizeof(Point),//计算顶点数量
        .points = points04,//设置顶点数组
    };
    polyfill(polygon04,RGB(239,239,239));

    Point points05[] = {{286,842},{283,943},{292,947},{347,916},{365,927},{369,926},{367,917},{341,891},{329,865},{325,848}};
    Polygon_Point polygon05 = {
        .point_num = sizeof(points05) / sizeof(Point),//计算顶点数量
        .points = points05,//设置顶点数组
    };
    polyfill(polygon05,RGB(239,239,239));
}

void fillShirtLace_right(){
    Point points02[] = {{849,909},{866,888},{829,824},{823,828}};
    Polygon_Point polygon02 = {
        .point_num = sizeof(points02) / sizeof(Point),//计算顶点数量
        .points = points02,//设置顶点数组
    };
    polyfill(polygon02,RGB(239,239,239));

    Point points03[] = {{801,940},{815,939},{848,925},{841,891},{820,830},{799,841}};
    Polygon_Point polygon03 = {
        .point_num = sizeof(points03) / sizeof(Point),//计算顶点数量
        .points = points03,//设置顶点数组
    };
    polyfill(polygon03,RGB(239,239,239));

    Point points04[] = {{799,841},{799,846},{795,846},{794,842}};
    Polygon_Point polygon04 = {
        .point_num = sizeof(points04) / sizeof(Point),//计算顶点数量
        .points = points04,//设置顶点数组
    };
    polyfill(polygon04,RGB(239,239,239));

    Point points05[] = {{794,842},{793,943},{788,947},{733,916},{715,927},{711,926},{713,917},{739,891},{751,865},{755,848}};
    Polygon_Point polygon05 = {
        .point_num = sizeof(points05) / sizeof(Point),//计算顶点数量
        .points = points05,//设置顶点数组
    };
    polyfill(polygon05,RGB(239,239,239));
}

void fillDress(){
    Point points[] = {{364,1080},{378,1038},{431,1026},{426,1080}};
    Polygon_Point polygon = {
        .point_num = sizeof(points) / sizeof(Point),//计算顶点数量
        .points = points,//设置顶点数组
    };
    polyfill(polygon,RGB(48,67,45));
    Point points2[] = {{716,1080},{702,1038},{649,1026},{654,1080}};
    Polygon_Point polygon2 = {
        .point_num = sizeof(points2) / sizeof(Point),//计算顶点数量
        .points = points2,//设置顶点数组
    };
    polyfill(polygon2,RGB(48,67,45));
    Point points3[] = {{430,1080},{436,1027},{540,1004},{644,1027},{650,1080}};
    Polygon_Point polygon3 = {
        .point_num = sizeof(points3) / sizeof(Point),//计算顶点数量
        .points = points3,//设置顶点数组
    };
    polyfill(polygon3,RGB(255,251,242));
}


void fillShirt_Left(){
    Point points01[] = {{323,830},{316,830},{284,815},{264,809},{236,784},{224,769},{222,760},{222,739},{225,728},{227,724},{331,577},{347,562},{354,559},{365,558},{380,562},{440,545}};
    Polygon_Point polygon01 = {
        .point_num = sizeof(points01) / sizeof(Point),//计算顶点数量
        .points = points01,//设置顶点数组
    };
    polyfill(polygon01,RGB(239,239,239));

    Point points02[] = {{440,545},{445,540},{450,548},{404,778},{398,803},{396,845},{399,862},{400,868},{423,912},{381,923},{364,911},{341,884},{331,859},{328,843},{327,826},{333,794},{347,758}};
    Polygon_Point polygon02 = {
        .point_num = sizeof(points02) / sizeof(Point),//计算顶点数量
        .points = points02,//设置顶点数组
    };
    polyfill(polygon02,RGB(239,239,239));
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

    fillShirt_Left();
    fillDress();
    fillArm();
    fillShirtLace_Left();
    fillShirtLace_right();
    draw_Left();
    draw_Right();

    // _getch();

    // 将绘制的图形保存为.jpg格式图像
    IMAGE img;
    getimage(&img, 0, 0, 1080, 1080);  // 获取整个屏幕区域的图像
    saveimage("E:\\git_workplace\\Computer_Graphis_Demo\\test.jpg",&img);

    closegraph();
    return 0;
}