#pragma once
#include "utils.h"

class Factor
{
public:
	static vector<int> cards_of_all_variables;
	static vector<vector<string>> value_of_all_variables;
	vector<int> scope;	// scope[0]Ϊ�ӽڵ�
	vector<int> card;
	vector<int> stride;  // scope��ÿ�������Ŀ�ȣ�����ñ�������һ����ֵ��Ҫ������ٸ�Ԫ�أ�
	vector<double> table;
	// ���ڲ���ָ�룬���Բ�����д��ֵ����
	//Factor& operator=(const Factor& copy) {
	//	table = copy.table;
	//	scope = copy.scope;
	//	return *this;
	//}
	//Factor(){}
	//Factor(Factor&& f) { scope = move(f.scope); card = move(f.card); stride = move(f.stride); table = move(f.table); value = move(f.value); }
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
			init.emplace_back(i*get_stride(not_in_z[0]));	// ��ֵΪ��ʱ���󣬸պ�ʹ����ֵ�汾������ת������Ȩ����
		}
		for (int i = 1; i < not_in_z.size(); i++) {
			int size = init.size();
			int cur_stride = get_stride(not_in_z[i]);
			int cur_card = cards_of_all_variables[not_in_z[i]];
			for (int j = 1; j < cur_card; j++) {	// ��ֵcur_card-1�ݣ�ÿ�ݼ�cur_stride
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
				for (int j = 0; j < c1; j++) {
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
			new_table.emplace_back(s);// �������������ʱ������ûʲô��
			matrix.clear();
		}
		scope = not_in_z;
		calc_card(); calc_stride(); table = move(new_table);//move������������������ֵ������ֵ��������֮��move��ֵû��
		cout << &table << " " << &new_table << endl;
		print_matrix(table, card[0]);
	}
	Factor operator*(const Factor& f) {
		int j = 0, k = 0; vector<int> scope_union;
		set_union(f.scope.begin(), f.scope.end(), this->scope.begin(), this->scope.end(), std::inserter(scope_union, scope_union.begin()));
		vector<int> assignment(scope_union.size());
		int product = 1;  // |Val(f U *this)|---ֵ���Ԫ�ظ���
		for (int variable : scope_union) {
			product *= cards_of_all_variables[variable];
		}
		vector<double> psi = *(new vector<double>(product));  // 1.�ѿռ�� 2.���������ڴ��ֹ�ú�������ʱ���ڴ汻�ͷţ����ƱȽ��˷�ʱ�䣩
		for (int i = 0; i < product; i++) {
			psi[i] = f.table[j] * table[j];
			for (int l = 0; l < scope_union.size();l++) {
				assignment[l] += 1;
				if (assignment[l] == cards_of_all_variables[scope_union[l]]) {
					j -= (cards_of_all_variables[scope_union[l]] - 1) * f.get_stride(l);
					k -= (cards_of_all_variables[scope_union[l]] - 1) * this->get_stride(l);
				}
				else {
					j += stride[l];
					k += stride[l];
					break;
				}
			}
		}
		Factor result;
		result.scope = scope_union;
		result.calc_card(); result.calc_stride();
		result.table = move(psi);// ��psi��ֵ�����ֵ����Ϊpsi�Ƕ���������ڴ棬
		// ���Բ��ᱻ��������˿���ֱ�ӽ�psi�ƶ����죬�ӹ�psi
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

	// table��һ������Ҷ˹���г�������ڵ㶼�Ǹ��ڵ�
	void scale(bool is_bayes) {
		if (is_bayes) {
			double sum = 0.0;
			int i = 0;
			while (i < table.size()) {
				sum = vector_sum(table, i, i+card[0]);
				for (int j = 0; j < card[0]; j++,i++) {
					table[i] /= sum;
				}
			}
		}
	}
};

//


