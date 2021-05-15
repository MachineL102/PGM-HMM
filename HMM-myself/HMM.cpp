#include "HMM.h"

void connect(Mat bgImage, cycle c1, cycle c2, double label) {
	Scalar color = Scalar(0, 0, 0);		//颜色（b, g, r）
	arrowedLine(bgImage, Point(c1.x + c1.r, c1.y), Point(c2.x - c2.r, c2.y), color, 2, 8, 0, 0.05);
	putText(bgImage, format("%.2f", label), Point((c1.x + c2.x) / 2.0 - 20, (c1.y + c2.y) / 2.0), 1.0, 1.0, Scalar(0, 0, 0), 1, 20);
}
void label_left(Mat bgImage, cycle c, double label) {
	putText(bgImage, format("%.2f", label), Point(c.x - 2 * c.r - 20, c.y + 4), 1.0, 1.0, Scalar(0, 0, 0), 1, 20);
}
void label_up(Mat bgImage, cycle c, double label) {
	putText(bgImage, format("%.2f", label), Point(c.x - c.r, c.y - 2 * c.r), 1.0, 1.0, Scalar(0, 0, 0), 1, 20);
}
void label_right(Mat bgImage, cycle c, double label) {
	if (label < 0.0) {
		circle(bgImage, Point(c.x + 5 * c.r, c.y), c.r / 2, Scalar(0, 0, 0), -1, 20);
	}
	else {
		putText(bgImage, format("%.2f", label), Point(c.x - c.r, c.y - 2 * c.r), 1.0, 1.0, Scalar(0, 0, 0), 1, 20);
	}
}
void HMM::plot(vector<int>& ObsIdxs, int len, vector<int> StaIdxs)
{
	float L = 800; float W = 400; bgImage = Mat(Size(L, W), CV_8UC3, Scalar(255, 255, 255));
	float seq_W = W / (N + 2);
	float seq_L = L / (len + 2);
	double offset = seq_L / 2;
	vector<vector<cycle>> wangge;
	wangge.resize(len);
	double x1 = seq_W; double y1 = seq_L;
	for (int i = 0; i < len; i++) {
		double cur_x = x1; double cur_y = y1;
		for (int j = 0; j < N + 1; j++) {
			cycle cur_cycle = cycle(cur_y, cur_x);
			if (j != 0) {
				cur_cycle.text = format("S:%d", j - 1);
			}
			else { cur_cycle.text = format("O:%d", ObsIdxs[i]); }
			cur_cycle.updata_text_position();
			wangge[i].push_back(cur_cycle);
			cur_x += seq_W;
		}
		y1 += seq_L;
	}
	for (auto v : wangge) {
		for (auto cyc : v) {
			circle(bgImage, Point(cyc.x, cyc.y), cyc.r, Scalar(0, 0, 0), 1, 20);
			putText(bgImage, cyc.text, cyc.text_position, 1.0, 1.0, Scalar(0, 0, 0), 1, 20);
		}
	}
	for (int i = 0; i < len - 1; i++) {
		connect(bgImage,wangge[i][StaIdxs[i] + 1], wangge[i + 1][StaIdxs[i + 1] + 1], A[StaIdxs[i]][StaIdxs[i + 1]]);
	}
	// init_probility
	for (int i = 0; i < N; i++) {
		label_left(bgImage,wangge[0][i + 1], P0[i]);
		label_right(bgImage,wangge[len - 1][i + 1], -1);
	}
	for (int i = 0; i < len; i++) {
		label_up(bgImage,wangge[i][0], B[StaIdxs[i]][ObsIdxs[i]]);
	}
	imshow("dst", bgImage);
	imwrite("s.jpg", bgImage);

	//waitKey(0);
}

HMM::HMM(int num_statas, int num_observe)
{
	N = num_statas;
	M = num_observe;
}

HMM::HMM(vector<vector<double>>& TransProb, vector<vector<double>>& ObsProb, vector<double>& InitProb)
{
	N = TransProb.size();
	M = ObsProb[0].size();
	/*
	  vector<vector<int>>  A(N,N);
	  vector<vector<int>>  B(N,M);
	  FArr1D  P0(N);
	*/
	pA = new vector<vector<double>>(TransProb);
	pB = new vector<vector<double>>(ObsProb);
	pP0 = new vector<double>(InitProb);

	A = *pA;
	B = *pB;
	P0 = *pP0;
}

HMM:: ~HMM()
{
	delete pA;
	delete pB;
	delete pP0;
	delete pbeta;
	delete palpha;
}

