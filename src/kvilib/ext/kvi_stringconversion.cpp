//=============================================================================
//
//   File : kvi_stringconversion.cpp
//   Creation date : Thu Oct 20 2000 14:12:21 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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




#define _KVI_STRINGCONVERSION_CPP_
#include "kvi_stringconversion.h"

#include "kvi_qstring.h"
#include <stdio.h>
#include <QStringList>

QString g_szGlobalDir;
QString g_szLocalDir;

namespace KviStringConversion
{

	void init(const QString& szGlobalDir,const QString& szLocalDir)
	{
		g_szGlobalDir=szGlobalDir;
		g_szLocalDir=szLocalDir;
	}

	void encodePath(QString& buffer)
	{
		if(!buffer.isEmpty())
		{
			if(!g_szLocalDir.isEmpty())
			{
				if(KviQString::find(buffer,g_szLocalDir)==0)
				{
					buffer.remove(0,g_szLocalDir.length());
					buffer.prepend("local://");
				}
			}
			if(!g_szGlobalDir.isEmpty())
			{
				if(KviQString::find(buffer,g_szGlobalDir)==0)
				{
					buffer.remove(0,g_szGlobalDir.length());
					buffer.prepend("global://");
				}
			}
		}
	}

	void decodePath(QString& buffer)
	{
		if(!buffer.isEmpty())
		{
			if(!g_szLocalDir.isEmpty())
			{
				if(KviQString::find(buffer,"local://")==0)
				{
					buffer.remove(0,8);
					buffer.prepend(g_szLocalDir);
				}
			}
			if(!g_szGlobalDir.isEmpty())
			{
				if(KviQString::find(buffer,"global://")==0)
				{
					buffer.remove(0,9);
					buffer.prepend(g_szGlobalDir);
				}
			}
		}
	}
	
	void encodePath(QStringList& buffer)
	{
		for ( QStringList::Iterator it = buffer.begin(); it != buffer.end(); ++it )
		{
			encodePath(*it);
		}
	}

	void decodePath(QStringList& buffer)
	{
		for ( QStringList::Iterator it = buffer.begin(); it != buffer.end(); ++it )
		{
			decodePath(*it);
		}
	}

	void toString(const bool bValue,QString &buffer)
	{
		buffer = bValue ? '1' : '0';
	}
	
	bool fromString(const QString & szValue,bool &buffer)
	{
		if(szValue.isEmpty())buffer = false;
		else buffer = !((KviQString::equalCS(szValue,"0")) || (KviQString::equalCI(szValue,"false")));
		return true;
	}

	void toString(const int iValue,QString &buffer)
	{
		buffer.setNum(iValue);
	}
	
	bool fromString(const QString &szValue,int &buffer)
	{
		bool bOk;
		buffer = szValue.toInt(&bOk);
		return bOk;
	}
	
	void toString(const unsigned int uValue,QString &buffer)
	{
		buffer.setNum(uValue);
	}
	
	bool fromString(const QString & szValue,unsigned int &buffer)
	{
		bool bOk;
		buffer= szValue.toUInt(&bOk);
		return bOk;
	}
	
	void toString(const unsigned short uValue,QString &buffer)
	{
		buffer.setNum(uValue);
	}
	
	bool fromString(const QString & szValue,unsigned short &buffer)
	{
		bool bOk;
		buffer= szValue.toUShort(&bOk);
		return bOk;
	}
	
	void toString(const QRect &rValue,QString &buffer)
	{
		buffer.sprintf("%d,%d,%d,%d",rValue.x(),rValue.y(),rValue.width(),rValue.height());
	}
	
	bool fromString(const QString & szValue,QRect &buffer)
	{
		KviQCString tmp = KviQString::toUtf8(szValue);
		const char * c = tmp.data();
		if(!c)return false;
		int l,t,w,h;
		if(sscanf(c,"%d,%d,%d,%d",&l,&t,&w,&h) != 4)return false;
		buffer.setRect(l,t,w,h);
		return true;
	}
	
	void toString(const QString &szValue,QString &buffer)
	{
		buffer = szValue;
	}
	
	bool fromString(const QString & szValue,QString &buffer)
	{
		buffer = szValue;
		return true;
	}
	
	void toString(const KviPixmap &pValue,QString &buffer)
	{
		buffer=pValue.path();
		encodePath(buffer);
	}
	
	bool fromString(const QString & szValue,KviPixmap &buffer)
	{
		QString szPath(szValue);
		decodePath(szPath);
		if(szPath.isEmpty()) {
			buffer.setNull();
			return true;
		} else {
			return buffer.load(szPath);
		}
	}
	
	void toString(const KviMsgType &mValue,QString &buffer)
	{
		buffer.sprintf("%d,%u,%u,%d,%d",mValue.m_iPixId,mValue.m_cForeColor,mValue.m_cBackColor,mValue.m_bLogEnabled,mValue.m_iLevel);
	}
	
