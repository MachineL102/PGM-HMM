#pragma once
#include "Factor.h"

class Factor_Set
{
public:
	vector<Factor*> factor_set_vector;  // 只是包含一些指针
	int size() { return factor_set_vector.size(); }
	int size()const { return factor_set_vector.size(); }
	//Factor_Set& operator=(const Factor_Set& copy) {
	//	factor_set_vector.clear();
	//	for (auto factor_p : copy.factor_set_vector) {
	//		Factor f = *factor_p;  // 触发了带指针的类的复制构造
	//		factor_set_vector.push_back(&f);
	//	}
	//	return *this;
	//}
	Factor& operator[](int i)const {	 // 注意：返回类型引用
		return *factor_set_vector[i];  // 如果Factor触发赋值构造，也是申请一块新的内存
	}
	void push_factor(Factor* fp) {
		factor_set_vector.push_back(fp);
	}
};

