// HMM-myself.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
#include "PGM.h"

int main() {
	PGM pgm;
	vector<vector<string>> data;
	vector<vector<int>> graph(4, vector<int>(4));
	data.push_back(vector<string>{"青色", "重", "长", "好瓜"});
	data.push_back(vector<string>{"青色", "重", "长", "好瓜"});
	data.push_back(vector<string>{"绿色", "重", "长", "好瓜"});
	data.push_back(vector<string>{"青色", "重", "短", "坏瓜"});
	data.push_back(vector<string>{"青色", "重", "短", "坏瓜"});
	data.push_back(vector<string>{"黑色", "轻", "长", "坏瓜"});
	data.push_back(vector<string>{"黑色", "重", "短", "好瓜"});
	data.push_back(vector<string>{"青色", "轻", "长", "好瓜"});
	data.push_back(vector<string>{"青色", "重", "长", "好瓜"});
	data.push_back(vector<string>{"绿色", "重", "长", "好瓜"});

	graph[3][0] = 1;graph[3][1] = 1;graph[3][2] = 1;
	pgm.learn_parm(data, &graph, true);
	pgm.inference(vector<int>{0, 1, 2});
	//int num_statas = 5; 
	//int num_observe = 4; 
	//HMM model(num_statas, num_observe);
	//// vector<int> Obs_data = from_file();

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

