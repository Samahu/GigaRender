#pragma once

class Worker;
class Job;

interface IBoss
{
	friend Worker;

protected:
	virtual void OnWorkerDone(Worker &worker, Job &job) = 0;
	virtual void OnWorkerGoingIdle(Worker &worker) = 0;
};