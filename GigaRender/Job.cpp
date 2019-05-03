#include "stdafx.h"
#include "Job.h"

Job::Job()
{
	static unsigned int id_gen = 0;
	id = ++id_gen;
}

Job::~Job()
{
}