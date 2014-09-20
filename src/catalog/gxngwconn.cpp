/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013,2014 NextGIS
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

#include "wxgis/catalog/gxngwconn.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/json/jsonreader.h"
#include "wxgis/core/crypt.h"

#ifdef wxGIS_USE_CURL

//--------------------------------------------------------------
//class wxGxNGWWebService
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxNGWService, wxGxObjectContainer)

wxGxNGWService::wxGxNGWService(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath)
{
    m_bChildrenLoaded = false;
    m_bIsConnected = false;
    m_bIsAuthorized = false;

    wxXmlDocument doc(wxString::FromUTF8(soPath));
    if (doc.IsOk())
    {
        wxXmlNode* pRootNode = doc.GetRoot();
        if (NULL != pRootNode)
        {
            m_sURL = pRootNode->GetAttribute(wxT("url"));
            m_sLogin = pRootNode->GetAttribute(wxT("user"));
            Decrypt(pRootNode->GetAttribute(wxT("pass")), m_sPassword);
        }
    }
}

wxGxNGWService::~wxGxNGWService(void)
{
}

bool wxGxNGWService::Delete(void)
{
    Disconnect();

    bool bRet = DeleteFile(m_sPath);

    if (!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
        return false;
    }
    return true;
}

bool wxGxNGWService::Rename(const wxString &sNewName)
{
    CPLString szDirPath = CPLGetPath(m_sPath);
    CPLString szName = CPLGetBasename(m_sPath);
    CPLString szNewName(ClearExt(sNewName).mb_str(wxConvUTF8));
    CPLString szNewPath(CPLFormFilename(szDirPath, szNewName, GetExtension(m_sPath, szName)));


    if (!RenameFile(m_sPath, szNewPath))
    {
        const char* err = CPLGetLastErrorMsg();
        wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
        return false;
    }
    else
    {
        m_sPath = szNewPath;
        m_sName = sNewName;
        //change event
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    return true;
}


bool wxGxNGWService::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    bool bRet = CopyFile(m_sPath, szDestPath, pTrackCancel);

    if (!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Copy"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
        wxLogError(sErr);
        if (pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    return true;
}

bool wxGxNGWService::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    Disconnect();
    bool bRet = MoveFile(m_sPath, szDestPath, pTrackCancel);

    if (!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Move"), GetCategory().c_str(), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
        wxLogError(sErr);
        if (pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
        return false;
    }

    return true;
}

bool wxGxNGWService::ConnectToNGW()
{
    if (IsConnected())
    {
        return true;
    }

    wxGISCurl curl;
    if (!curl.IsOk())
    {
         return false;
    }

    wxString sURL = wxString::FromUTF8(m_sURL) + wxString(wxT("/login"));
    if (!sURL.StartsWith(wxT("http")))
    {
        sURL.Prepend(wxT("http://"));
    }

    wxString sPostData = wxString::Format(wxT("login=%s&password=%s"), m_sLogin.c_str(), m_sPassword.c_str());

    PERFORMRESULT res = curl.Post(sURL, sPostData);

    if(!res.IsValid)
        return false;

    m_bIsConnected = true;

    int pos;
    if((pos = res.sHead.Find(wxT("Set-Cookie"))) != wxNOT_FOUND)
    {
        m_bIsAuthorized = true;
        m_sAuthCookie = res.sHead.Right(res.sHead.Len() - 11 - pos);
        pos = m_sAuthCookie.Find(wxT("\r\n"));
        if(pos != wxNOT_FOUND)
        {
            m_sAuthCookie = m_sAuthCookie.Left(pos);
        }
    }

    return true;
}

bool wxGxNGWService::Connect(void)
{
    if(!ConnectToNGW())
        return false;

    LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectChanged);

    return true;
}

bool wxGxNGWService::Disconnect(void)
{
    if (!IsConnected())
    {
        return true;
    }

    DestroyChildren();
    wxGIS_GXCATALOG_EVENT(ObjectChanged);

    m_bChildrenLoaded = false;
    m_bIsConnected = false;

    return true;
}

bool wxGxNGWService::IsConnected()
{
    return m_bIsConnected;
}

void wxGxNGWService::Refresh(void)
{
    DestroyChildren();
    LoadChildren();
    wxGxObject::Refresh();
}

bool wxGxNGWService::HasChildren(void)
{
    LoadChildren();
    return wxGxObjectContainer::HasChildren();
}

void wxGxNGWService::LoadChildren(void)
{
    if (m_bChildrenLoaded)
        return;
    if(!m_bIsConnected)
    {
        ConnectToNGW();
        if(!m_bIsConnected)
            return;
    }


    new wxGxNGWRootResource(this, this, _("Resources"), CPLString(m_sURL.ToUTF8()));

/*    if(m_bIsAuthorized)
        new wxGxNGWRoot(this, _("Administration"), CPLString(m_sURL.ToUTF8()));*/

    m_bChildrenLoaded = true;
}

bool wxGxNGWService::CanCreate(long nDataType, long DataSubtype)
{
    return false;
}

wxGISCurl wxGxNGWService::GetCurl()
{
    wxGISCurl curl;
    curl.AppendHeader(wxT("Cookie:") + m_sAuthCookie);
    return curl;
}

wxString wxGxNGWService::GetLogin() const
{
	return m_sLogin;
}

wxString wxGxNGWService::GetPassword() const
{
	return m_sPassword;
}

//--------------------------------------------------------------
//class wxGxNGWRootResource
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxNGWRootResource, wxGxNGWResourceGroup)

