// HMM-myself.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "PGM.h"
#include <map>


int main() {
	std::vector<std::vector<string>> data;
	open_train_data("train_data.csv", data);
	PGM pgm; //pgm.show_skeleton();
	pgm.naive_bayes_fit(data, true);
	pgm.inference("test_data.csv", "valid", true);
	//vector<vector<int>> Obs_data{ {0, 3, 3, 1, 3, 1, 0, 0, 0, 1, 3, 2, 0, 0, 3, 1, 1, 0, 1, 1, 2, 1, 0, 3, 0, 0, 1, 2, 1, 1},
	//{1, 3, 1, 0, 0, 0, 1, 3, 2,2, 1, 2, 1, 0, 3, 0,1, 1, 0, 1, 1, 2, 1, 0, 3, 0, 0, 1, 2, 1, 1} ,
	//{0, 3, 3, 1, 3, 1, 0, 0, 0, 1, 3, 2,2, 1, 2, 1, 0, 3, 0,1, 3, 0, 0, 1, 2, 1, 1} ,
	//{0, 3, 3, 1, 3, 1, 0, 0, 0, 1, 3, 2,2, 1, 2, 1, 0, 3, 0, 0,1, 0, 3, 0, 0, 1, 2, 1, 1} };
	//
	//model.learn(Obs_data, 100);

	////vector<int> Obs_data2{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1 };
	////model.predict(Obs_data2);
	return 0;
}

