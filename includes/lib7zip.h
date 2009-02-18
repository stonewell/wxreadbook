#ifndef __LIB_7ZIP_H__
#define __LIB_7ZIP_H__

#include <string>
#include <vector>


#ifndef _WIN32
#define __int64 long long int
typedef std::basic_string<wchar_t> wstring;
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
	C7ZipObjectPtrArray();
	~C7ZipObjectPtrArray();

public:
	void clear();
};

class C7ZipArchiveItem : public virtual C7ZipObject
{
public:
	C7ZipArchiveItem();
	virtual ~C7ZipArchiveItem();

public:
	virtual wstring GetFullPath() const;
	virtual bool IsDir() const;
	virtual unsigned int GetArchiveIndex() const;
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
	virtual bool GetItemCount(unsigned int * pNumItems);
	virtual bool GetItemInfo(unsigned int index, C7ZipArchiveItem ** ppArchiveItem);
	virtual bool Extract(unsigned int index, C7ZipOutStream * pOutStream);
	virtual bool Extract(const C7ZipArchiveItem * pArchiveItem, C7ZipOutStream * pOutStream);
	
	virtual void Close();
};

class C7ZipLibrary
{
public:
	C7ZipLibrary();
	~C7ZipLibrary();

private:
	void * m_h7zipHandler;
	bool m_bInitialized;

	unsigned char m_pLibData[sizeof(void *) * 7];

	C7ZipObjectPtrArray m_CodecInfoArray;

protected:

public:
	bool Initialize();
	void Deinitialize();

	bool GetSupportedExts(WStringArray & exts);

#ifdef _WIN32
	bool OpenArchive(const wstring & filepath, C7ZipArchive ** ppArchive);
#endif
	bool OpenArchive(C7ZipInStream * pInStream, C7ZipArchive ** ppArchive);
};

#endif
