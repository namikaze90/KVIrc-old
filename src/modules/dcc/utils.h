#ifndef _UTILS_H_
#define _UTILS_H_
//
//   File : utils.h
//   Creation date : Tue Jul 23 02:54:45 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_console.h"
#include "kvi_kvs_moduleinterface.h"

#ifndef _UTILS_CPP_
	extern bool dcc_kvs_get_listen_ip_address(KviKvsModuleCommandCall *c,KviConsole * pConsole,QString &szListenIp);
#endif

#endif //_UTILS_H_
