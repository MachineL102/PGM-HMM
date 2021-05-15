#pragma once
#include "utils.h"

class HMM
{
	int N;	// 状态种类
	int M;	// 观测种类
	int Tmax;
	Mat bgImage;
	vector<vector<double>> A;		// N * N - 状态转移概率
	vector<vector<double>> B;		// N * M - 发射概率
	vector<double> P0;				// N , 每个状态的初始概率
	vector<vector<double>> alpha;	// Tmax * N		第t时刻状态为j的前向变量
	vector<vector<double>> beta;	// Tmax * N		第t时刻状态为j的后向变量

	vector<vector<double>>* pA;
	vector<vector<double>>* pB;
	vector<double>* pP0;
	vector<vector<double>>* pbeta;
	vector<vector<double>>* palpha;

public:
	HMM(int num_statas, int num_observe);
	HMM(vector<vector<double>>& TransProb, vector<vector<double>>& ObsProb, vector<double>& InitProb);
	~HMM();
	double Forward(vector<int>& ObsIdxs);
	double Backward(vector<int>& ObsIdxs);
	vector<int> predict(vector<int>& ObsIdxs);
	vector<int> PosteriorDecodingIdxs(vector<int>& ObsIdxs);
	int learn(vector<vector<int>>& ObsIdxs, int iter);
	void plot(vector<int>& ObsIdxs, int len, vector<int>);
};

