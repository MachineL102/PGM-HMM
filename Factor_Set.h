#pragma once
#include "Factor.h"

class Factor_Set
{
public:
	vector<Factor*> factor_set_vector;  // ֻ�ǰ���һЩָ��
	int size() { return factor_set_vector.size(); }
	int size()const { return factor_set_vector.size(); }
	//Factor_Set& operator=(const Factor_Set& copy) {
	//	factor_set_vector.clear();
	//	for (auto factor_p : copy.factor_set_vector) {
	//		Factor f = *factor_p;  // �����˴�ָ�����ĸ��ƹ���
	//		factor_set_vector.push_back(&f);
	//	}
	//	return *this;
	//}
	Factor& operator[](int i)const {	 // ע�⣺������������
		return *factor_set_vector[i];  // ���Factor������ֵ���죬Ҳ������һ���µ��ڴ�
	}	
	Factor& operator[](int i) {	 // ע�⣺������������
		return *factor_set_vector[i];  // ���Factor������ֵ���죬Ҳ������һ���µ��ڴ�
	}
	void push_factor(Factor* fp) {
		factor_set_vector.push_back(fp);
	}
};

