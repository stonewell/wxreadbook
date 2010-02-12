#pragma once

#include <wx/wx.h>

#include "TextProcessMacros.h"

#include "./IO/IMemoryMappedFile.h"

#include "ILine.h"
#include "./Document/IDocumentLine.h"
#include "./View/IViewLine.h"

#include "ILineMatcher.h"
#include "./Document/IDocumentLineMatcher.h"
#include "./View/IViewLineMatcher.h"

#include "./Document/IDocumentLineManager.h"
#include "./View/IViewLineManager.h"

#include "./ILineBuilder.h"
#include "./Document/IDocumentLineBuilder.h"
#include "./View/IViewLineBuilder.h"

#include "./Document/DocumentObjectFactory.h"
#include "./View/ViewObjectFactory.h"

#include "./Utils/ICriticalSection.h"
#include "./Utils/IEvent.h"
#include "./Utils/IReadWriteLock.h"

