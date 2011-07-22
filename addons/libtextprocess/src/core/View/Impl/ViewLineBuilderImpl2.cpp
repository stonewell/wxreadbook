#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#ifdef HAVE_MEMORY
#include <memory>
#endif
#else
#include <memory>
#endif

TextProcess::View::Impl::CViewLineBuilderImpl2::CViewLineBuilderImpl2(void) :
INIT_PROPERTY(ClientArea, NULL),INIT_PROPERTY(ViewFont, NULL),INIT_PROPERTY(DocumentLineOffset, 0),INIT_PROPERTY(ViewLineOffset, 0),INIT_PROPERTY(DocumentLineManager, NULL),INIT_PROPERTY(ViewLineManager, NULL),INIT_PROPERTY(Graphics, NULL),INIT_PROPERTY(Cancel, 0)
{
}

TextProcess::View::Impl::CViewLineBuilderImpl2::~CViewLineBuilderImpl2(void) {
}

int TextProcess::View::Impl::CViewLineBuilderImpl2::BuildLines() {
	std::auto_ptr<TextProcess::Document::IDocumentLineMatcher> pMatcher(
		TextProcess::Document::CDocumentObjectFactory::CreateLineMatcher(
		GetDocumentLineOffset()));
	TextProcess::Document::IDocumentLine * pDocLine =
		GetDocumentLineManager()->FindLine(pMatcher.get());

	if (pDocLine == NULL) {
		return 1;
	}

	float width = GetViewFont()->GetPointSize();
	wxFileOffset viewLineOffset = GetViewLineOffset();
	int unicodeCharWidth = 0;
	int asciiCharWidth = 0;

	int defaultLineCharSize = 
		CalculateDefaultLineCharSize(unicodeCharWidth, asciiCharWidth);

	IViewLine * pLastLine = NULL;

	wxUint32 nBuildLineCount = GetBuildLineCount();

	while (!m_Cancel && pDocLine != NULL) {
		wxFileOffset startOffset = viewLineOffset;
		wxFileOffset endOffset = pDocLine->GetDecodedLength();

		if (GetBuilderDirection() == TextProcess::Prev) {
			startOffset = viewLineOffset - defaultLineCharSize * nBuildLineCount * 2;

			if (startOffset < 0)
				startOffset = 0;

			startOffset = startOffset / defaultLineCharSize * defaultLineCharSize;

			endOffset = viewLineOffset;
		}

		std::vector<TextProcess::View::IViewLine *> lines;

		InternalBuildLines(pDocLine, 
			startOffset, endOffset, 
			nBuildLineCount, 
			lines,
			defaultLineCharSize,
			unicodeCharWidth,
			asciiCharWidth);

		if (m_Cancel) {
			std::vector<TextProcess::View::IViewLine *>::reverse_iterator it;
			for (it = lines.rbegin(); it != lines.rend(); it++) {
				pLastLine = *it;
				delete pLastLine;
			}
			break;
		}

		if (GetBuilderDirection() == TextProcess::Prev) {
			std::vector<TextProcess::View::IViewLine *>::reverse_iterator it;

			for (it = lines.rbegin(); it != lines.rend(); it++) {
				if (nBuildLineCount) {
					GetViewLineManager()->AddPrevLine(*it, pLastLine);
					pLastLine = *it;
					nBuildLineCount--;
				} else {
					TextProcess::View::IViewLine * pLine = *it;
					delete pLine;
				}
			}
		} else {
			std::vector<TextProcess::View::IViewLine *>::iterator it;

			for (it = lines.begin(); it != lines.end(); it++) {
				if (nBuildLineCount) {
					GetViewLineManager()->AddNextLine(*it, pLastLine);
					pLastLine = *it;
					nBuildLineCount--;
				} else {
					TextProcess::View::IViewLine * pLine = *it;
					delete pLine;
				}
			}
		}

		if (!nBuildLineCount) {
			if (!GetWaitForLineAccessed())
				return 2;

			while (!m_Cancel) {
				if (pLastLine->WaitForAccessing(500) != WAIT_TIMEOUT) {
					break;
				}
			}

			nBuildLineCount = GetBuildLineCount();
		}

TPL_PRINTF("pDocLine = %d, %d\n", pDocLine == NULL, m_Cancel);

		if (m_Cancel)
			break;

		if (GetBuilderDirection() == TextProcess::Next) {
			pDocLine = GetDocumentLineManager()->GetNextLine(pDocLine);
			viewLineOffset = 0;
		} else {
			pDocLine = GetDocumentLineManager()->GetPrevLine(pDocLine);

			if (pDocLine != NULL)
				viewLineOffset = pDocLine->GetDecodedLength();
		}

		if (pDocLine != NULL) {
			pDocLine->AccessLine();

			std::auto_ptr<IViewLineMatcher> pMatcher(
				CViewObjectFactory::CreateLineMatcher(pDocLine->GetOffset(), 0));
			if (GetViewLineManager()->FindLine(pMatcher.get(), 0) != NULL) {
TPL_PRINTF("pDocLine 2= %d, %d, %d\n", pDocLine == NULL, m_Cancel, (int)pDocLine->GetOffset());
				break;
			}
		}
TPL_PRINTF("pDocLine = %d\n", pDocLine == NULL);

	}

	if (GetBuilderDirection() == TextProcess::Next)
		GetViewLineManager()->HasAllNextLines();
	else
		GetViewLineManager()->HasAllPreviousLines();

	return true;
}