wxGxNGWRootResource::wxGxNGWRootResource(wxGxNGWService *pService, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxNGWResourceGroup(pService, wxJSONValue(), oParent, soName, soPath)
{
    m_nResourceId = 0;
    m_sName = wxString(_("Resources"));
}

wxGxNGWRootResource::~wxGxNGWRootResource(void)
{
}

bool wxGxNGWRootResource::CanDelete(void)
{
	return false;
}

bool wxGxNGWRootResource::CanRename(void)
{
	return false;
}

bool wxGxNGWRootResource::CanCopy(const CPLString &szDestPath)
{
	return false;
}

bool wxGxNGWRootResource::CanMove(const CPLString &szDestPath)
{
	return false;
}

//--------------------------------------------------------------
// wxGxNGWResource
//--------------------------------------------------------------
wxGxNGWResource::wxGxNGWResource(const wxJSONValue &Data)
{
    wxJSONValue JSONResource = Data[wxT("resource")];
    m_bHasChildren = JSONResource[wxT("children")].AsBool();
    m_sDescription = JSONResource[wxT("description")].AsString();
    m_sDisplayName = JSONResource[wxT("display_name")].AsString();
    m_nResourceId = JSONResource[wxT("id")].AsInt();
    //wxArrayString m_aInterfaces;
    m_sKeyName = JSONResource[wxT("keyname")].AsString();
    m_nOwnerId = JSONResource[wxT("owner_user")].AsInt();
    const wxJSONInternalArray* pArr = JSONResource[wxT("permissions")].AsArray();
    if(pArr)
    {
        for(size_t i = 0; i < pArr->size(); ++i)
        {
            m_aPermissions.Add(pArr->operator[](i).AsString());
        }
    }
    pArr = JSONResource[wxT("scopes")].AsArray();
    if(pArr)
    {
        for(size_t i = 0; i < pArr->size(); ++i)
        {
            m_aScopes.Add(pArr->operator[](i).AsString());
        }
    }
}

wxGxNGWResource::~wxGxNGWResource()
{

}

int wxGxNGWResource::GetResourceId() const
{
	return m_nResourceId;
}

bool wxGxNGWResource::DeleteResource()
{
	wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return false;
	
	// DELETE /resource/0/child/4 0 - parentid 4 - curren id
    wxString sURL = wxString::FromUTF8(m_pService->GetPath()) + wxString::Format(wxT("/resource/%d/child/%d"), GetParentResourceId(), m_nResourceId);
    if (!sURL.StartsWith(wxT("http")))
    {
        sURL.Prepend(wxT("http://"));
    }
    PERFORMRESULT res = curl.Delete(sURL);
	
	return res.IsValid && res.nHTTPCode < 400;
}

/* create
POST /resource/0/child/ HTTP/1.1
Host: bishop.gis.to
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:32.0) Gecko/20100101 Firefox/32.0
Accept: application/json
Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate
Content-Type: application/json; charset=UTF-8
X-Requested-With: XMLHttpRequest
Referer: http://bishop.gis.to/resource/0/create?cls=resource_group
Content-Length: 118
Cookie: tkt="15e47993c5f4288c0aee9b7c06c1fcb84b03efeed6ddd5869772499f83cbbe785a944f1b3bb3d63e6714cd1bd219f01dc06114577113f0d91df2c1b568d8e07b541b5ff54!userid_type:int"; tkt="15e47993c5f4288c0aee9b7c06c1fcb84b03efeed6ddd5869772499f83cbbe785a944f1b3bb3d63e6714cd1bd219f01dc06114577113f0d91df2c1b568d8e07b541b5ff54!userid_type:int"
Connection: keep-alive
Pragma: no-cache
Cache-Control: no-cache

{"resource":{"cls":"resource_group","parent":{"id":0},"display_name":"test","keyname":"test_key","description":"qqq"}}
*/

//--------------------------------------------------------------
//class wxGxNGWResourceGroup
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxNGWResourceGroup, wxGxObjectContainer)

