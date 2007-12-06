//=============================================================================
//
//   File : kvi_avatar.cpp
//   Creation date : Fri Dec 01 2000 13:58:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================



#include "kvi_avatar.h"
#include "kvi_qstring.h"

#include <QImage>


/*
	@doc: ctcp_avatar
	@title:
		The AVATAR idea
	@short:
		Extending IRC fun: the AVATAR idea
	@body:
		[big]Introduction[/big]
		Starting from version 3.0.0 KVIrc supports the AVATAR protocol.
		The AVATAR term dictionary definitions include:[br]
		- Hindu mythology incarnation of a God[br]
		- Embodiment of a concept or philosophy[br]
		- In [b]cyberspace communities[/b], the rappresentation of an
		user in a shared virtual reality.[br]
		The AVATAR protocol attempts to improve the IRC communication
		by adding a method for associating a graphical rappresentation
		to an IRC user.[br]
		Since this may involve binary data transfers between users,
		the protocol is intended to be client based.
		[big]The terms[/big]
		The user that wants to offer a digital rappresentation of himself
		will be called "source user". The ones that will receive the notification
		will be called "target users".
		Every irc user can be either a source user or target user.
		[big]The idea[/big]
		Every irc user has a client-side property called AVATAR. Let's say that there
		are two users: A and B.[br]
		When user A wants to see the B's avatar he simply sends a CTCP AVATAR request
		to B (the request is sent through a PRIVMSG irc command).[br]
		User B replies with a CTCP AVATAR notification (sent through a NOTICE irc command)
		with the name or url of his avatar.[br]
		The actual syntax for the notification is:[br]
		[b]AVATAR <avatar_file> [<filesize>][/b]
		The <avatar_file> may be either the name of a B's local image file or an url
		pointing to an image on some web server.[br]
		The optional <filesize> parameter is sent only if <avatar_file> is
		stored on the B's machine and there will be more info on that later.[br]
		Anyway, after A has received the notification he tries to locate the avatar
		file in its local cache (yes, <filesize> may help here and more on this later).
		If the file can be found
		and loaded then it is simply displayed in some way near the B's nickname
		otherwise A must download the avatar from some place.
		If the <avatar_file> contains a leading url prefix (http://) then
		A fetches the image from the specified url and after that displays
		it near the B's nickname. If the <avatar_file> does not contain the
		leading url prefix then it is assumed that B offers this file for
		downloading via DCC from his machine. In this case A may also avoid
		requesting the file if the <filesize> is too large and the transfer
		would occupy too much bandwidth (for example).
		The DCC download is initiated by issuing a DCC GET <avatar_file> request to B.
		B may then reply with a standard DCC SEND or a DCC RSEND (kvirc's extension).[br]
		The implementation of the DCC GET protocol is defined by other documents here around :).[br]
		[br]
		The CTCP AVATAR messages can be sent to a single user , a set of users or a channel:
		this depends only on the source user and how many clients he wants to reach.
		[br]
		There should be a convention on the image sizes: not a protocol limit.
		For example, the convention could be that all the images should be smaller than
		129x129 pixels. The preferred image format is "png" (Portable Network Graphics)
		since it allows good compression rates without compromising the image quality.
		Other formats may be accepted as well (Theoretically this protocol could be
		extended to allow movies or 3D images).
		The "preferred" image size may grow with time, as the network transmission speed grows.
*/

KviAvatar::KviAvatar(const QString &szLocalPath,const QString &szName,QPixmap * pix)
{
	m_pPixmap = pix;
	m_pScaledPixmap = 0;
	if(m_pPixmap == 0)m_pPixmap = new QPixmap(32,32); // cool memory map :)

	m_bRemote = KviQString::equalCIN("http://",szName,7);

	m_szLocalPath = szLocalPath;
	m_szName = szName;
}

KviAvatar::~KviAvatar()
{
	delete m_pPixmap;
	if(m_pScaledPixmap)delete m_pScaledPixmap;
}

QPixmap * KviAvatar::scaledPixmap(unsigned int w,unsigned int h)
{
	if(((unsigned int)(m_pPixmap->width())) == w)
	{
		if(((unsigned int)(m_pPixmap->height())) == h)
			return m_pPixmap;
	}

	if(m_pScaledPixmap)
	{
		if((m_uLastScaleWidth == w) && (m_uLastScaleHeight == h))return m_pScaledPixmap;
		delete m_pScaledPixmap;
		m_pScaledPixmap = 0;
	}

	int curW = m_pPixmap->width();
	int curH = m_pPixmap->height();

	if(curW < 1)curW = 1;
	if(curH < 1)curH = 1;

	m_uLastScaleWidth = w;
	m_uLastScaleHeight = h;

	int scaleW = w;
	int scaleH;

	/* We want to maintain the aspect of the image instead simply set
	   height and width. The first step is trying to adapt the image size
	   by "w" vaule */
	
	scaleH = (curH * scaleW) / curW;
	
	/* Now check the resized image size. If it is too wide or too tall,
	   resize it again by "h" value */
	if(scaleH > h) {
		scaleH = h;
		scaleW = (scaleH * curW) / curH;
	}

	m_pScaledPixmap = new QPixmap(m_pPixmap->scaled(scaleW,scaleH));

	return m_pScaledPixmap;
}
