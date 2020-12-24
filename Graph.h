#pragma once
#include "utils.h"
typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus; //����״̬
typedef enum { UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD } EType; //���ڱ�����������������

template <typename Tv> struct Vertex { //�������Ϊ���������δ�ϸ��װ��
    Tv data; int inDegree, outDegree; VStatus status; //���ݡ����������״̬
    int dTime, fTime; //ʱ���ǩ
    int parent; int priority; //�ڱ������еĸ��ڵ㡢���ȼ���
    Vertex(Tv const& d = (Tv)0) : //�����¶���
        data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
        dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {} //�ݲ�����Ȩ�����
};
template <typename Te> struct Edge { //�߶���Ϊ���������δ�ϸ��װ��
    Te data; int weight; EType type; //���ݡ�Ȩ�ء�����
    Edge(Te const& d, int w) : data(d), weight(w), type(UNDETERMINED) {} //����
};

template <typename Tv>
struct print_vertex_data//
{
    virtual void operator() (Vertex<Tv>& e) { printf("%d", e.data); }
};


template <typename Tv, typename Te> //�������͡�������
class Graph {
private:
    vector<Vertex<Tv>> V; //���㼯�����㣩
    vector< vector< Edge< Te >* > > E; //�߼���ָ��ߵ�ָ�룩
public:
    int n;    int e;
    Graph() :n(0), e(0) {}

    //������ص����������ͣ�������Ϊ��ֵ
    Tv& vertex(int i) { return V[i].data; }
    VStatus& status(int i) { return V[i].status; } //״̬
    int& dTime(int i) { return V[i].dTime; } //ʱ���ǩdTime
    int& fTime(int i) { return V[i].fTime; } //ʱ���ǩfTime
    int& parent(int i) { return V[i].parent; } //�ڱ������еĸ���
    int& priority(int i) { return V[i].priority; } //�ڱ������е����ȼ���
    EType& type(int i, int j) { return E[i][j]->type; } //��(i, j)������
    Te& edge(int i, int j) { return E[i][j]->data; } //��(i, j)������
    int& weight(int i, int j) { return E[i][j]->weight; } //��(i, j)��Ȩ��

    //���������ͣ�������Ϊ��ֵ
    int inDegree(int i) { return V[i].inDegree; } //���
    int outDegree(int i) { return V[i].outDegree; } //����
    bool exists(int i, int j) //��(i->j)�Ƿ����
    {
        return (0 <= i) && (i < n) && (0 <= j) && (j < n) && E[i][j] != NULL;
    }
    int nextNbr(int i, int j) //����ڶ���j����һ�ڽӶ��㣨�����ڽӱ�����Ч�ʣ�
    {
        while ((-1 < j) && (!exists(i, --j))); 
        return j;
    } //����������̽
    int firstNbr(int i) { return nextNbr(i, n); } //�׸��ڽӶ���
    vector<int> getParents(int x) {
        vector<int> parents;
        for (int i = 0; i < n; i++) {
            if (exists(i, x)) parents.push_back(i);
        }
        return parents;
    }
    void insert(Tv const& vertex) { //���붥��
        if (n == 0) {
            E.emplace_back(vector<Edge<Te>*>(1, (Edge<Te>*) NULL)); n++;
            V.emplace_back(Vertex<Tv>(vertex)); return;
        }
        for (int j = 0; j < n; j++) E[j].push_back(NULL); n++; //������Ԥ��һ��Ǳ�ڵĹ�����
        E.emplace_back(vector<Edge<Te>*>(n, (Edge<Te>*) NULL)); //�����¶����Ӧ�ı�����
        //Vertex<>(���캯��) Tv���������������
        V.emplace_back(Vertex<Tv>(vertex)); //������������һ������
    }

