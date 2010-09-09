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

TextProcess::View::Impl::CViewLineBuilderImpl::CViewLineBuilderImpl(void) :
			INIT_PROPERTY(ClientArea, NULL),INIT_PROPERTY(ViewFont, NULL),INIT_PROPERTY(DocumentLineOffset, 0),INIT_PROPERTY(ViewLineOffset, 0),INIT_PROPERTY(DocumentLineManager, NULL),INIT_PROPERTY(ViewLineManager, NULL),INIT_PROPERTY(Graphics, NULL),INIT_PROPERTY(Cancel, 0)
{
}

TextProcess::View::Impl::CViewLineBuilderImpl::~CViewLineBuilderImpl(void) {
}

int TextProcess::View::Impl::CViewLineBuilderImpl::BuildLines() {
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

	int defaultLineCharSize = CalculateDefaultLineCharSize(pDocLine);

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

		InternalBuildLines(pDocLine, startOffset, endOffset, nBuildLineCount, lines);

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
				break;
			}
		}

	}

	if (GetBuilderDirection() == TextProcess::Next)
		GetViewLineManager()->HasAllNextLines();
	else
		GetViewLineManager()->HasAllPreviousLines();

	return true;
}

void TextProcess::View::Impl::CViewLineBuilderImpl::Cancel() {
	m_Cancel = 1;

	if (GetBuilderDirection() == TextProcess::Next)
		GetViewLineManager()->HasAllNextLines();
	else
		GetViewLineManager()->HasAllPreviousLines();
}

void TextProcess::View::Impl::CViewLineBuilderImpl::FixViewLineSize(
		TextProcess::Utils::Impl::wxReadOnlyString * pDocLineData,
		wxFileOffset offset, wxFileOffset & size, long & curAllViewLineWidth) {

	wxCoord width = 0;
	wxCoord height = 0;

	wxFileOffset newSize = pDocLineData->ReadOnlyResize(offset + size);

	GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());

	long last_width = width;

	do {

		if (width < curAllViewLineWidth)
			break;

		if (width - curAllViewLineWidth < GetClientArea()->GetWidth()) {

			newSize = pDocLineData->ReadOnlyResize(offset + size + 1);

			if (newSize != offset + size + 1) {
				break;
			}

			last_width = width;
			GetGraphics()->GetTextExtent(*pDocLineData, &width, &height,
					GetViewFont());

			while (!m_Cancel && (width - curAllViewLineWidth)
					< GetClientArea()->GetWidth() && (size_t) (offset + size)
					<= pDocLineData->GetPchDataLength()) {

				size++;
				newSize = pDocLineData->ReadOnlyResize(offset + size + 1);

				if (newSize != offset + size + 1)
					break;

				last_width = width;
				GetGraphics()->GetTextExtent(*pDocLineData, &width, &height,
						GetViewFont());

			}
		} else if (width - curAllViewLineWidth > GetClientArea()->GetWidth()) {

			size--;
			newSize = pDocLineData->ReadOnlyResize(offset + size);

			if (newSize != offset + size)
				break;

			last_width = width;
			GetGraphics()->GetTextExtent(*pDocLineData, &width, &height,
					GetViewFont());

			while (!m_Cancel && (width - curAllViewLineWidth)
					> GetClientArea()->GetWidth() && size > 0) {

				size--;
				newSize = pDocLineData->ReadOnlyResize(offset + size);

				if (newSize != offset + size)
					break;

				last_width = width;
				GetGraphics()->GetTextExtent(*pDocLineData, &width, &height,
						GetViewFont());

			}
		}//
	} while (false);

	curAllViewLineWidth = last_width;

	if (size <= 0)
		size = 1;
}