// Forward algorithm
double HMM::Forward(vector<int>& ObsIdxs)
{
	// 给定长度为Tmax的观测数据，输出该次观测的概率P(O|u)
	double sum = 0.0;
	Tmax = ObsIdxs.size();

	palpha = new vector<vector<double>>();
	palpha->resize(Tmax);  // 省去了初始化为0第构造函数调用
	alpha = *palpha;  // 使用数组下标访问
	// Initialization
	int t = 0;
	for (int i = 0; i < N; i++)
		alpha[0].push_back(P0[i]*B[i][ObsIdxs[t]]);

	// Forward
	for (t = 0; t < Tmax-1; t++)
	{
		for (int j = 0; j < N; j++)
		{
			sum = 0.0;
			for (int i = 0; i < N; i++) {
				sum += alpha[t][i] * A[i][j];  // 在t时刻的状态i上求和
			}
			alpha[t+1].push_back(sum * B[j][ObsIdxs[t+1]]);
		};
	}
	// Termination
	t = Tmax-1;
	sum = 0.0;
	for (int i = 0; i < N; i++)
		sum += alpha[t][i];

	return sum;
}

// Backward algorithm
double HMM::Backward(vector<int>& ObsIdxs)
{
	double sum = 0.0;
	Tmax = ObsIdxs.size();
	pbeta = new vector<vector<double>>();
	pbeta->resize(Tmax);  // 省去了初始化为0第构造函数调用
	beta = *pbeta;

	// Initialization
	int t = Tmax - 1;
	for (int i = 0; i < Tmax; i++)
		beta[i].resize(N);
	for (int i = 0; i < N; i++)
		beta[t][i] = 1.0;

	// Recursion
	for (t = Tmax - 2; t >= 0; t--)
	{
		for (int i = 0; i < N; i++)
		{
			sum = 0.0;
			for (int j = 0; j < N; j++) {
				sum += A[i][j] * B[j][ObsIdxs[t + 1]] * beta[t + 1][j];
			}
			beta[t][i] = sum;
		};
	}

	// Termination
	// B[i][ObsIdxs[t]] : 状态i发射第t种观测值的概率
	t = 0;
	sum = 0.0;
	for (int i = 0; i < N; i++)
		sum += P0[i] * B[i][ObsIdxs[t]] * beta[t][i];
	return sum;
}

