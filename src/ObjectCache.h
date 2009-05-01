#ifndef __OBJECT_CACHE_H__
#define __OBJECT_CACHE_H__

class CReadBookView;
class CReadBookLine;
extern CReadBookLine * CreateReadBookLine(CReadBookView * pView,
							 wxInt32 nLineWidth,
							 wxInt32 nMaxAsciiCharCount,
							 wxInt32 nAvgAsciiCharWidth,
							 wxInt32 nColMargin);
extern void ReleaseReadBookLine(CReadBookLine * pLine);

class CReadBookChar;
CReadBookChar * CreateReadBookChar(CReadBookView * pView);
void ReleaseReadBookChar(CReadBookChar * pChar);

void InitObjectCache();
void ReleaseObjectCache();
#endif