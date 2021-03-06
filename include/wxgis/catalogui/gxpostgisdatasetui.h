/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDatasetUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Dmitry Baryshnikov
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

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/catalog/gxpostgisdataset.h"

/** @class wxGxPostGISTableUI

    A PostGIS Table Dataset GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxPostGISTableUI :
	public wxGxPostGISTable,
	public IGxObjectUI,
	public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxPostGISTableUI)
public:
	wxGxPostGISTableUI(int nRemoteId, const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxPostGISTableUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxPostGISTable.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};
	//IGxObjectEditUI	
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

/** @class wxGxPostGISFeatureDatasetUI

    A PostGIS Table Dataset GxObjectUI.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxPostGISFeatureDatasetUI :
	public wxGxPostGISFeatureDataset,
	public IGxObjectUI,
	public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxPostGISFeatureDatasetUI)
public:
	wxGxPostGISFeatureDatasetUI(int nRemoteId, const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon &LargeIcon = wxNullIcon, const wxIcon &SmallIcon = wxNullIcon);
	virtual ~wxGxPostGISFeatureDatasetUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxPostGISFeatureDataset.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxEmptyString;};
	//IGxObjectEditUI
	virtual wxArrayString GetPropertyPages() const;
	virtual bool HasPropertyPages(void) const;
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

#endif //wxGIS_USE_POSTGRES


