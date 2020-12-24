#pragma once
#include "utils.h"
#include "Factor.h"
#include "Algorithm.h"
enum edge_stata
{
	is_ready, not_ready, sended
};
class Clique
{
public:
	vector<Factor*> f_ps;
	Factor* phi;
	edge_stata rs;
	Clique() { rs = not_ready;phi = NULL; }
	void init() { phi = pgm::Algorithm().Factor_Product(f_ps);}
	void SP_Message();
};

