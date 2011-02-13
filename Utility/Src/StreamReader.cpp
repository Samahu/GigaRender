#include "stdafx.h"
#include "StreamReader.h"

StreamReader::StreamReader()
{
}

bool StreamReader::IsClosed() const
{
	return NULL == fileHandle;
}

StreamReader * StreamReader::Create(const char *fileName)
{
	// Check if the file exist first.
	HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE == fileHandle)
		return NULL;

	StreamReader *pSR = new StreamReader();
	pSR->fileHandle = fileHandle;

	return pSR;
}

StreamReader * StreamReader::Create(const wchar_t *fileName)
{
	// Check if the file exist first.
	HANDLE fileHandle = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE == fileHandle)
		return NULL;

	StreamReader *pSR = new StreamReader();
	pSR->fileHandle = fileHandle;

	return pSR;
}

StreamReader::~StreamReader()
{
	if (NULL != fileHandle)
		CloseHandle(fileHandle);
}

UINT64 StreamReader::Size() const
{
	assert(NULL != fileHandle);

	LARGE_INTEGER size;
	GetFileSizeEx(fileHandle, &size);
	return size.QuadPart;
}

unsigned int StreamReader::Read(void *buffer, int bufferSize, int elementSize, int elementsCount)
{
	assert(NULL != buffer);
	assert(bufferSize > 0);
	assert(elementSize > 0);
	assert(elementsCount > 0);
	assert(NULL != fileHandle);

	DWORD dwRead;
	BOOL result = ReadFile(fileHandle, buffer, elementSize * elementsCount, &dwRead, NULL);
	return (unsigned int)dwRead;
}

bool StreamReader::Seek(INT64 offset, SEEK_ORIGIN origin)
{
	assert(NULL != fileHandle);

	LARGE_INTEGER liMove;
	liMove.QuadPart = offset;
	return TRUE == SetFilePointerEx(fileHandle, liMove, NULL, (DWORD)origin);
}

void StreamReader::Close()
{
	if (NULL != fileHandle)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}
}