	bool fromString(const QString & szValue,KviMsgType &buffer)
	{
		int iId,iLog,iLevel;
		unsigned int uFore,uBack;
		KviQCString tmp = KviQString::toUtf8(szValue);
		char * cx = tmp.data();
		if(!cx)return false;
		if(sscanf(cx,"%d,%u,%u,%d,%d",&iId,&uFore,&uBack,&iLog,&iLevel) != 5)return false;
		buffer = KviMsgType(buffer.m_szType,iId,uFore,uBack,iLog,iLevel);
		return true;
	}
	
	void toString(const QColor &cValue,QString &buffer)
	{
		buffer = cValue.name();
	}
	
	bool fromString(const QString & szValue,QColor &buffer)
	{
		if(szValue[0]=='#') {
			buffer.setNamedColor(szValue);
		} else {
			//compatability with kvirc3 configs
			buffer.setRed(szValue.section(',',0,0).toInt());
			buffer.setGreen(szValue.section(',',1,1).toInt());
			buffer.setBlue(szValue.section(',',2,2).toInt());
		}
		return true;
	}
	
	void toString(const QFont &fValue,QString &buffer)
	{
		QString family(fValue.family());
		buffer.sprintf("%s,%d,%d,%d",KviQString::toUtf8(family).data(),fValue.pointSize(),fValue.styleHint(),fValue.weight());
		QString options;
		if(fValue.bold())options.append('b');
		if(fValue.italic())options.append('i');
		if(fValue.underline())options.append('u');
		if(fValue.strikeOut())options.append('s');
		if(fValue.fixedPitch())options.append('f');
	
		if(!options.isEmpty())
		{
			buffer.append(',');
			buffer.append(options);
		}
	}
	
	bool fromString(const QString & szValue,QFont &buffer)
	{
		if(szValue.isEmpty()) return false;
		QString str = szValue;
		QString family,pointSize,styleHint,weight,options;
		family=KviQString::getToken(str,',');
		pointSize=KviQString::getToken(str,',');
		styleHint=KviQString::getToken(str,',');
		weight=KviQString::getToken(str,','); 
		if(!family.isEmpty())buffer.setFamily(family);
		int iPointSize,iStyleHint,iWeight;
		bool bOk;
		iPointSize = pointSize.toInt(&bOk);
		if(!bOk || (iPointSize < 0))
			iPointSize = -1;
		iStyleHint = styleHint.toInt(&bOk);
		if(!bOk || (iStyleHint < 0))
			iStyleHint=0;
		iWeight = weight.toInt(&bOk);
		if(!bOk || (iWeight < 0))
			iWeight=-1;
		buffer=QFont(family,iPointSize,iWeight);
		buffer.setStyleHint((QFont::StyleHint)iStyleHint);
		if(!str.isEmpty())
		{
			buffer.setBold(str.contains("b"));
			buffer.setItalic(str.contains("i"));
			buffer.setUnderline(str.contains("u"));
			buffer.setStrikeOut(str.contains("s"));
			buffer.setFixedPitch(str.contains("f"));
		}
		return true;
	}
	
	void toString(const QStringList &sValue,QString &buffer)
	{
		buffer = sValue.join(",\\[ITEM],");
	}
	
	bool fromString(const QString & szValue,QStringList &buffer)
	{
		buffer = szValue.split(",\\[ITEM],");
		return true;
	}
	
	void toString(const QTextCharFormat &szValue,QString &buffer)
	{
		QString tmpBuffer;
		toString(szValue.font(),buffer);
		buffer.append('/');
		toString(szValue.foreground().color(),tmpBuffer);
		buffer.append(tmpBuffer);
		buffer.append(',');
		toString(szValue.background().color(),tmpBuffer);
		buffer.append(tmpBuffer);
	}
		
	bool fromString(const QString & szValue,QTextCharFormat &buffer)
	{
		debug("from string");
		QFont font;
		debug(szValue.section('/',0,0));
		fromString(szValue.section('/',0,0),font);
		buffer.setFont(font);
		QColor color;
		debug(szValue.section('/',1,1));
		debug(szValue.section('/',1,1).section(',',0,0));
		fromString(szValue.section('/',1,1).section(',',0,0),color);
		buffer.setForeground(QBrush(color));
		debug(szValue.section('/',1,1).section(',',1,1));
		fromString(szValue.section('/',1,1).section(',',1,1),color);
		buffer.setBackground(QBrush(color));
		return true;
	}
	
	void toString(const QList<int> &szValue,QString &buffer)
	{
		buffer.clear();
		foreach(int i,szValue)
		{
			if(!buffer.isEmpty())
			{
				buffer.append(',');
			}
			KviQString::appendFormatted(buffer,"%d",i);
		}
	}
		
	bool fromString(const QString & szValue,QList<int> &buffer)
	{
		QStringList nums = szValue.split(',');
		buffer.clear();
		foreach(QString s,nums)
		{
			bool bOk;
			buffer.append(s.toInt(&bOk));
			if(!bOk) return false;
		}
		return true;
	}

}
