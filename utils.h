#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <set>
#include <queue>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct cycle {
	float x;
	float y;
	float r;
	std::string text;
	cv::Point text_position;
	cycle(float x1, float y1) :x(x1), y(y1) { r = 20; };
	void updata_text_position() {
		int shrink = 1; float percentage = 9.0;
		float string_width = text.size() * shrink * percentage;
		float high = shrink * percentage;
		text_position = cv::Point(x - string_width / 2.0, y + high / 2.0);
	}

};
template<typename T>
void vector_scale(vector<T>& v) {	// 向量归一化，输入是左值，直接左值引用
	T sum = 0;
	for (T v_ : v) 
		sum += v_;
	for (T i = 0; i < v.size(); i++) v[i] = v[i] / sum;
}
template<typename T>
T dot_product(vector<T> v1, vector<T> v2) {
	T product = 0;  // |Val(f U *this)|---值域的元素个数
	for (int i = 0; i < v1.size(); i++) {
		product += v1[i] * v2[i];
	}
	return product;
}
template<typename T>
T vector_product(vector<T> input) {
	T product = 1;  // |Val(f U *this)|---值域的元素个数
	for (int i : input) {
		product *= i;
	}
	return product;
}
template<typename T>
T  vector_sum(vector<T> v, int i ,int j) {
	T sum = 0;  // |Val(f U *this)|---值域的元素个数
	while (i < j) {
		sum += v[i];
		i++;
	}
	return sum;
}
template<typename T>
vector<T>  vector_intersection(vector<vector<T>> v) {
	if (v.size() == 1) return v[0]; if (v.size() == 1) return vector<T>{};
	vector<T> r = v[0]; vector<T> tmp;
	for (int i = 1; i < v.size(); i++) {
		std::set_intersection(r.begin(), r.end(), v[i].begin(), v[i].end(), inserter(tmp, tmp.begin()));
		r = move(tmp);
	}
	return r;
}
template<typename T>
vector<T>  vector_union(vector<vector<T>> v) {
	vector<T> r;
	for (auto v_ : v) {
		std::set_union(r.begin(), r.end(), v_.begin(), v_.end(), inserter(r, r.begin()));
	}
	return r;
}

template<typename T>
int print_matrix(vector<T> v, int card) {
	cout << "矩阵" << endl;
	for (int i = 0;i != v.size()/card;i++)
	{
		for (int j = 0;j != card;j++)
			cout << v[i * card + j] << "\t";
		cout << endl;
	}
	return 0;
};
template<typename T>
int print_matrix(vector<vector<T>> v){
	int m = v.size(), n = v[0].size();


	for (int i = 0;i != m;i++)
	{
		for (int j = 0;j != n;j++)
			cout << v[i][j] << "\t";
		cout << endl;
	}
	return 0;
};

template<typename T>
vector<T> vector_exclude(vector<T> v, vector<T> z) {
	vector<T> r;
	for (T v_ : v) {
		if (find(z.begin(), z.end(), v_) == z.end()) {
			r.emplace_back(move(v_));  // 只有加了move才能调用右值版本减少拷贝，
		}
	}
	return r;
};
template<typename T>
vector<T> vector_exclude(vector<T> v, T z) {
	return vector_exclude(v, vector<T>{z});
};
template<typename T>
int argmax(vector<T> v) {
	T a = v[0]; int index = 0;
	for (int i = 0; i < v.size(); i++) {
		if (v[i] > a) {
			a = v[i]; index = i;
		}
	}
	return index;
};

static int dice(int range) { return rand() % range; } //取[0, range)中的随机整数
static int dice(int lo, int hi) { return lo + rand() % (hi - lo); } //取[lo, hi)中的随机整数
static float dice(float range) { return rand() % (1000 * (int)range) / (float)1000.; }
static double dice(double range) { return rand() % (1000 * (int)range) / (double)1000.; }
static char dice() { return (char)(32 + rand() % 96); }