wxGxNGWResourceGroup::wxGxNGWResourceGroup(wxGxNGWService *pService, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath), wxGxNGWResource(Data)
{
    m_eResourceType = enumNGWResourceTypeResourceGroup;
    m_pService = pService;
    m_sName = m_sDisplayName;
    m_bChildrenLoaded = false;
	m_bHasGeoJSON =  NULL != GetOGRCompatibleDriverByName(GetDriverByType(enumGISFeatureDataset, enumVecGeoJSON).mb_str());
}

wxGxNGWResourceGroup::~wxGxNGWResourceGroup()
{

}

wxGISEnumNGWResourcesType wxGxNGWResourceGroup::GetType(const wxJSONValue &Data) const
{
    wxString sType = Data[wxT("resource")][wxT("cls")].AsString();
    wxGISEnumNGWResourcesType eType = enumNGWResourceTypeNone;
    if(sType.IsSameAs(wxT("resource_group")))
        eType = enumNGWResourceTypeResourceGroup;
    else if(sType.IsSameAs(wxT("postgis_layer")))
        eType = enumNGWResourceTypePostgisLayer;
    else if(sType.IsSameAs(wxT("wmsserver_service")))
        eType = enumNGWResourceTypeWMSServerService;
    else if(sType.IsSameAs(wxT("baselayers")))
        eType = enumNGWResourceTypeBaseLayers;
    else if(sType.IsSameAs(wxT("postgis_connection")))
        eType = enumNGWResourceTypePostgisConnection;
    else if(sType.IsSameAs(wxT("webmap")))
        eType = enumNGWResourceTypeWebMap;
    else if(sType.IsSameAs(wxT("wfsserver_service")))
        eType = enumNGWResourceTypeWFSServerService;
	else if(sType.IsSameAs(wxT("vector_layer")))
		eType = enumNGWResourceTypeVectorLayer;

    return eType;
}

void wxGxNGWResourceGroup::AddResource(const wxJSONValue &Data)
{
    wxGISEnumNGWResourcesType eType = GetType(Data);

    switch(eType)
    {
    case enumNGWResourceTypeResourceGroup:
        new wxGxNGWResourceGroup(m_pService, Data, this, wxEmptyString, m_sPath);
        break;
    case enumNGWResourceTypePostgisLayer:
		if(m_bHasGeoJSON)
			new wxGxNGWLayer(m_pService, enumNGWResourceTypePostgisLayer, Data, this, wxEmptyString, m_sPath);
        break;
    case enumNGWResourceTypeVectorLayer:
        if(m_bHasGeoJSON)
			new wxGxNGWLayer(m_pService, enumNGWResourceTypeVectorLayer, Data, this, wxEmptyString, m_sPath);
        break;
    }
}

