//=============================================================================
//
//   File : aboutdialog.cpp
//   Creation date : Sun Jun 23 17:59:12 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "aboutdialog.h"
#include "abouttext.inc"

#include "kvi_defaults.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_fileutils.h"
#include "kvi_sourcesdate.h"

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QCloseEvent>


extern KviAboutDialog * g_pAboutDialog;
extern KviApp * g_pApp;
/*
"<font color=\"#FFFF00\"><b>KVIrc public releases :</b></font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">0.9.0</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 25.01.1999</font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">1.0.0 'Millennium'</font><br>\n" \
"<font size=\"2\">\"The net in your hands\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 21.12.1999</font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">2.0.0 'Phoenix'</font><br>\n" \
"<font size=\"2\">\"The client that can't make coffee\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 30.05.2000</font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">2.1.0 'Dark Star'</font><br>\n" \
"<font size=\"2\">\"The client that can't make coffee\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 30.01.2001</font><br>\n" \
"<br>\n" \
"<font color=\"#FF0000\">2.1.1 'Monolith'</font><br>\n" \
"<font size=\"2\">\"A breath of fresh net\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: 01.05.2001</font><br>\n" \
"<br> 3.0.0-xmas build: 24-12-2001\n" \
"3.0.0-beta1: 24-06-2002\n "
"<font color=\"#FF0000\">3.0.0 'Avatar'</font><br>\n" \
"<font size=\"2\">\"No slogan yet\"</font><br>\n" \
"<font size=\"2\" color=\"#808080\">Release date: Still unknown</font><br>\n" \
*/

KviAboutDialog::KviAboutDialog()
: QDialog()
{
	_tabs = new QTabWidget();
	_buttons = new QDialogButtonBox();
	QPushButton * btnOk = _buttons->addButton(__tr2qs_ctx("Close","about"),QDialogButtonBox::AcceptRole);

	setModal(false);
	setWindowTitle(__tr2qs_ctx("About KVIrc...","about"));

	QVBoxLayout * mainLayout = new QVBoxLayout();
	mainLayout->addWidget(_tabs);
	mainLayout->addWidget(_buttons);
	setLayout(mainLayout);
	debug("created");
	
	QString buffer;
	g_pApp->findImage(buffer,"kvi_splash.png");

	QPixmap pix(buffer);

	QWidget * w = new QWidget(_tabs);
	QGridLayout * g = new QGridLayout(w,2,1,4,8);

	QLabel * l = new QLabel(w);
	l->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	l->setBackgroundColor(Qt::black);
	l->setAlignment(Qt::AlignCenter);
	l->setPixmap(pix);

	g->addWidget(l,0,0);

	QString aboutString= "KVIrc <b>" KVI_VERSION " '" KVI_RELEASE_NAME "'</b><br>";
	aboutString += __tr2qs_ctx("Forged by the <b>KVIrc Development Team</b>","about");
	aboutString += "<br>";
	aboutString += __tr2qs_ctx("Sources date","about");
	aboutString += ": ";
	aboutString += KVI_SOURCES_DATE;

	l = new QLabel(aboutString,w);
	l->setAlignment(Qt::AlignCenter);
	g->addWidget(l,1,0);

	_tabs->addTab(w,__tr2qs_ctx("About","about"));
	


	w = new QWidget(_tabs);
	g = new QGridLayout(w,1,1,4,8);

	QTextEdit * v = new QTextEdit(w);
	v->setReadOnly(true);
	g->addWidget(v,0,0);

	v->setText(g_szAboutText);

	_tabs->addTab(w,__tr2qs_ctx("Honor && Glory","about"));



	w = new QWidget(_tabs);
	g = new QGridLayout(w,1,1,4,8);

	v = new QTextEdit(w);
	v->setReadOnly(true);
	v->setWordWrapMode(QTextOption::NoWrap);
	g->addWidget(v,0,0);

	QString szLicense;

	QString szLicensePath;
	g_pApp->getGlobalKvircDirectory(szLicensePath,KviApp::License,"COPYING");
	
	if(!KviFileUtils::loadFile(szLicensePath,szLicense))
	{
		szLicense = __tr2qs_ctx("Oops... Can't find the license file...\n" \
			"It MUST be included in the distribution...\n" \
			"Please report to <pragma at kvirc dot net>","about");
	}

	v->setText(szLicense);

	_tabs->addTab(w,__tr2qs_ctx("License","about"));


	connect(btnOk,SIGNAL(clicked(bool)),this,SLOT(closeButtonPressed(bool)));
}

KviAboutDialog::~KviAboutDialog()
{
	g_pAboutDialog = 0;
	delete _tabs;
	delete _buttons;
}

void KviAboutDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	delete this;
}

void KviAboutDialog::closeButtonPressed(bool checked)
{
	delete this;
}
