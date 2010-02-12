#pragma once

#include "../TextProcess.h"

#ifndef _WIN32
#include <errno.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#endif

#include "../Utils/Impl/CriticalSection.h"
#include "../Utils/Impl/Event.h"
#include "../Utils/Impl/ReadWriteLock.h"
#include "../Utils/Impl/StringPool.h"
#include "../Utils/Impl/wxReadOnlyString.h"

#pragma warning(disable:4250)

#include "LineImpl.h"
#include "../Document/Impl/DocumentLineImpl.h"
#include "../View/Impl/ViewLineImpl.h"

#include "../Document/Impl/DocumentLineMatcherImpl.h"
#include "../View/Impl/ViewLineMatcherImpl.h"

#include "LineManagerImpl.h"
#include "../Document/Impl/DocumentLineManagerImpl.h"
#include "../View/Impl/ViewLineManagerImpl.h"

#include "LineBuilderImpl.h"
#include "../Document/Impl/DocumentLineBuilderImpl.h"
#include "../View/Impl/ViewLineBuilderImpl.h"

#include "../IO/Impl/MemoryMappedFile.h"

