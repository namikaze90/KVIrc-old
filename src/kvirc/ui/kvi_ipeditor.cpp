//
//   File : kvi_ipeditor.cpp
//   Creation date : Wed Jun 12 2000 14:16:49 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_ipeditor.h"
#include "kvi_qcstring.h"

#include <QApplication>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QEvent>

#include <ctype.h>


// FIXME: #warning "THIS COULD GO INTO libkvioptions ?"

KviIpEditor::KviIpEditor(QWidget * parent,AddressType addrType,const QString &ipAddr,const char *name)
:QFrame(parent,name)
{

	for(int i=0;i<7;i++)
	{
		m_pEdit[i]  = 0;
		m_pLabel[i] = 0;
	}
	m_pEdit[7] = 0;
	setFrameStyle(QFrame::Sunken|QFrame::StyledPanel);

	setBackgroundRole(QPalette::Base);

	setAddressType(addrType);

	setAddress(ipAddr);
}

KviIpEditor::~KviIpEditor()
{
	
}

void KviIpEditor::setEnabled(bool bEnabled)
{
	QFrame::setEnabled(bEnabled);
	for(int i=0;i<8;i++)
	{
		if(m_pEdit[i])m_pEdit[i]->setEnabled(bEnabled);
		if(i<7)if(m_pLabel[i])
		{
			// Is this the right way ?
			m_pLabel[i]->setBackgroundRole(isEnabled() ? QPalette::Base : QPalette::Background);
			m_pLabel[i]->setEnabled(bEnabled);
		}
	}
	setBackgroundRole(isEnabled() ? QPalette::Base : QPalette::Background);
}

void KviIpEditor::setAddressType(AddressType addrType)
{
	if((addrType != IpV4) && (addrType != IpV6))m_addrType = IpV4;
	else m_addrType = addrType;
	recreateChildren();
}

KviIpEditor::AddressType KviIpEditor::addressType() const
{
	return m_addrType;
}

bool KviIpEditor::hasEmptyFields() const
{
	bool bHasEF = false;
	for(int i=0;i<8;i++)
	{
		if(m_pEdit[i])
		{
			if(m_pEdit[i]->text().isEmpty())bHasEF = true;
		}
	}
	return bHasEF;
}

void KviIpEditor::clear()
{
	if(!m_pEdit[0])return;
	int maxW = (m_addrType == IpV4) ? 4 : 8;
	for(int i=0;i< maxW ;i++)
	{
		m_pEdit[i]->setText("");
	}
}

bool KviIpEditor::setAddress(const QString &ipAddr)
{
	// FIXME We could check if the addres
    //       is valid before effectively setting the fields
	clear();

	KviQCString ip = ipAddr.ascii(); // ip addresses are digits & latin letters abcdef (IpV6)

	ip = ip.stripWhiteSpace();
	const char * c = ip.data();

	if(!c)return false; // huh ?....(should never happen at this point)

	if(m_addrType == IpV4)
	{
		for(int i=0;i<4;i++)
		{
			const char *anchor = c;
			while(isdigit(*c))c++;
			if(c == anchor)return false; // Invalid empty field
			KviQCString str(anchor,(c - anchor) + 1);
			bool bOk;
			int num = str.toInt(&bOk);
			if(!bOk)return false; // should never happen , but just to be sure
			if((num < 0) || (num > 255))return false; // Invalid field
			m_pEdit[i]->setText(str.data());
			if(i < 3){
				if(*c == '.')c++;
				else return false; // missing separator
			}
		}
	} else {
		for(int i=0;i<8;i++)
		{
			const char *anchor = c;
			while(isdigit(*c) || ((tolower(*c) >= 'a') && (tolower(*c) <= 'f')) || ((tolower(*c) >= 'A') && (tolower(*c) <= 'F')))c++;
			KviQCString str(anchor,(c - anchor) + 1);
			if(str.length() > 4)return false; // Too long
			m_pEdit[i]->setText(str.data());
			if(i < 7){
				if(*c == ':')c++;
				else return false; // missing separator
			}
		}
	}
	if(*c)return false; // trailing garbage (we could avoid this)
	return true;
}