bool wxGxNGWResourceGroup::HasChildren(void)
{
    LoadChildren();
    return wxGxObjectContainer::HasChildren();
}

void wxGxNGWResourceGroup::Refresh(void)
{
    DestroyChildren();
    LoadChildren();
    wxGxObject::Refresh();
}

void wxGxNGWResourceGroup::LoadChildren(void)
{
    if (m_bChildrenLoaded)
        return;

    wxGISCurl curl = m_pService->GetCurl();
    if(!curl.IsOk())
        return;

    wxString sURL = wxString::FromUTF8(m_sPath) + wxString::Format(wxT("/resource/%d/child/"), m_nResourceId);
    if (!sURL.StartsWith(wxT("http")))
    {
        sURL.Prepend(wxT("http://"));
    }
    PERFORMRESULT res = curl.Get(sURL);

    wxJSONReader reader;
    wxJSONValue  JSONRoot;
    int numErrors = reader.Parse(res.sBody, &JSONRoot);
    if (numErrors > 0)  {
        const wxArrayString& errors = reader.GetErrors();
        wxString sErrMsg(_("GeoJSON parsing error"));
        for (size_t i = 0; i < errors.GetCount(); ++i)
        {
            wxString sErr = errors[i];
            sErrMsg.Append(wxT("\n"));
            sErrMsg.Append(wxString::Format(wxT("%ld. %s"), i, sErr.c_str()));
        }
        wxLogError(sErrMsg);
        return;
    }

    wxLogDebug(res.sBody);

    m_bChildrenLoaded = true;

    const wxJSONInternalArray* pArr = JSONRoot.AsArray();
    if(pArr)
    {
        for(size_t i = 0; i < pArr->size(); ++i)
        {
            wxJSONValue JSONVal = pArr->operator[](i);
            AddResource(JSONVal);
        }
    }
}

int wxGxNGWResourceGroup::GetParentResourceId() const
{
	wxGxNGWResource* pParentResource = dynamic_cast<wxGxNGWResource*>(m_oParent);
	if(NULL == pParentResource)
		return wxNOT_FOUND;
	return pParentResource->GetResourceId();
}

//--------------------------------------------------------------
//class wxGxNGWLayer
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxNGWLayer, wxGxFeatureDataset)

wxGxNGWLayer::wxGxNGWLayer(wxGxNGWService *pService, wxGISEnumNGWResourcesType eType, const wxJSONValue &Data, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFeatureDataset(enumVecGeoJSON, oParent, soName, soPath), wxGxNGWResource(Data)
{
    m_eResourceType = eType;
    m_pService = pService;
    m_sName = m_sDisplayName;
}

wxGxNGWLayer::~wxGxNGWLayer()
{

}

wxString wxGxNGWLayer::GetCategory(void) const
{ 
	switch(m_eType)
	{
		case enumNGWResourceTypeVectorLayer:
			return wxString(_("NGW vector layer")); 
		case enumNGWResourceTypePostgisLayer:
			return wxString(_("NGW PostGIS layer")); 
		default:
			return wxEmptyString;
	}
}

wxGISDataset* const wxGxNGWLayer::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
		wxString sURL = wxString::FromUTF8(m_sPath) + wxString::Format(wxT("/resource/%d/geojson/"), m_nResourceId);
		if (!sURL.StartsWith(wxT("http")))
		{
			sURL.Prepend(wxT("http://"));
		}

		wxString sAuth = m_pService->GetLogin() + wxT(":") + m_pService->GetPassword();
		CPLSetConfigOption("GDAL_HTTP_USERPWD", sAuth.mb_str());
        wxGISFeatureDataset* pDSet = new wxGISFeatureDatasetCached(CPLString(sURL.ToUTF8()), m_eType);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}


int wxGxNGWLayer::GetParentResourceId() const
{
	wxGxNGWResource* pParentResource = dynamic_cast<wxGxNGWResource*>(m_oParent);
	if(NULL == pParentResource)
		return wxNOT_FOUND;
	return pParentResource->GetResourceId();
}

#endif // wxGIS_USE_CURL