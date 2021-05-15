#pragma once
#include "utils.h"
typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus; //顶点状态
typedef enum { UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD } EType; //边在遍历树中所属的类型

template <typename Tv> struct Vertex { //顶点对象（为简化起见，并未严格封装）
    Tv data; int inDegree, outDegree; VStatus status; //数据、出入度数、状态
    int dTime, fTime; //时间标签
    int parent; int priority; //在遍历树中的父节点、优先级数
    Vertex(Tv const& d = (Tv)0) : //构造新顶点
        data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
        dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {} //暂不考虑权重溢出
};
template <typename Te> struct Edge { //边对象（为简化起见，并未严格封装）
    Te data; int weight; EType type; //数据、权重、类型
    Edge(Te const& d, int w) : data(d), weight(w), type(UNDETERMINED) {} //构造
};

template <typename Tv>
struct print_vertex_data//
{
    virtual void operator() (Vertex<Tv>& e) { printf("%d", e.data); }
};


template <typename Tv, typename Te> //顶点类型、边类型
class Graph {
private:
    vector<Vertex<Tv>> V; //顶点集（顶点）
    vector< vector< Edge< Te >* > > E; //边集（指向边的指针）
public:
    int n;    int e;
    Graph() :n(0), e(0) {}

    //如果返回的是引用类型，可以作为左值
    Tv& vertex(int i) { return V[i].data; }
    VStatus& status(int i) { return V[i].status; } //状态
    int& dTime(int i) { return V[i].dTime; } //时间标签dTime
    int& fTime(int i) { return V[i].fTime; } //时间标签fTime
    int& parent(int i) { return V[i].parent; } //在遍历树中的父亲
    int& priority(int i) { return V[i].priority; } //在遍历树中的优先级数
    EType& type(int i, int j) { return E[i][j]->type; } //边(i, j)的类型
    Te& edge(int i, int j) { return E[i][j]->data; } //边(i, j)的数据
    int& weight(int i, int j) { return E[i][j]->weight; } //边(i, j)的权重

    //非引用类型，不能作为左值
    int inDegree(int i) { return V[i].inDegree; } //入度
    int outDegree(int i) { return V[i].outDegree; } //出度
    bool exists(int i, int j) //边(i->j)是否存在
    {
        return (0 <= i) && (i < n) && (0 <= j) && (j < n) && E[i][j] != NULL;
    }
    int nextNbr(int i, int j) //相对于顶点j的下一邻接顶点（改用邻接表可提高效率）
    {
        while ((-1 < j) && (!exists(i, --j))); return j;
    } //逆向线性试探
    int firstNbr(int i) { return nextNbr(i, n); } //首个邻接顶点
    vector<int> getParents(int x) {
        vector<int> parents;
        for (int i = 0; i < n; i++) {
            if (exists(i, x)) parents.push_back(i);
        }
        return parents;
    }
    void insert(Tv const& vertex) { //插入顶点
        for (int j = 0; j < n; j++) E[j].push_back(NULL); n++; //各顶点预留一条潜在的关联边
        E.push_back(vector<Edge<Te>*>(n, (Edge<Te>*) NULL)); //创建新顶点对应的边向量
        //Vertex<>(构造函数) Tv：顶点的数据类型
        V.push_back(Vertex<Tv>(vertex)); //顶点向量增加一个顶点
    }

    void insert(Te const& edge, int w, int i, int j, bool is_directed = true) { //插入权重为w的边e = (i, j)
        if (exists(i, j)) return;
        E[i][j] = new Edge<Te>(edge, w);
        e++; V[i].outDegree++; V[j].inDegree++;
        if (!is_directed) {
            E[j][i] = new Edge<Te>(edge, w);//如果是无向图：添加在二维数组中对称的边
            e++; V[j].outDegree++; V[i].inDegree++;
        }
    }

    void remove(int i) {
        for (int j = 0; j < n; j++)
            if (exists(j, i)) { E[j].erase(E[j].begin() + i); V[j].outDegree--; e--; }
        n--;
        for (int j = 0; j < n; j++)
            if (exists(i, j)) { V[j].inDegree--; e--; }
        E.erase(E.begin() + i);
    }
    void remove(int i, int j) {
        E[i][j] = NULL;//分号容易打错
        e--;
        V[j].inDegree = V[j].inDegree - 1;
        V[i].outDegree = V[i].outDegree - 1;
    }

