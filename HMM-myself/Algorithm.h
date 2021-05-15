#pragma once
#include "Factor_Set.h"
#include "Variable.h"
// 如何构建团树--看书
namespace pgm {
	class Algorithm
	{
	public:
		// 设置团树中每个团的belief，所以不能
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
			Factor* factor = new Factor; // 如果没有定义构造函数，那么()使用默认构造，不加()不使用默认构造
			Factor_Set result;
			for (auto p : factor_set.factor_set_vector) {
				if (find(p->scope.begin(), p->scope.end(), variable) != p->scope.end()) {
					include_variable.push_back(index);
				}
				else { no_include_variable.push_back(p); }
				index++;
			}
			*factor = factor_set[include_variable[0]];  // 此时考虑是需要一份新的factor内存数据，因为要保留原有，所以运算符[]不能返回指针或引用
			for (int i = 1; i < include_variable.size()-1; i++) {
				*factor = move((*factor) * (*factor_set.factor_set_vector[include_variable[i]]));  // 是否返回引用类型
			}
			(*factor).Eliminate(variable);// 普通赋值便是右值被左值接管，如果函数返回引用，则引用对象肯定不能是临时对象
			no_include_variable.push_back(factor);
			result.factor_set_vector = no_include_variable;
			//CTree.insert(include_variable);  // 聚类图顶点为多个因子
			return result;
		}
		Factor Sum_Product_VE(const Factor_Set& factor_set, vector<int> orderd_variable_set) {
			// 复制构造一个带有指针的类，如果不重写factor_set的赋值构造函数，就会有两个指针指向同一块内存，使用和析构都是非常危险的
			// 解决方案一：重写赋值构造函数，每次赋值重新申请一块内存，那么每次Sum_Product_Eliminate_Var算法的返回都要申请
			// 解决方案二：shared_ptr
			// 解决方案三：使用Factor_Set的引用，只使用一份原指针，保证不更改原数据，可以考虑const_ptr
			Factor_Set result = Sum_Product_Eliminate_Var(factor_set, orderd_variable_set[0]);  // 复制了没有当前需要消除变量的因子指针，新指针指向所有包含该变量的因子乘积。
			for (int variable = 1; variable < orderd_variable_set.size(); variable++) {
				result = Sum_Product_Eliminate_Var(result, orderd_variable_set[variable]); // 消除了第variable个变量的剩余因子
			}
			// 消除了所有待消除变量的剩余因子的乘积
			Factor result_ = result[0];
			for (int i = 1; i < result.size(); i++) {
				result_ = result_ * result[i];
			}
			vector_scale(result_.table);
			print_matrix(result_.table, result_.card[0]);
			return result_;
		}
	};

}

