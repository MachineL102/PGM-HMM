#pragma once
#include "utils.h"

class HMM
{
	int N;	// ״̬����
	int M;	// �۲�����
	int Tmax;
	Mat bgImage;
	vector<vector<double>> A;		// N * N - ״̬ת�Ƹ���
	vector<vector<double>> B;		// N * M - �������
	vector<double> P0;				// N , ÿ��״̬�ĳ�ʼ����
	vector<vector<double>> alpha;	// Tmax * N		��tʱ��״̬Ϊj��ǰ�����
	vector<vector<double>> beta;	// Tmax * N		��tʱ��״̬Ϊj�ĺ������

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