    void insert(Te const& edge, int w, int i, int j, bool is_directed = true) { //����Ȩ��Ϊw�ı�e = (i, j)
        if (exists(i, j)) return;
        E[i][j] = new Edge<Te>(edge, w);
        e++; V[i].outDegree++; V[j].inDegree++;
        if (!is_directed) {
            E[j][i] = new Edge<Te>(edge, w);//���������ͼ������ڶ�ά�����жԳƵı�
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
        E[i][j] = NULL;//�ֺ����״��
        e--;
        V[j].inDegree = V[j].inDegree - 1;
        V[i].outDegree = V[i].outDegree - 1;
    }

     //visitָ������ָ�룬������������Ϊ�գ����������б�ΪVertex��Vertexģ�����Ϊ<Tv>
    void bfs(int i, void(*visit)(Vertex<Tv>)) { //ֻҪvisit������()�Ϳ��ԣ��ṹ�壬����������
        queue<int> q;
        q.push(i);
        while (!q.empty()) {
            int num_current_vertex = q.front(); q.pop();
            visit(V[num_current_vertex]);
            status(num_current_vertex) = VISITED;
            for (int j = firstNbr(num_current_vertex); j > -1; j = nextNbr(num_current_vertex, j)) {  // i���ڽӽڵ�
                bool a = exists(num_current_vertex, j);
                bool b = !(status(j) == VISITED);
                if (exists(num_current_vertex, j) && !(status(j) == VISITED)) {
                    q.push(j);
                }
            }
        }
    }
    void bfs(int i, function<void(int)> visit) { //ֻҪvisit������()�Ϳ��ԣ��ṹ�壬����������
        queue<int> q;
        q.push(i);
        while (!q.empty()) {
            int num_current_vertex = q.front(); q.pop();
            visit(num_current_vertex);
            status(num_current_vertex) = VISITED;
            for (int j = firstNbr(num_current_vertex); j > -1; j = nextNbr(num_current_vertex, j)) {  // i���ڽӽڵ�
                bool a = exists(num_current_vertex, j);
                bool b = !(status(j) == VISITED);
                if (exists(num_current_vertex, j) && !(status(j) == VISITED)) {
                    q.push(j);
                }
            }
        }
    }

    //�Ե�center��VertexΪ����չʾͼ��skeleton
    void show_skeleton(int center = 0) {
        int w = 800;
        int r = w / (8 * n);
        Scalar color = Scalar(247, 245, 252);//BGR
        Mat img = Mat::ones(w, w, CV_8UC3);
        Point cur_point = Point(w / 2, w / 2);
        circle(img, cur_point, r, color, 1, 8);
        status(center) = VISITED;
        queue<int> q;
        q.push(center);
        vector<int> coordinate(2 * n);//ÿһ���ڵ������
        coordinate[0] = cur_point.x;
        coordinate[1] = cur_point.y;
        int x; int y; int num_cur_vertex; int c;//��ǰ�ڵ�ĵ�c��Nbr
        vector<int> virtual_parents(n);//ÿһ���ڵ�����⸸�ڵ��ǵڼ���---Ҳ���ǻ���ǰԲʱӦ�����ӵ��ĸ��ڵ�
        while (!q.empty()) {
            //ÿ���ڵ㶼��������--���վ���center�ľ���
            num_cur_vertex = q.front(); q.pop();
            Point cur_point = Point(coordinate[num_cur_vertex * 2], coordinate[num_cur_vertex * 2 + 1]);
            x = cur_point.x; y = cur_point.y;
            circle(img, cur_point, r, color, -1, 8);
            Point text_point = Point(cur_point.x - r / 4, cur_point.y + r / 4);
            putText(img, V[num_cur_vertex].data, text_point, 2, 0.5, Scalar(106, 68, 20), 1, 8);
            status(num_cur_vertex) = VISITED; c = 0;
            for (int j = firstNbr(num_cur_vertex); j > -1; j = nextNbr(num_cur_vertex, j)) {  // i���ڽӽڵ�
                if (exists(num_cur_vertex, j) && !(status(j) == VISITED)) {
                    q.push(j); c++;
                    coordinate[2 * j] = x + 8 * r * cos(c * 3.1415926 * (360 + dice(240)) / 180 / (V[num_cur_vertex].inDegree + 1));
                    coordinate[2 * j + 1] = y + 8 * r * sin(c * 3.1415926 * (360 + dice(60)) / 180 / (V[num_cur_vertex].inDegree + 1));
                }
            }

        }
        int col = 0; int start = 0;
        for (int row = 0; row < n; row++) {
            start++;
            for (int col = start; col < n; col++) {
                if (exists(row, col)) {
                    Point p1 = Point(coordinate[row * 2], coordinate[row * 2 + 1]);
                    Point p2 = Point(coordinate[col * 2], coordinate[col * 2 + 1]);
                    line(img, p1, p2, color, 1, 8);
                }
            }
        }
        imshow("Graph skeleton", img);
        waitKey(0);
    }
};
