/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFileUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2014 Dmitry Baryshnikov
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#pragma once

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxfile.h"

/** @class wxGxFile

    A file GxObject.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxFileUI :
	public wxGxFile,
	public IGxObjectUI
{
    DECLARE_CLASS(wxGxFileUI)
public:
	wxGxFileUI(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxFileUI(void);
};

/** @class wxGxPrjFileUI

    A proj file GxObject.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxPrjFileUI :
    public wxGxPrjFile,
	public IGxObjectUI,
	public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxPrjFileUI)
public:
	wxGxPrjFileUI(wxGISEnumPrjFileType eType, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxPrjFileUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxPrjFile.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxPrjFile.NewMenu"));};
	//IGxObjectEditUI	
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
protected:
    wxIcon m_oLargeIcon;
    wxIcon m_oSmallIcon;
};

/** @class wxGxTextFileUI

    A text file GxObject.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxTextFileUI :
    public wxGxTextFile
{
    DECLARE_CLASS(wxGxTextFileUI)
public:
	wxGxTextFileUI(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxTextFileUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxTextFile.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxTextFile.NewMenu"));};
protected:
    wxIcon m_oLargeIcon;
    wxIcon m_oSmallIcon;
};
