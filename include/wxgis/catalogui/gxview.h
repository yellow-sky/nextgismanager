/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxobject.h"



/** @class wxGxView
 * 
 * 	The base class for views.
 * 
 * 	@library{catalogui}
 */

class WXDLLIMPEXP_GIS_CLU wxGxView :
    public IView
{
public:	
	wxGxView(void);
	virtual ~wxGxView(void);
    //IView
    virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
	virtual wxString GetViewName(void) const;
	virtual wxIcon GetViewIcon(void);
	virtual void SetViewIcon(wxIcon Icon);
    virtual void Refresh(void){};    
    //wxGxView
	virtual bool Applies(wxGxSelection* const Selection);
    virtual void BeginRename(long nObjectID = wxNOT_FOUND){};
    virtual void OnShow(bool bShow){};
protected:
	wxString m_sViewName;
	wxXmlNode* m_pXmlConf;
    wxIcon m_Icon;
};

/** @class wxGxAutoRenamer
 * 
 * Begin Rename created object.
 * 
 * @library{catalogui}
*/

class wxGxAutoRenamer
{
public:
    wxGxAutoRenamer(void);
    virtual ~wxGxAutoRenamer(void);
    virtual void BeginRenameOnAdd(wxGxView* const pGxView = NULL, const wxString &sPath = wxEmptyString);
    virtual bool IsBeginRename(wxGxView* const pGxView, const wxString &sPath);
protected:
    wxGxView* m_pGxViewToRename;
    wxString m_sPathToRename;
};

int GxObjectCompareFunction(wxGxObject* const pObject1, wxGxObject* const pObject2, long sortData);
