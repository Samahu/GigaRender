#pragma once

class PatchResolution
{
private:
	int value;
	int levelsCount;

public:
	int GetValue() const
	{
		return value;
	}

	int GetLevelsCount() const
	{
		return levelsCount;
	}

	int GetMaxLevel() const
	{
		return levelsCount - 1;
	}

private:
	// Private Constructor
	PatchResolution(int size, int levelsCount)
    {
        this->value = size;
        this->levelsCount = levelsCount;
    }


public:
	// Constructor Guard
	static PatchResolution* Create(int size)
    {
        int powOf2 = size;
        int ctr = 0;
        while (powOf2 > 2 && powOf2 % 2 == 0)
        {
            powOf2 /= 2;
            ++ctr;
        }

        if (2 != powOf2)
            return NULL;

        return new PatchResolution(size, ctr + 2);
    }
};