QString KviIpEditor::address() const
{
	QString ret;

	if(m_addrType == IpV6)
	{
		for(int i=0;i<8;i++)
		{
			ret.append(m_pEdit[i]->text());
			if(i < 7)ret.append(":");
		}
	} else {
		for(int i=0;i<4;i++)
		{
			QString tmp = m_pEdit[i]->text();
			bool bOk;
			int num = tmp.toInt(&bOk);
			if(!bOk)num = 0;
			tmp.setNum(num);
			ret.append(tmp);
			if(i < 3)ret.append(".");
		}
	}
	return ret;
}

void KviIpEditor::recreateChildren()
{
	// A bit slow , but compact
	bool bIpV4 = (m_addrType == IpV4);
	int max = bIpV4 ? 4 : 8;
	QFontMetrics fm(font());
	//int minWidth = fm.width(bIpV4 ? "000" : "AAAA") + 4;
	for(int i=0;i<max;i++)
	{
		if(!m_pEdit[i]){
			m_pEdit[i] = new QLineEdit(this);
			m_pEdit[i]->installEventFilter(this);
			m_pEdit[i]->setFrame(false);
			m_pEdit[i]->setAlignment(Qt::AlignCenter);
		}
		//m_pEdit[i]->setMinimumWidth(minWidth);
		m_pEdit[i]->setMaxLength(bIpV4 ? 3 : 4);
		m_pEdit[i]->show();
		if(i < (max - 1))
		{
			if(!m_pLabel[i])m_pLabel[i] = new QLabel(this);
			m_pLabel[i]->setText(bIpV4 ? "." : ":");
			m_pLabel[i]->show();
			// Is this the right way ? setBackgroundMode seems to not work well
			m_pLabel[i]->setBackgroundRole(isEnabled() ? QPalette::Base : QPalette::Background);
		}
	}
	// Kill the unused widgets , if any
	if(bIpV4)
	{
		for(int i=4;i<8;i++)
		{
			if(m_pEdit[i])
			{
				delete m_pEdit[i];
				m_pEdit[i] = 0;
			}
			if(m_pLabel[i - 1])
			{
				delete m_pLabel[i - 1];
				m_pLabel[i - 1] = 0;
			}
		}
	}
	//setMinimumWidth(4 + (max * minWidth) + ((max - 1) * m_pLabel[0]->sizeHint().width()));
	setMinimumHeight(m_pLabel[0]->sizeHint().height() + 4);
	resizeEvent(0);
}

