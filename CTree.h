#pragma once
#include "utils.h"
#include "Factor.h"
#include "Clique.h"
#include "Graph.h"

typedef Factor Message;
class CTree :
    public Graph<Clique*, pair<edge_stata, Message>>
{
public:
    void init() {}
    void update_edge_stata(int j) {
        for (int nbr = firstNbr(j); nbr > -1; nbr = nextNbr(j, nbr)) {
            if (edge(j, nbr).first == not_ready) {
                bool con = true;
                // ���nbr->jδ���������������������ѷ��ͣ���jΪ
                for (int nbr2 = firstNbr(j); nbr2 > -1; nbr2 = nextNbr(j, nbr2)) {
                    if (edge(nbr2, j).first != sended) {
                        con = false; break;
                    }
                }
                if (con) edge(j, nbr).first = is_ready;
            }
        }
    }
    //
    void SP_Message(int i, int j) {
        Factor f = *(new Factor);
        f = *(vertex(i)->phi);
        // ����i�����ڽڵ����j
        for (int nbr = firstNbr(i); nbr > -1; nbr = nextNbr(i, nbr)) {
            f = f * edge(nbr, i).second;
        }
        // �ж�j�Ƿ����
        edge(i, j).first = sended;
        edge(i, j).second = f;
        update_edge_stata(j);
    }
    bool get_ready_clique(int& x, int& y) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (edge(i, j).first == is_ready) {
                    x = i; y = j;
                    return true;
                }
            }
        }
        return false;
    }
    void calc_all_belief() {
        this;
        function<void(int)> pf = std::bind(&CTree::calc_belief, this, std::placeholders::_1);
        bfs(0, pf);
    }
    void calc_belief(int j) {
        vertex(j)->phi;
        vector<Factor*> tmp;
        for (int nbr2 = firstNbr(j); nbr2 > -1; nbr2 = nextNbr(j, nbr2)) {
            tmp.push_back(&edge(nbr2, j).second);
        }
        *vertex(j)->phi = *(vertex(j)->phi) * *(pgm::Algorithm().Factor_Product(tmp));
    }   
    void CTree_SP_Calibrate(CTree ctree) {
        init(); // ������ʼ����ÿ�����е��������
        int i, j;
        while (ctree.get_ready_clique(i, j)) {
            SP_Message(i, j); // ���������ܿ��Է��͵��ŷ�����Ϣ��Ȼ��Ҫ������Щ���յ�����Ϣ�����ĸ�
            // �ڵ㣬Ȼ����¾���״̬
            // �м������ڽڵ���м�������״̬��ͼ�ṹ֮����˫������ӣ������ݳ�ʼ��Ϊδ����
            // ���������ͣ�pair<�Ƿ��������Ϣ��typedef��>   ��Ȩ��
        }
        calc_all_belief();
    }
    
};