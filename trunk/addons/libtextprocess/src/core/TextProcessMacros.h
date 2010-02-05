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