bool KviIpEditor::eventFilter(QObject * o,QEvent *e)
{
	if(o->inherits("QLineEdit"))
	{
		if(e->type() == QEvent::KeyPress)
		{
			QString s;
			// find the editor
			int edIdx = -1;
			for(int i=0;i<8;i++)
			{
				if(m_pEdit[i] == o)
				{
					edIdx = i;
					break;
				}
			}
			if(edIdx == -1)return QFrame::eventFilter(o,e); // user added QLineEdit child ?
			int edMax = (m_addrType == IpV4) ? 3 : 7;
			int cursorPos = ((QLineEdit *)o)->cursorPosition();
			switch(((QKeyEvent *)e)->key())
			{
				case Qt::Key_Right:
					s = ((QLineEdit *)o)->text();
					if(((unsigned int)cursorPos) == s.length())
					{
						if(edIdx < edMax)
						{
							m_pEdit[++edIdx]->setCursorPosition(0);
							m_pEdit[edIdx]->setFocus();
							return true;
						}
					}
				break;
				case Qt::Key_Left:
				case Qt::Key_Backspace:
					if(cursorPos == 0)
					{
						if(edIdx > 0)
						{
							s = m_pEdit[--edIdx]->text();
							m_pEdit[edIdx]->setCursorPosition(s.length());
							m_pEdit[edIdx]->setFocus();
							return true;
						}
					}
					return QFrame::eventFilter(o,e);
				break;
				case Qt::Key_End:
				case Qt::Key_Home:
				case Qt::Key_Delete:
				case Qt::Key_Tab:
					return QFrame::eventFilter(o,e);
				break;
				default:
					// a normal key (this part substitutes a QValidator)
					const char c = tolower(((QKeyEvent *)e)->ascii());
					if(m_addrType == IpV4)
					{
						if((c >= '0') && (c <= '9'))
						{
#if QT_VERSION >= 300
							if(m_pEdit[edIdx]->hasSelectedText())m_pEdit[edIdx]->cut();
#else
							if(m_pEdit[edIdx]->hasMarkedText())m_pEdit[edIdx]->cut();
#endif
							cursorPos = m_pEdit[edIdx]->cursorPosition();
							s = m_pEdit[edIdx]->text();
							s.insert(cursorPos,c);
							bool bOk = false;
							int num = s.toInt(&bOk);
							if(!bOk)return true; //should never happen , but just to be sure
							if((num < 0) || (num > 255))return true; //invalid field
							m_pEdit[edIdx]->setText(s);
							if(num > 25)
							{
								// The focus goes to the next editor
								if(edIdx < edMax)
								{
									m_pEdit[++edIdx]->setFocus();
									m_pEdit[edIdx]->selectAll();
									//m_pEdit[edIdx]->setCursorPosition(0);
									return true;
								}
							}
#if QT_VERSION >= 300
							m_pEdit[edIdx]->cursorForward(false);
#else
							m_pEdit[edIdx]->cursorRight(false);
#endif
						} else {
							if((c == '.') && (edIdx < edMax))
							{
#if QT_VERSION >= 300
								if(!m_pEdit[edIdx]->hasSelectedText())
#else
								if(!m_pEdit[edIdx]->hasMarkedText())
#endif
								{
									m_pEdit[++edIdx]->setFocus();
									m_pEdit[edIdx]->selectAll();
								}
							}
						}
					} else {
						if(  ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) )
						{
#if QT_VERSION >= 300
							if(m_pEdit[edIdx]->hasSelectedText())m_pEdit[edIdx]->cut();
#else
							if(m_pEdit[edIdx]->hasMarkedText())m_pEdit[edIdx]->cut();
#endif
							cursorPos = m_pEdit[edIdx]->cursorPosition();
							s = m_pEdit[edIdx]->text();

							if(s.length() == 4)
							{
								if((cursorPos == 4) && (edIdx < edMax))
								{
									// the char goes in the next editor
									s = c;
									m_pEdit[++edIdx]->setText(s);
									m_pEdit[edIdx]->end(false);
									m_pEdit[edIdx]->setFocus();
								} // else either no space or invalid place in the string
							} else {
								// ok .. can insert
								s.insert(cursorPos,c);
								m_pEdit[edIdx]->setText(s);
								if((s.length() == 4) && (edIdx < edMax))
								{
									// the focus now goes to the next editor
									m_pEdit[++edIdx]->setFocus();
									m_pEdit[edIdx]->selectAll();
									//m_pEdit[edIdx]->setCursorPosition(0);
								} else {
#if QT_VERSION >= 300
									m_pEdit[edIdx]->cursorForward(false);
#else
									m_pEdit[edIdx]->cursorRight(false);
#endif
								}
							}
						} else {
							if((c == ':') && (edIdx < edMax))
							{
#if QT_VERSION >= 300
								if(!m_pEdit[edIdx]->hasSelectedText())
#else
								if(!m_pEdit[edIdx]->hasMarkedText())
#endif
								{
									m_pEdit[++edIdx]->setFocus();
									m_pEdit[edIdx]->selectAll();
								}
							}
						}
					}
					return true;
				break;
			}
		}
	}
	return QFrame::eventFilter(o,e);
}

void KviIpEditor::resizeEvent(QResizeEvent *e)
{
	if(m_pEdit[0])
	{
		int maxW = (m_addrType == IpV4) ? 4 : 8;
		int labHint = m_pLabel[0]->sizeHint().width();
		int hghHint = height() - 4;
		int ediWdth = ((width() - 4) - ((maxW - 1) * labHint)) / maxW;
		int curX = 2;
		for(int i=0;i<maxW;i++)
		{
			if(i > 0)
			{
				m_pLabel[i - 1]->setGeometry(curX,2,labHint,hghHint);
				curX += labHint;
			}
			m_pEdit[i]->setGeometry(curX,2,ediWdth,hghHint);
			curX += ediWdth;
		}
	}
	if(e)QFrame::resizeEvent(e);
}

QSize KviIpEditor::sizeHint()
{
	if(m_pEdit[0])
	{
		int labHint = m_pLabel[0]->sizeHint().width();
		int hghHint = m_pEdit[0]->sizeHint().height();
		int ediHint = m_pEdit[0]->sizeHint().width();
		if(m_addrType == IpV4)return QSize((labHint * 3) + (ediHint * 4) + 4,hghHint + 4);
		else return QSize((labHint * 7) + (ediHint * 8) + 4,hghHint + 4);
	} else return QFrame::sizeHint();
}