    // visit指向函数的指针，函数返回类型为空，函数参数列表为Vertex，Vertex模板参数为<Tv>
    //void bfs(int i, void(*visit)(Vertex<Tv>)) { //只要visit重载了()就可以，结构体，函数都可以
    //    queue<int> q;
    //    q.push(i);
    //    while (!q.empty()) {
    //        int num_current_vertex = q.front(); q.pop();
    //        visit(V[num_current_vertex]);
    //        status(num_current_vertex) = VISITED;
    //        for (int j = firstNbr(num_current_vertex); j > -1; j = nextNbr(num_current_vertex, j)) {  // i的邻接节点
    //            bool a = exists(num_current_vertex, j);
    //            bool b = !(status(j) == VISITED);
    //            if (exists(num_current_vertex, j) && !(status(j) == VISITED)) {
    //                q.push(j);
    //            }
    //        }
    //    }
    //}
    //void bfs(int i, function<void(int)> visit) { //只要visit重载了()就可以，结构体，函数都可以
    //    queue<int> q;
    //    q.push(i);
    //    while (!q.empty()) {
    //        int num_current_vertex = q.front(); q.pop();
    //        visit(num_current_vertex);
    //        status(num_current_vertex) = VISITED;
    //        for (int j = firstNbr(num_current_vertex); j > -1; j = nextNbr(num_current_vertex, j)) {  // i的邻接节点
    //            bool a = exists(num_current_vertex, j);
    //            bool b = !(status(j) == VISITED);
    //            if (exists(num_current_vertex, j) && !(status(j) == VISITED)) {
    //                q.push(j);
    //            }
    //        }
    //    }
    //}

    //以第center个Vertex为中心展示图的skeleton
    //void show_skeleton(int center = 0) {
    //    int w = 800;
    //    int r = w / (8 * n);
    //    Scalar color = Scalar(247, 245, 252);//BGR
    //    Mat img = Mat::ones(w, w, CV_8UC3);
    //    Point cur_point = Point(w / 2, w / 2);
    //    circle(img, cur_point, r, color, 1, 8);
    //    status(center) = VISITED;
    //    queue<int> q;
    //    q.push(center);
    //    vector<int> coordinate(2 * n);//每一个节点的坐标
    //    coordinate[0] = cur_point.x;
    //    coordinate[1] = cur_point.y;
    //    int x; int y; int num_cur_vertex; int c;//当前节点的第c个Nbr
    //    vector<int> virtual_parents(n);//每一个节点的虚拟父节点是第几个---也就是画当前圆时应该连接到哪个节点
    //    while (!q.empty()) {
    //        //每个节点都会进入队列--按照距离center的距离
    //        num_cur_vertex = q.front(); q.pop();
    //        Point cur_point = Point(coordinate[num_cur_vertex * 2], coordinate[num_cur_vertex * 2 + 1]);
    //        x = cur_point.x; y = cur_point.y;
    //        circle(img, cur_point, r, color, -1, 8);
    //        Point text_point = Point(cur_point.x - r / 4, cur_point.y + r / 4);
    //        putText(img, V[num_cur_vertex].data, text_point, CV_FONT_HERSHEY_COMPLEX, 0.5, Scalar(106, 68, 20), 1, 8);
    //        status(num_cur_vertex) = VISITED; c = 0;
    //        for (int j = firstNbr(num_cur_vertex); j > -1; j = nextNbr(num_cur_vertex, j)) {  // i的邻接节点
    //            if (exists(num_cur_vertex, j) && !(status(j) == VISITED)) {
    //                q.push(j); c++;
    //                coordinate[2 * j] = x + 8 * r * cos(c * 3.1415926 * (360 + dice(240)) / 180 / (V[num_cur_vertex].inDegree + 1));
    //                coordinate[2 * j + 1] = y + 8 * r * sin(c * 3.1415926 * (360 + dice(60)) / 180 / (V[num_cur_vertex].inDegree + 1));
    //            }
    //        }

    //    }
    //    int col = 0; int start = 0;
    //    for (int row = 0; row < n; row++) {
    //        start++;
    //        for (int col = start; col < n; col++) {
    //            if (exists(row, col)) {
    //                Point p1 = Point(coordinate[row * 2], coordinate[row * 2 + 1]);
    //                Point p2 = Point(coordinate[col * 2], coordinate[col * 2 + 1]);
    //                line(img, p1, p2, color, 1, 8);
    //            }
    //        }
    //    }
    //    imshow("Graph skeleton", img);
    //    waitKey(0);
    //}
};
