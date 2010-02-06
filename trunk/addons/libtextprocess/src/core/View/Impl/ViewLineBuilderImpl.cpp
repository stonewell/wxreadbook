#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::View::Impl::CViewLineBuilderImpl::CViewLineBuilderImpl(void) :
INIT_PROPERTY(ClientArea, NULL)
,INIT_PROPERTY(ViewFont, NULL)
,INIT_PROPERTY(DocumentLineOffset, 0)
,INIT_PROPERTY(ViewLineOffset, 0)
,INIT_PROPERTY(DocumentLineManager, NULL)
,INIT_PROPERTY(ViewLineManager, NULL)
,INIT_PROPERTY(Graphics, NULL)
{
}

TextProcess::View::Impl::CViewLineBuilderImpl::~CViewLineBuilderImpl(void)
{
}

int TextProcess::View::Impl::CViewLineBuilderImpl::BuildLines()
{
	TextProcess::Document::IDocumentLine * pDocLine =
		GetDocumentLineManager()->FindLine(TextProcess::Document::CDocumentObjectFactory::CreateLineMatcher(GetDocumentLineOffset()));

	float width = GetViewFont()->GetPixelSize().x;
	int viewLineOffset = GetViewLineOffset();

	int defaultLineCharSize = GetClientArea()->GetWidth() / width;

	IViewLine * pNextLine = NULL;
	IViewLine * pFirstDocViewLine = NULL;

	while (!m_Cancel && pDocLine != NULL)
	{
		pFirstDocViewLine = NULL;
		pNextLine = NULL;

		while (!m_Cancel && viewLineOffset < pDocLine->GetLength())
		{
			int viewLineSize = defaultLineCharSize;

			if (viewLineSize + viewLineOffset > pDocLine->GetLength())
			{
				viewLineSize = pDocLine->GetLength() - viewLineOffset;
			}

			FixViewLineSize(pDocLine, viewLineOffset, viewLineSize);

			if (m_Cancel) break;

			IViewLine * pLine =
                CViewObjectFactory::CreateViewLine(viewLineOffset, viewLineSize, pDocLine);

			if (pFirstDocViewLine == NULL && GetBuilderDirection() == TextProcess::Prev)
			{
				GetViewLineManager()->AddPrevLine(pLine, pFirstDocViewLine);
				pFirstDocViewLine = pLine;
				pNextLine = pLine;
			}
			else
			{
				GetViewLineManager()->AddNextLine(pLine, pNextLine);
				pNextLine = pLine;
			}

			viewLineOffset += viewLineSize;
		}

		if (m_Cancel) break;

		viewLineOffset = 0;

		if (GetBuilderDirection() == TextProcess::Next)
			pDocLine = GetDocumentLineManager()->GetNextLine(pDocLine);
		else
			pDocLine = GetDocumentLineManager()->GetPrevLine(pDocLine);

		if (pDocLine != NULL &&
			GetViewLineManager()->FindLine(CViewObjectFactory::CreateLineMatcher(pDocLine->GetOffset(), 0), false) != NULL)
		{
			break;
		}
	}

	GetViewLineManager()->HasAllLines();

	return true;
}

void TextProcess::View::Impl::CViewLineBuilderImpl::Cancel()
{
	m_Cancel = 1;
	GetViewLineManager()->HasAllLines();
}

void TextProcess::View::Impl::CViewLineBuilderImpl::FixViewLineSize(TextProcess::Document::IDocumentLine * pDocLine, int offset, int & size)
{
	long width = 0;
	long height = 0;

	GetGraphics()->GetTextExtent(pDocLine->GetData(offset, size), &width, &height, 0, 0, GetViewFont());

	if (width < GetClientArea()->GetWidth())
	{
		GetGraphics()->GetTextExtent(pDocLine->GetData(offset, size + 1), &width, &height, 0, 0, GetViewFont());

		while (!m_Cancel && width < GetClientArea()->GetWidth() && size <= pDocLine->GetLength())
		{
			size++;
			GetGraphics()->GetTextExtent(pDocLine->GetData(offset, size + 1), &width, &height, 0, 0, GetViewFont());
		}
	}
	else if (width > GetClientArea()->GetWidth())
	{
		size--;
		GetGraphics()->GetTextExtent(pDocLine->GetData(offset, size), &width, &height, 0, 0, GetViewFont());

		while (!m_Cancel && width > GetClientArea()->GetWidth() && size > 0)
		{
			size--;
			GetGraphics()->GetTextExtent(pDocLine->GetData(offset, size), &width, &height, 0, 0, GetViewFont());
		}
	}//if
}
