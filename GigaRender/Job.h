#pragma once

class Patch;
class Job;

class Job
{
private:
	unsigned int id;

	Job & operator= (Job &) {}	// removes warning: [assignment operator could not be generated].

public:
	unsigned int GetID() const { return id; }

	Job();
	virtual ~Job();
};