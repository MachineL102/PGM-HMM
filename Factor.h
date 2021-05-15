#pragma once
#include "utils.h"

class Factor
{
public:
	static vector<int> cards_of_all_variables;
	static vector<vector<std::string>> value_of_all_variables;
	vector<int> scope;	// scope[0]为子节点
	vector<int> card;
	vector<int> stride;  // scope中每个变量的跨度（到达该变量的下一个赋值需要跨过多少个元素）
	vector<double> table;
	Factor& operator=(const Factor& f) {
		scope = f.scope; card = f.card; stride = f.stride; table = f.table;
		return *this;
	}
	Factor& operator=(Factor&& right_value) {
		table = move(right_value.table);
		scope = move(right_value.scope);
		card = move(right_value.card);
		stride = move(right_value.stride);
		return * this;
	}
	Factor(const Factor& f) {
		scope = f.scope; card = f.card; stride = f.stride; table = f.table;
	}
	Factor(){}
	double operator[](int index) {
		return scope[index];
	}
	double operator[](int index)const {
		return scope[index];
	}
	int get_stride(int variable) const {
		vector<int>::const_iterator it = find(scope.begin(), scope.end(), variable);
		if (it == scope.end()) return 0;
		else return stride[distance(scope.begin(), it)];
	}
	void Eliminate(int variable) { return Eliminate(vector<int>{variable}); }
	void Eliminate(vector<int> Z) {
		vector<int> not_in_z = vector_exclude(scope, Z);
		vector<int> init;//cards_of_all_variables[not_in_z.back()]*get_stride(not_in_z.back())
		for (int i = 0; i < cards_of_all_variables[not_in_z[0]]; i++) {
			init.emplace_back(i*get_stride(not_in_z[0]));	// 数值为临时对象，刚好使用右值版本函数，转移所有权即可
		}
		for (int i = 1; i < not_in_z.size(); i++) {
			int size = init.size();
			int cur_stride = get_stride(not_in_z[i]);
			int cur_card = cards_of_all_variables[not_in_z[i]];
			for (int j = 1; j < cur_card; j++) {	// 赋值cur_card-1份，每份加cur_stride
				for (int k = 0; k < size; k++) {
					init.emplace_back(init[k] + j*cur_stride);
				}
			}
		}
		vector<int> intersection;
		vector<double> new_table;
		vector<vector<int>> matrix;
		for (auto init_ : init) {
			for (int i = 0; i < not_in_z.size(); i++) {
				vector<int> tmp;
				int s1 = get_stride(not_in_z[i]);
				int c1 = cards_of_all_variables[not_in_z[i]];
				for (int j = 0; j < table.size(); j++) {
					int pos = init_ + ((c1 * s1) * j);
					for(int k = 0; k < s1; k++)if(pos < table.size())tmp.emplace_back(pos + k);
				}
				matrix.push_back(tmp);
			}
			intersection = vector_intersection(matrix);
			double s = 0.0;
			for (auto pos : intersection) {
				s += table[pos];
			}
			new_table.emplace_back(s);// 如果参数不是临时对象则没什么用
			matrix.clear();
		}
		scope = not_in_z;
		calc_card(); calc_stride(); table = move(new_table);//move将用作函数参数的左值当作右值处理，除此之外move右值没用
		//print_matrix(table, card[0]);
	}
	void EV(std::vector<int> E) {
		int init_; vector<int> tmp; int b = 0; vector<vector<int>> mat;
		for (int i = 0; i < E.size(); i++) {
			if (E[i] == -1 || find(scope.begin(), scope.end(), i)==scope.end()) continue;
			int s = get_stride(i);
			int c = cards_of_all_variables[i];
			init_ = E[i] * s;
			for (int j = 0; j < table.size(); j++) {
				int pos = init_ + ((c * s) * j); // 该证据变量的位置init_，持续取s个，下一个该取值在c*s之后
				for (int k = 0; k < s; k++)
					if (pos < table.size())tmp.emplace_back(pos + k);
			}
			mat.emplace_back(move(tmp));
		}
		if (mat.size() == 0) return;
		tmp = vector_intersection(mat);  // 自动调用移动赋值
		for (int t = 0;t < table.size();t++) {
			if (t != tmp[b]) table[t] = 0;
			else b += 1; if (b == tmp.size()) break;
		}
		for (int t = tmp.back()+1;t < table.size();t++) {
			table[t] = 0;
		}
	}
	Factor operator*(const Factor& f) {
		int j = 0, k = 0; vector<int> scope_union;
		set_union(f.scope.begin(), f.scope.end(), scope.begin(), scope.end(), std::inserter(scope_union, scope_union.begin()));
		vector<int> assignment(scope_union.size()+1);
		int product = 1;  // |Val(f U *this)|---值域的元素个数
		for (int variable : scope_union) {
			product *= cards_of_all_variables[variable];
		}
		vector<double> psi = *(new vector<double>(product));  // 1.堆空间大 2.堆中申请内存防止该函数结束时被内存被释放（复制比较浪费时间）
		for (int i = 0; i < product; i++) {
			psi[i] = f.table[j] * table[k];
			for (int l = 0; l < scope_union.size();l++) {
				assignment[l] += 1;
				if (assignment[l] == cards_of_all_variables[scope_union[l]]) {
					assignment[l] = 0;
					j -= (cards_of_all_variables[scope_union[l]] - 1) * f.get_stride(scope_union[l]);
					k -= (cards_of_all_variables[scope_union[l]] - 1) * this->get_stride(scope_union[l]);
				}
				else {
					j += f.get_stride(scope_union[l]);
					k += this->get_stride(scope_union[l]);
					break;
				}
			}
		}
		Factor result;
		result.scope = scope_union;
		result.calc_card(); result.calc_stride();
		result.table = move(psi);// 将psi左值变成右值，因为psi是堆中申请的内存，
		// 所以不会被析构，因此可以直接将psi移动构造，接管psi
		return result;
	}
	void calc_card() {
		card.clear();
		for (auto i : scope) {
			card.push_back(cards_of_all_variables[i]);
		}
	}
	void calc_stride() {
		stride.clear();
		int product = 1;
		stride.push_back(1);
		for (int i = 1; i < card.size();i++) {
			product *= card[i - 1];
			stride.push_back(product);
		}
	}

	// table归一化，贝叶斯网中除了自身节点都是父节点
	void scale(bool is_bayes) {
		//if (is_bayes) {
		//	double sum = 0.0;
		//	int i = 0;
		//	while (i < table.size()) {
		//		sum = vector_sum(table, i, i+card[0]);
		//		for (int j = 0; j < card[0]; j++,i++) {
		//			table[i] /= sum;
		//		}
		//	}
		//}
	}
};

//