void TextProcess::View::Impl::CViewLineBuilderImpl::FixViewLineSize(
		TextProcess::Utils::Impl::wxReadOnlyString * pDocLineData,
		wxFileOffset offset, wxFileOffset & size) {
	wxCoord width = 0;
	wxCoord height = 0;

	wxString data = pDocLineData->substr(offset, size);

	GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());

	do {
		if (width < GetClientArea()->GetWidth()) {
			if ((size_t) (offset + size + 1) <= pDocLineData->GetPchDataLength()) {
				data = pDocLineData->substr(offset, size + 1);
				GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());

				while (!m_Cancel && width < GetClientArea()->GetWidth()) {

					size++;
					if ((size_t) (offset + size + 1) > pDocLineData->GetPchDataLength())
						break;

					data = pDocLineData->substr(offset, size + 1);
					GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());
				}
			}
		} else if (width > GetClientArea()->GetWidth()) {
			size--;

			if (size > 0) {
				data = pDocLineData->substr(offset, size);
				GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());

				while (!m_Cancel && width > GetClientArea()->GetWidth() && size > 0) {
					size--;

					if (size <= 0)
						break;

					data = pDocLineData->substr(offset, size);
					GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());
				}
			}
		}//
	} while (false);

	if (size <= 0)
		size = 1;
}

int TextProcess::View::Impl::CViewLineBuilderImpl::InternalBuildLines(
		TextProcess::Document::IDocumentLine * pDocLine, wxFileOffset startOffset,
		wxFileOffset & endOffset, wxInt32 nBuildCount, std::vector<
				TextProcess::View::IViewLine *> & lines) {
	wxChar * pBuf = NULL;
	wxFileOffset pBufLen = 0;
	long cur_all_line_width = 0;
	bool use_readonly_string = true;
	wxInt32 decodedLength = pDocLine->GetDecodedLength();
	float width = GetViewFont()->GetPointSize();

	int defaultLineCharSize = CalculateDefaultLineCharSize(pDocLine);

	pDocLine->GetData(0, decodedLength, &pBuf, &pBufLen);

	if (decodedLength > 200)
		use_readonly_string = false;

	TextProcess::Utils::Impl::wxReadOnlyString docLineData(pBuf, pBufLen);

	if (use_readonly_string && startOffset > 0) {
		docLineData.ReadOnlyResize(startOffset);

		wxCoord width, height;

		GetGraphics()->GetTextExtent(docLineData, &width, &height, GetViewFont());

		cur_all_line_width = width;
	}

	wxFileOffset viewLineOffset = startOffset;

	while (!m_Cancel && viewLineOffset < endOffset) {
		wxFileOffset viewLineSize = defaultLineCharSize;

		if (viewLineSize + viewLineOffset > decodedLength) {
			viewLineSize = decodedLength - viewLineOffset;
		}

		if (use_readonly_string) {
			FixViewLineSize(&docLineData, viewLineOffset, viewLineSize,
					cur_all_line_width);
		} else {
			FixViewLineSize(&docLineData, viewLineOffset, viewLineSize);
		}

		if (m_Cancel)
			break;

		IViewLine * pLine = CViewObjectFactory::CreateViewLine(viewLineOffset,
				viewLineSize, pDocLine);

printf("View Line:%d %d, %d \n", (int)viewLineOffset, (int)viewLineSize, (int)pDocLine->GetOffset());

		lines.push_back(pLine);

		viewLineOffset += viewLineSize;

		nBuildCount--;

		if (!nBuildCount && GetBuilderDirection() == TextProcess::Next)
			break;
	}

	endOffset = viewLineOffset;

	return 1;
}

wxInt32 TextProcess::View::Impl::CViewLineBuilderImpl::CalculateDefaultLineCharSize(
		TextProcess::Document::IDocumentLine * pDocLine) {
	wxChar * pBuf = NULL;
	wxFileOffset pBufLen = 0;
	wxInt32 length = pDocLine->GetDecodedLength();

	pDocLine->GetData(0, length, &pBuf, &pBufLen);
	TextProcess::Utils::Impl::wxReadOnlyString docLineData(pBuf, pBufLen);

	wxCoord width, height;

	if (length > 200) {
		length = 200;

		wxString data = docLineData.substr(0, length);

		GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());
	} else {
		GetGraphics()->GetTextExtent(docLineData, &width, &height, GetViewFont());
	}

	if (width == 0)
		return GetClientArea()->GetWidth() / GetViewFont()->GetPointSize();

	return GetClientArea()->GetWidth() * length / width;
}
