#pragma once
#include "Graph.h"
#include "Factor_Set.h"
#include "Algorithm.h"
#include "threadpool.h"

void splits(const std::string& s, std::vector<std::string>& sv, const char flag = ' ') {
	sv.clear();
	std::istringstream iss(s);
	std::string temp;

	auto begin = s.begin();
	auto end = s.end();

	while (true) {
		auto pre = begin;
		begin = std::find(begin, end, flag);
		sv.emplace_back(std::string(pre, begin));	//	耗时
		if (begin == end) break;
		++begin;
	}
	return;
}
bool open_train_data(std::string filename, std::vector<std::vector<string>>& data) {
	std::ifstream ifs(filename);
	std::string line;
	vector<string> tmp;
	while (std::getline(ifs, line)) {
		splits(line, tmp, ',');
		data.push_back(move(tmp));
		tmp.clear();
	}
	return true;
}

class PGM
	:public Graph<std::string, int>
{
	Factor_Set fs;
	int num_vars;
	std::mutex mutex_;
	std::mutex mutex_r;
public:
	PGM() { Graph(); }
	PGM(vector<vector<int>> graph) {
	}
	//set<Vertex<std::string>> Reachable(int x, vector<int> Z) {
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
	int inference(vector<pair<int, std::string>> m) {
		std::vector<int> evidence(num_vars, -1);
		std::vector<int> order_variables_set;
		for (auto m_ : m) {
			evidence[m_.first] = distance(Factor::value_of_all_variables[m_.first].begin(),
				find(Factor::value_of_all_variables[m_.first].begin(),
					Factor::value_of_all_variables[m_.first].end(), m_.second));
			order_variables_set.emplace_back(m_.first);	// 上面已经是使用过m_，所以可以移动处理
		}
		// 将非证据清0
		Factor_Set efs;
		for (int i = 0; i < num_vars; i++) {
			Factor* f = new Factor;
			f->scope = fs[i].scope; f->card = fs[i].card; f->stride = fs[i].stride; f->table = fs[i].table;
			efs.push_factor(f);
		}
		for (int i = 0; i < num_vars; i++) {
			efs[i].EV(evidence);
		}
		int index = pgm::Algorithm().Sum_Product_VE(efs, order_variables_set);
		for (auto p : efs.factor_set_vector) delete p;
		return index;
	}
	void inference(vector<int> order_variables_set) { pgm::Algorithm().Sum_Product_VE(fs, order_variables_set); }
	void learn_graph(const vector<vector<std::string>>& data) {}
	void learn_parm(const vector<vector<std::string>>& data, vector<vector<int>>* graph = NULL, bool header = false,
		bool is_bayes = true, bool mult_thread = false) {
		int start = 0;
		if (header) { for (auto name : data[0]) { insert(name); } start++; }
		else {}
		if (!graph) learn_graph(data);
		else {
			num_vars = (*graph)[0].size();
			cout << "num_vars:::::" << num_vars << endl;
			for (int i = 0; i < num_vars; i++) insert(to_string(i));
			for (int i = 0; i < num_vars; i++) {
				for (int j = 0; j < num_vars; j++) {
					if((*graph)[i][j])
						insert(1, 1, i, j);
				}
			}
		}
		for (int i = 0; i < num_vars;i++) { fs.push_factor(new Factor); }

		if (is_bayes) {
			for (int i = 0; i < num_vars; i++) {
				for (int j = 0; j < num_vars; j++) {
					// i->j的情况下j变量的因子辖域包含i
					if ((*graph)[i][j])fs[j].scope.emplace_back(i);
				}
			}
		}
		else {
			int s = 0;
			for (int i = 0; i < (*graph).size(); i++, s++) {
				for (int j = s; j < (*graph)[0].size(); j++) {
					fs[i].scope.emplace_back(j);
					fs[j].scope.emplace_back(i);
				}
			}
		}
		for (int j = 0; j < num_vars; j++) { 
			if (fs[j].scope.size() == 0) {
				fs[j].scope.emplace_back(j); continue;
			}
			for (int k = 0; k < fs[j].scope.size(); k++) {
				if (k == 0 & j < fs[j].scope[0]) { fs[j].scope.insert(fs[j].scope.begin(), j); break; }
				if (k == fs[j].scope.size() - 1) { fs[j].scope.emplace_back(j); break; }
				if (fs[j][k] < j & fs[j][k + 1] > j) { fs[j].scope.insert(fs[j].scope.begin() + k + 1, j); break; }
			}
		}

		Factor::cards_of_all_variables.resize(num_vars);
		Factor::value_of_all_variables.resize(num_vars);

		for (int i = start; i < data.size(); i++) {
			for (int j = 0; j < num_vars; j++) {
				// 计算每个变量的值域并放在 Factor::value_of_all_variables 的相应位置
				if(find(Factor::value_of_all_variables[j].begin(), Factor::value_of_all_variables[j].end(), data[i][j])== Factor::value_of_all_variables[j].end())Factor::value_of_all_variables[j].push_back(data[i][j]);
			}
		}
		for (int j = 0; j < num_vars; j++) {
			Factor::cards_of_all_variables[j] = Factor::value_of_all_variables[j].size();
		}
		for (int i = 0; i < num_vars; i++) {
			// scope 和 card stride 顺序保持一致
			fs[i].calc_card(); fs[i].calc_stride(); fs[i].table.resize(vector_product(fs[i].card));
		}
		if (mult_thread) {
			std::threadpool tp(8);
			std::vector<std::future<bool>> future_bool;
			for (int j = 0; j < num_vars; j++) {
				future_bool.emplace_back(tp.commit(std::bind(&PGM::thread_learn, this, ref(fs[j]), data, j)));
			}
			for (int i = 0; i < future_bool.size(); i++) {
				future_bool[i].get();
			}
			return;
		}
		
		int pos;
		for (auto line : data) { // pos(ai bj ck) = i*stride[0] + j*stride[1] + k*stride[2]
			for (int i = 0; i < num_vars; i++) {  // cur = fs[i]
				vector<int> ijk;
				for (auto var : fs[i].scope) {
					ijk.emplace_back(distance(Factor::value_of_all_variables[var].begin(), find(Factor::value_of_all_variables[var].begin(), Factor::value_of_all_variables[var].end(), line[var])));
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
	bool thread_learn(Factor& f,vector<vector<std::string>>& data, int j) {
		// 更新变量值域

;		int pos;
		for (auto line : data) { // pos(ai bj ck) = i*stride[0] + j*stride[1] + k*stride[2]
			vector<int> ijk;
			for (auto var : f.scope) {
				ijk.emplace_back(distance(Factor::value_of_all_variables[var].begin(), find(Factor::value_of_all_variables[var].begin(), Factor::value_of_all_variables[var].end(), line[var])));
			}
			pos = dot_product(ijk, f.stride);
			f.table[pos] += 1;
		}
		if(true)for (int i = 0; i < num_vars; i++)	f.scale(true);
		return true;
	}
	void show() { show_skeleton(); }

	void naive_bayes_fit(const std::vector<std::vector<string>>& data, bool mult_thread) {
		int num_vars = data[0].size();
		vector<vector<int>> graph(num_vars, vector<int>(num_vars));
		for (int i = 0; i < num_vars - 1; i++) { graph[num_vars - 1][i] = 1; }

		graph[0][5] = 1; graph[0][8];graph[0][12] = 1;
		graph[8][9] = 1;graph[10][11] = 1; graph[12][13] = 1;
		graph[14][15] = 1; graph[16][17] = 1; graph[18][19] = 1;

		learn_parm(data, &graph, false, true, mult_thread);
		//vector<pair<int, std::string>> evidence; evidence.push_back({ 0, "青色" });
		//evidence.push_back({ 1, "重" }); evidence.push_back({ 2, "长" });
		//pgm.inference(evidence);
		//pgm.inference(vector<int>{0, 3, 1});
	}
	double thread_inference(vector<int>& r, std::vector<std::vector<string>>& data, int i, int j,
		vector<string>& labels) {
		vector<pair<int, std::string>> e; int z = i;
		for (int sample = i; sample < j; sample++) {
			for (int ii = 0; ii < num_vars - 1; ii++) {
				e.push_back({ ii, data[sample][ii] });
			}
			//std::lock_guard<std::mutex> head_lock(mutex_r);
			r.emplace_back(inference(e));
			cout << "thread "<< std::this_thread::get_id() << ":"<< 100.0 * double(sample-z)/ (j - z)<< "%" << endl; // /(j-z)
			e.clear();
		}
		int correct = 0;
		for (int k = 0; k < r.size();k++) {
			if (int(distance(Factor::value_of_all_variables[num_vars - 1].begin(),
				find(Factor::value_of_all_variables[num_vars - 1].begin(), Factor::value_of_all_variables[num_vars - 1].end(),
					labels[z+k]))) == r[k])
				correct++;
		}
		double corr = (double)correct / (j - i);
		// cout << "corr : " <<  corr << endl;
		return corr;
	}
	void inference(const std::string filename, std::string mode = "test", bool mult_thread = false) {
		vector<pair<int, std::string>> e; vector<string> labels;
		std::vector<std::vector<string>> data;
		open_train_data(filename, data);
		if (mode != "test") { for (auto sample : data)labels.emplace_back(move(sample.back()));}
		int zz = 0;
		if (mult_thread) {
			vector<vector<int>> res(8);
			std::threadpool tp(8); int n = data.size() / 8; int init = 0;
			std::vector<std::future<double>> future_bool;
			for (int j = 0; j < 7; j++) {
				vector<int> a;
				future_bool.emplace_back(tp.commit(std::bind(&PGM::thread_inference, this, 
					ref(res[j]), data, init, init+n, labels)));
				init = init + n;
			}

			future_bool.emplace_back(tp.commit(std::bind(&PGM::thread_inference, this,
				ref(res[7]), data, init, data.size(), labels)));
			double avl_corr = 0.0;
			for (int i = 0; i < future_bool.size(); i++) {
				avl_corr += future_bool[i].get();
			}
			cout << "avl_corr : " << double(avl_corr/8) << endl;
			return;
		}
		vector<int> res;
		for (auto sample : data) {
			for (int i = 0; i < num_vars - 1; i++) {
				e.push_back({i, sample[i]});
			}
			res.emplace_back(inference(e));
			cout << zz++ << endl;
			e.clear();
		}
		int correct = 0;
		for (int i = 0; i < labels.size();i++) { 
			if (int(distance(Factor::value_of_all_variables[num_vars - 1].begin(),
				find(Factor::value_of_all_variables[num_vars - 1].begin(), Factor::value_of_all_variables[num_vars - 1].end(),
					labels[i]))) == res[i])
				correct++; 
			cout << i << endl;
		}
		cout << "corr : " << (double)correct / data.size() << endl;
	}
};

