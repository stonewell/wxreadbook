#ifndef BOOKMARKSDLG_H_INCLUDED
#define BOOKMARKSDLG_H_INCLUDED


#include "wx/dialog.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/bitmap.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"
#include "wx/list.h"

class CBookMarksDlg : public wxDialog
{
public:
	CBookMarksDlg(wxWindow * pParent);
	virtual ~CBookMarksDlg();
};

#endif // BOOKMARKSDLG_H_INCLUDED
