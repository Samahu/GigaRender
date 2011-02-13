#pragma once

class PatchID
{
private:
	int i, j;

public:
	int GetI() const { return i; }
	int GetJ() const { return j; }

public:
	PatchID()
	{
		i = j = 0;
	}
	PatchID(int i, int j)
		:i(i), j(j)
	{
	}

	bool operator== (const PatchID & patchID) const
	{
		return i == patchID.i && j == patchID.j;
	}
};