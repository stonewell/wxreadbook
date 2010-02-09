#ifndef __LIB_7ZIP_H__
#define __LIB_7ZIP_H__

#include <string>
#include <vector>

#ifndef _WIN32
#define __int64 long long int
typedef std::basic_string<wchar_t> wstring;
typedef std::basic_string<char> string;
#define CLASS_E_CLASSNOTAVAILABLE (0x80040111L)
#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2
#define S_OK 				 0
#else
typedef std::basic_string<wchar_t> wstring;
#endif

typedef std::vector<wstring> WStringArray;

class C7ZipObject
{
public:
	C7ZipObject() {}
	virtual ~C7ZipObject() {}
};

class C7ZipObjectPtrArray : public std::vector<C7ZipObject *>
{
public:
	C7ZipObjectPtrArray(bool auto_release = true);
	virtual ~C7ZipObjectPtrArray();

public:
	void clear();

private:
    bool m_bAutoRelease;
};

class C7ZipArchiveItem : public virtual C7ZipObject
{
public:
	C7ZipArchiveItem();
	virtual ~C7ZipArchiveItem();

public:
	virtual wstring GetFullPath() const  = 0;
	virtual bool IsDir() const  = 0;
	virtual unsigned int GetArchiveIndex() const  = 0;
};

class C7ZipInStream
{
public:
	virtual wstring GetExt() const = 0;
	virtual int Read(void *data, unsigned int size, unsigned int *processedSize) = 0;
	virtual int Seek(__int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition) = 0;
	virtual int GetSize(unsigned __int64 * size) = 0;
};

class C7ZipOutStream
{
public:
	virtual int Write(const void *data, unsigned int size, unsigned int *processedSize) = 0;
	virtual int Seek(__int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition) = 0;
	virtual int SetSize(unsigned __int64 size) = 0;
};

class C7ZipArchive : public virtual C7ZipObject
{
public:
	C7ZipArchive();
	virtual ~C7ZipArchive();

public:
	virtual bool GetItemCount(unsigned int * pNumItems) = 0;
	virtual bool GetItemInfo(unsigned int index, C7ZipArchiveItem ** ppArchiveItem) = 0;
	virtual bool Extract(unsigned int index, C7ZipOutStream * pOutStream) = 0;
	virtual bool Extract(const C7ZipArchiveItem * pArchiveItem, C7ZipOutStream * pOutStream) = 0;
	
	virtual void Close() = 0;
};

class C7ZipLibrary
{
public:
	C7ZipLibrary();
	~C7ZipLibrary();

private:
	bool m_bInitialized;

	C7ZipObjectPtrArray m_InternalObjectsArray;
protected:

public:
	bool Initialize();
	void Deinitialize();

	bool GetSupportedExts(WStringArray & exts);

	bool OpenArchive(C7ZipInStream * pInStream, C7ZipArchive ** ppArchive);

    const C7ZipObjectPtrArray & GetInternalObjectsArray() { return m_InternalObjectsArray; }

    bool IsInitialized() const { return m_bInitialized; }
};

#endif