// Viterbi algorithm: 求出给定观测值条件下的最优序列 P(Q|O)
vector<int> HMM::predict(vector<int>& ObsIdxs)
{
	double p, q;
	int t, k;
	vector<int> StateIdxs(Tmax);
	vector<vector<double>> delta;
	delta.resize(Tmax);
	vector<vector<int>> psi;
	psi.resize(Tmax);

	// Initialization
	t = 0;
	for (int i = 0; i < N; i++)
	{
		delta[t].push_back(P0[i] * B[i][ObsIdxs[t]]);
		psi[t].push_back(0); // Outside limits - not used
	};

	// Recursion
	for (t = 1; t < Tmax; t++)
	{
		for (int j = 0; j < N; j++)
		{
			p = 0.0;
			k = 0; // Outside limits, must be redefined below
			for (int i = 0; i < N; i++)
			{
				q = delta[t-1][i] * A[i][j]; // 最有可能在t时刻到达状态j的概率
				if (q > p)
				{
					p = q;
					k = i;
				};
			};
			delta[t].push_back(p * B[j][ObsIdxs[t]]);
			psi[t].push_back(k); // 前一时刻的哪一个状态最有可能到达状态j
		};
	};

	// Termination
	t = Tmax-1;
	p = 0.0;
	k = 0; // Outside limits, must be redefined below
	for (int i = 0; i < N; i++)
	{
		q = delta[t][i];
		if (q > p)
		{
			p = q;
			k = i;
		};
	};
	StateIdxs[t] = k;  // q* in Rabiner's paper

	// Path (state sequence) backtracking
	for (t = Tmax - 2; t >= 0; t--)
	{
		StateIdxs[t] = psi[t+1][StateIdxs[t+1]];  // t时刻使t+1时刻状态最有可能为StateIdxs[t+1]的状态
	};
	for (int i = 0; i < StateIdxs.size(); i++) {
		cout << "hidden stata:" << StateIdxs[i] << endl;
	}
	plot(ObsIdxs, 6, StateIdxs);
	return StateIdxs;
}
#define random() (rand() / double(RAND_MAX) - 0.5)
int HMM::learn(vector<vector<int>>& some_ObsIdxs, int iter = 100)
{
	for (int num_example = 0; num_example < some_ObsIdxs.size(); num_example++) {
		vector<int> ObsIdxs = some_ObsIdxs[num_example];
		// init A, B, P0
		// +=1
		double prob1 = 1.0 / N;
		double prob2 = 1.0 / M;
		vector<vector<double>> new_A;
		vector<vector<double>> new_B;
		vector<double> new_P0;
		if (num_example == 0) {
			A.resize(N, vector<double>(N, prob1));
			cout << "prob" << A[0][0] << A[0][1] << endl;
			B.resize(N, vector<double>(M, prob2));
			P0.resize(N, prob1);
			for (int i = 0; i < N;i++) {
				double s = 0.0;
				for (int j = 0; j < N; j++) {
					if (j != N - 1) {
						double tmp = random() / N;
						cout << "tmp:" << tmp << endl;
						s += tmp;
						A[i][j] += tmp;
					}
					else {
						A[i][j] -= s;
					}
				}
			}
			for (int i = 0; i < N;i++) {
				double s = 0.0;
				for (int j = 0; j < M; j++) {
					if (j != M - 1) {
						double tmp = random() / M;
						s += tmp;
						B[i][j] += tmp;
					}
					else {
						B[i][j] -= s;
					}
				}
			}
		}

		new_A.resize(N, vector<double>(N));
		new_B.resize(N, vector<double>(M));
		new_P0.resize(N);

		for (int iter_ = 0; iter_ < iter; iter_++) {
			// update: alpha, beta
			//cv::Mat NewSamples(0, A[0].size(), cv::DataType<float>::type);
			//for (unsigned int i = 0; i < A.size(); ++i)
			//{
			//	cv::Mat Sample(1, A[0].size(), cv::DataType<float>::type, A[i].data());
			//	NewSamples.push_back(Sample);
			//}
			//cout << NewSamples << endl;
			Forward(ObsIdxs);
			Backward(ObsIdxs);
			cout << "prob" << A[0][0] << A[0][1] << endl;

			// gamma
			vector<vector<double>> gamma;
			gamma.resize(Tmax);
			for (int t = 0; t < Tmax; t++) {
				double sum = 0.0;
				for (int i = 0; i < N; i++) {
					double tmp = alpha[t][i] * beta[t][i];
					sum += tmp;
					gamma[t].emplace_back(tmp);
				}
				for (size_t j = 0; j < gamma[t].size(); j++) {
					gamma[t][j] /= sum;
				}
			}

			// ksi
			vector<vector<vector<double>>> ksi;
			ksi.resize(Tmax - 1);  // t时刻状态i，t+1时刻状态为j的变量，所以Tmax-1(t:0-Tmax-2)或者(t:1-Tmax-1)
			for (int t = 0; t < Tmax - 1; t++) {
				double tmp;
				double sum = 0.0;
				ksi[t].resize(N);
				for (int i = 0; i < N; i++) {
					for (int j = 0; j < N; j++) {
						tmp = alpha[t][i] * A[i][j] * B[j][ObsIdxs[t + 1]] * beta[t + 1][j];
						sum += tmp;
						ksi[t][i].emplace_back(tmp);
					}
				}
				for (int i = 0; i < N; i++) {
					for (int j = 0; j < N; j++) {
						ksi[t][i][j] /= sum;
					}
				}
			}

			// new_A
			for (int i = 0; i < N; i++) {
				double sum_gamma = 0.0;
				for (int t = 0; t < Tmax - 1; t++) {
					sum_gamma += gamma[t][i];
				}
				for (int j = 0; j < N; j++) {
					double sum_ksi = 0.0;
					for (int t = 0; t < Tmax - 1; t++) {
						sum_ksi += ksi[t][i][j];
					}
					new_A[i][j] = sum_ksi / sum_gamma;
				}
				new_P0[i] = gamma[0][i];
			}

			// new_B
			for (int j = 0; j < N; j++) {
				for (int k = 0; k < M; k++) {
					double cond_sum_gamma = 0.0;
					double sum_gamma = 0.0;
					for (int t = 0; t < Tmax; t++) {
						if (ObsIdxs[t] == k) {
							cond_sum_gamma += gamma[t][j];
						}
						sum_gamma += gamma[t][j];
					}
					new_B[j][k] = cond_sum_gamma / sum_gamma;
				}
			}
			A = new_A;
			B = new_B;
			P0 = new_P0;
		}
		vector<int> result = predict(some_ObsIdxs[num_example]);
		cout << "Next example:" << num_example << endl;
	}
	return 0;
}