void TextProcess::View::Impl::CViewLineBuilderImpl2::Cancel() {
	m_Cancel = 1;

	if (GetBuilderDirection() == TextProcess::Next)
		GetViewLineManager()->HasAllNextLines();
	else
		GetViewLineManager()->HasAllPreviousLines();
}

wxCoord CalculateLineExtent(const wxChar * pBuf, 
							wxFileOffset offset, wxFileOffset size,
							int unicodeCharWidth, int asciiCharWidth)
{
	wxCoord width = 0;

	for(wxFileOffset i = 0; i < size; i++)
	{
		if ((pBuf[offset + i] & 0xFF) == pBuf[offset + i])
		{
			width += asciiCharWidth;
		}
		else
		{
			width += unicodeCharWidth;
		}
	}

	return width;
}

void TextProcess::View::Impl::CViewLineBuilderImpl2::FixViewLineSize(const wxChar * pBuf,
																	 size_t pBufLen,
																	 wxFileOffset offset, wxFileOffset & size, 
																	 int unicodeCharWidth,
																	 int asciiCharWidth)
{

	wxCoord width = CalculateLineExtent(pBuf, offset, size, unicodeCharWidth, asciiCharWidth);
	wxCoord height = 0;

	do {
		if (width < GetClientArea()->GetWidth()) {
			if ((size_t) (offset + size + 1) <= pBufLen) {
				width = CalculateLineExtent(pBuf, offset, size + 1, unicodeCharWidth, asciiCharWidth);

				while (!m_Cancel && width < GetClientArea()->GetWidth()) {
					size++;

					if ((size_t) (offset + size + 1) > pBufLen)
						break;

					width = CalculateLineExtent(pBuf, offset, size + 1, unicodeCharWidth, asciiCharWidth);
				}
			}
		} else if (width > GetClientArea()->GetWidth()) {
			size--;

			if (size > 0) {
				width = CalculateLineExtent(pBuf, offset, size, unicodeCharWidth, asciiCharWidth);

				while (!m_Cancel && width > GetClientArea()->GetWidth() && size > 0) {
					size--;

					if (size <= 0)
						break;

					width = CalculateLineExtent(pBuf, offset, size, unicodeCharWidth, asciiCharWidth);
				}
			}
		}//
	} while (false);

	if (size <= 0)
		size = 1;
}

int TextProcess::View::Impl::CViewLineBuilderImpl2::InternalBuildLines(
	TextProcess::Document::IDocumentLine * pDocLine, wxFileOffset startOffset,
	wxFileOffset & endOffset, wxInt32 nBuildCount, std::vector<
	TextProcess::View::IViewLine *> & lines,
	int defaultLineCharSize,
	int unicodeCharWidth,
	int asciiCharWidth) {
		wxChar * pBuf = NULL;
		wxFileOffset pBufLen = 0;
		bool use_readonly_string = true;
		wxInt32 decodedLength = pDocLine->GetDecodedLength();

		pDocLine->GetData(0, decodedLength, &pBuf, &pBufLen);

		wxFileOffset viewLineOffset = startOffset;

		while (!m_Cancel && viewLineOffset < endOffset) {
			wxFileOffset viewLineSize = defaultLineCharSize;

			if (viewLineSize + viewLineOffset > decodedLength) {
				viewLineSize = decodedLength - viewLineOffset;
			}

			FixViewLineSize(pBuf, decodedLength,
				viewLineOffset, viewLineSize,
				unicodeCharWidth, asciiCharWidth);

			if (m_Cancel)
				break;

			IViewLine * pLine = CViewObjectFactory::CreateViewLine(viewLineOffset,
				viewLineSize, pDocLine);
TPL_PRINTF("View Line:%d %d, %d =%d %d %d\n", (int)viewLineOffset, (int)viewLineSize, (int)pDocLine->GetOffset(), nBuildCount, (int)endOffset, (int)m_Cancel);

			lines.push_back(pLine);

			viewLineOffset += viewLineSize;

			nBuildCount--;

			if (!nBuildCount && GetBuilderDirection() == TextProcess::Next)
				break;
		}

		endOffset = viewLineOffset;

		return 1;
}

wxInt32 TextProcess::View::Impl::CViewLineBuilderImpl2::CalculateDefaultLineCharSize(
	int & unicodeCharWidth,
	int & asciiCharWidth) {
		const wxString & asciiString = wxT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./,?;'\"`!+-*|}{[]");
		const wxString & unicodeString = wxT("你我他哈哈哈哈哈萨斯大法师发声法史莱克法计算得分，。！");

		wxCoord width, height;
		GetGraphics()->GetTextExtent(asciiString, &width, &height, GetViewFont());

		if (width == 0)
			asciiCharWidth = GetViewFont()->GetPointSize();
		else
			asciiCharWidth = width / asciiString.Length();

		GetGraphics()->GetTextExtent(unicodeString, &width, &height, GetViewFont());

		if (width == 0)
			unicodeCharWidth = GetViewFont()->GetPointSize();
		else
			unicodeCharWidth = width / unicodeString.Length();

		return GetClientArea()->GetWidth() / asciiCharWidth;
}
