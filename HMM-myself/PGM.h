#pragma once
#include "Graph.h"
#include "Factor_Set.h"
#include "Algorithm.h"

class PGM
	:Graph<std::string, int>
{
	Factor_Set fs;
	int num_vars;
public:
	PGM() { Graph(); }
	PGM(vector<vector<int>> graph) {

	}
	//set<Vertex<string>> Reachable(int x, vector<int> Z) {
	//	enum dir { up, down };
	//	vector<int> L = Z;
	//	set<int> A;
	//	vector<int> parents;
	//	while (!L.empty()) {
	//		int Y = L.back(); L.pop_back();
	//		if (find(A.begin(), A.end(), Y) == A.end()) {
	//			parents = getParents(Y);
	//			set_union(L.begin(), L.end(), parents.begin(), parents.end(), L);
	//		}
	//		A.insert(Y);
	//	}
	//	delete &L;
	//	vector<pair<int, dir>> L{ {x, up} };  // 标记将要被访问的节点
	//	set<pair<int, dir>> V; // 标记已经访问的节点
	//	set<int> R;
	//	while (!L.empty()) {
	//		auto yd = L.back(); L.pop_back();
	//		if (find(V.begin(), V.end(), yd) == V.end()) {
	//			if (1) {

	//			}
	//		}
	//	}
	//}
	void inference(vector<int> order_variables_set) { pgm::Algorithm().Sum_Product_VE(fs, order_variables_set); }
	void learn_graph(const vector<vector<string>>& data) {}
	void learn_parm(const vector<vector<string>>& data, vector<vector<int>>* graph = NULL, bool header = false, bool is_bayes = true) {
		int start = 0;
		if (header) { for (auto name : data[0]) { insert(name); } start++; }
		else {}
		if (!graph) learn_graph(data);
		else {
			num_vars = (*graph).size();
			insert("颜色"); insert("轻重");insert("长短");insert("好坏");
			for (int i = 0; i < num_vars; i++) {
				for (int j = 0; j < num_vars; j++) {
					if((*graph)[i][j])insert(1, 1, i, j);
				}
			}
		}
		for (int i = 0; i < num_vars;i++) { fs.push_factor(new Factor); }
		for (int j = 0; j < num_vars; j++) {fs[j].scope.push_back(j);}

		if (is_bayes) {
			for (int i = 0; i < num_vars; i++) {
				for (int j = 0; j < num_vars; j++) {
					// i->j的情况下j变量的因子辖域包含i
					if ((*graph)[i][j])fs[j].scope.push_back(i);
				}
			}
		}
		else {
			int s = 0;
			for (int i = 0; i < (*graph).size(); i++, s++) {
				for (int j = s; j < (*graph)[0].size(); j++) {
					fs[i].scope.push_back(j);
					fs[j].scope.push_back(i);
				}
			}
		}
		Factor::value_of_all_variables.resize(num_vars);
		for (int i = start; i < data.size(); i++) {
			for (int j = 0; j < num_vars; j++) {
				if(find(Factor::value_of_all_variables[j].begin(), Factor::value_of_all_variables[j].end(), data[i][j])== Factor::value_of_all_variables[j].end())Factor::value_of_all_variables[j].push_back(data[i][j]);
			}
		}
		for (int j = 0; j < num_vars; j++) {
			Factor::cards_of_all_variables.push_back(Factor::value_of_all_variables[j].size());
		}

		for (int i = 0; i < num_vars; i++) {
			// scope 和 card stride 顺序保持一致
			fs[i].calc_card(); fs[i].calc_stride(); fs[i].table.resize(vector_product(fs[i].card));
		}
		int pos;
		for (auto line : data) { // pos(ai bj ck) = i*stride[0] + j*stride[1] + k*stride[2]
			for (int i = 0; i < num_vars; i++) {  // cur = fs[i]
				vector<int> ijk;
				for (auto var : fs[i].scope) {
					ijk.push_back(distance(Factor::value_of_all_variables[var].begin(), find(Factor::value_of_all_variables[var].begin(), Factor::value_of_all_variables[var].end(), line[var])));
					cout << ijk.back() << endl;
				}
				pos = dot_product(ijk, fs[i].stride);
				fs[i].table[pos] += 1;
			}
		}
		if(is_bayes)for (int i = 0; i < num_vars; i++)	fs[i].scale(is_bayes);
		//for (auto p : fs.factor_set_vector) {
		//	print_matrix(p->table, p->card[0]);
		//}
	}
};

