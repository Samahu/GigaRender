#pragma once

class CriticalSection;
class Thread;
class Semaphore;
class Job;
interface IBoss;

class Worker
{
private:
	Thread *thread;
	Semaphore *assignmentControl;
	bool suicide;
	bool idle;
	Job *job;
	IBoss *boss;

	CriticalSection *csIdle;
	CriticalSection *csSuicide;


private:
	void setIdle(bool value);
	void setSuicide(bool value);
	static unsigned int __stdcall threadProcedure(void* parameter);
	unsigned int loop();

protected:
	virtual void perform(Job &job) = 0;

public:
	Worker(IBoss *boss, const char* name);
	virtual ~Worker();

	void Start();
	void CommitSuicide();
	bool IsSuiciding() const;
	bool IsIdle() const;
	void QuitIdle();
	// returns: true if the job was successfully assigned.
	// remarks:
	//  the function might fail for one of two reasons:
	//		- if there is an already assigned job.
	//		- if the worker was ordered to commit a suicide.
	//  if assignment was accepted then the worker will automatically quit the idle state.
	bool Assign(Job &job);
};