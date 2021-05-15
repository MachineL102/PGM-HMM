#pragma once
#include "Factor_Set.h"
#include "Variable.h"
// ��ι�������--����
namespace pgm {
	class Algorithm
	{
	public:
		// ����������ÿ���ŵ�belief�����Բ���
		Factor* Factor_Product(vector<Factor*> f_ps) {
			Factor f = *(new Factor);
			f = *f_ps[0];
			for (int i = 1; i < f_ps.size(); i++) {
				f = f * (*f_ps[i]);
			}
			return &f;
		}

		Factor_Set Sum_Product_Eliminate_Var(const Factor_Set& factor_set, int variable) {
			vector<int> include_variable;
			vector<Factor*> no_include_variable;
			int index = 0;
			Factor* factor = new Factor; // ���û�ж��幹�캯������ô()ʹ��Ĭ�Ϲ��죬����()��ʹ��Ĭ�Ϲ���
			Factor_Set result;
			for (auto p : factor_set.factor_set_vector) {
				if (find(p->scope.begin(), p->scope.end(), variable) != p->scope.end()) {
					include_variable.push_back(index);
				}
				else { no_include_variable.push_back(p); }
				index++;
			}
			*factor = factor_set[include_variable[0]];  // ��ֵ
			for (int i = 1; i < include_variable.size(); i++) {
				*factor = (*factor) * (*factor_set.factor_set_vector[include_variable[i]]);  // �Ƿ񷵻���������
			}
			(*factor).Eliminate(variable);// ��ͨ��ֵ������ֵ����ֵ�ӹܣ���������������ã������ö���϶���������ʱ����
			no_include_variable.push_back(factor);
			result.factor_set_vector = no_include_variable;
			//CTree.insert(include_variable);  // ����ͼ����Ϊ�������
			return result;
		}
		int Sum_Product_VE(const Factor_Set& factor_set, vector<int> orderd_variable_set) {
			// ���ƹ���һ������ָ����࣬�������дfactor_set�ĸ�ֵ���캯�����ͻ�������ָ��ָ��ͬһ���ڴ棬ʹ�ú��������Ƿǳ�Σ�յ�
			// �������һ����д��ֵ���캯����ÿ�θ�ֵ��������һ���ڴ棬��ôÿ��Sum_Product_Eliminate_Var�㷨�ķ��ض�Ҫ����
			// �����������shared_ptr
			// �����������ʹ��Factor_Set�����ã�ֻʹ��һ��ԭָ�룬��֤������ԭ���ݣ����Կ���const_ptr
			Factor_Set result = Sum_Product_Eliminate_Var(factor_set, orderd_variable_set[0]);  // ������û�е�ǰ��Ҫ��������������ָ�룬��ָ��ָ�����а����ñ��������ӳ˻���
			for (int variable = 1; variable < orderd_variable_set.size(); variable++) {
				result = Sum_Product_Eliminate_Var(result, orderd_variable_set[variable]); // �����˵�variable��������ʣ������
			}
			// ���������д�����������ʣ�����ӵĳ˻�
			Factor result_ = result[0]; // ����
			for (int i = 1; i < result.size(); i++) {
				result_ = result_ * result[i];
			}
			vector_scale(result_.table);
			return argmax(result_.table);
		}
	};

}

