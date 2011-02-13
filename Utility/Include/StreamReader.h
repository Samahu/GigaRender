#pragma once

class StreamReader
{
private:
	HANDLE fileHandle;	// when using windows implementation remove stdio.h include from stdafx.h

private:
	StreamReader();

public:
	enum SEEK_ORIGIN
	{
		SeekBegin = 0,
		SeekCurrent,
		SeekEnd
	};

	bool IsClosed() const;

public:
	static StreamReader * Create(const char* fileName);
	static StreamReader * Create(const wchar_t* fileName);
	~StreamReader();

	UINT64 Size() const;
	unsigned int Read(void* buffer, int bufferSize, int elementSize, int elementsCount);
	bool Seek(INT64 offset, SEEK_ORIGIN origin);
	void Close();
};