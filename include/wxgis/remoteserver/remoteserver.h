/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  remote server header.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
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

#include "wxgis/catalog/catalog.h"
/*#include "wxgis/networking/networking.h"

/** \class INetCallback networking.h
    \brief The network callback class.
*//*
class INetCallback
{
public:
    virtual ~INetCallback(void){};
	//pure virtual
    virtual void OnConnect(void) = 0;
    virtual void OnDisconnect(void) = 0;
    virtual void PutInMessage(WXGISMSG msg) = 0;
};
*/