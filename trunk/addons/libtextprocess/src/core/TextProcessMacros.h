#pragma once

#define DECLARE_PROPERTY_GETTER(t,p) \
	virtual t Get##p() const = 0;

#define DECLARE_PROPERTY_SETTER(t,p) \
	virtual void Set##p(t value) = 0;

#define DECLARE_PROPERTY(t, p) \
	DECLARE_PROPERTY_GETTER(t,p);\
	DECLARE_PROPERTY_SETTER(t,p);

#define IMPL_PROPERTY_GETTER(t, p) \
	virtual t Get##p() const \
	{ \
		return m_##p; \
	}

#define IMPL_PROPERTY_SETTER(t, p) \
	virtual void Set##p(t value) \
	{ \
		m_##p = value; \
	}

#define IMPL_PROPERTY_FIELD(t, p) \
	t m_##p;

#define IMPL_PROPERTY(t, p) \
	private: \
	IMPL_PROPERTY_FIELD(t,p); \
	public : \
	IMPL_PROPERTY_GETTER(t,p); \
	IMPL_PROPERTY_SETTER(t,p);

#define INIT_PROPERTY(p, v) \
	m_##p(v)

#define DECLARE_TPL_INTERFACE_VIRTUAL_DESTRUCTOR(a) \
	virtual ##a

#define DECLARE_TPL_INTERFACE(t) \
	t() {} \
	virtual ~t() {}

#ifndef _WIN32

#define WAIT_OBJECT_0 (0)
#define WAIT_TIMEOUT (1)
#define WAIT_ABANDONED (2)
#define INFINITE (-1)

#endif

#ifdef _DEBUG
#define TPL_PRINTF printf
#define TPL_WPRINTF wprintf
#else
#define TPL_PRINTF(...)
#define TPL_WPRINTF(...)
#endif
