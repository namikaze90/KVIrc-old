//
//   File : libkvitip.cpp
//   Creation date : Thu May 10 2001 13:50:11 CEST by Szymon Stefanek
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

#include "libkvitip.h"
#include "kvi_module.h"
#include "kvi_styled_controls.h"

#include "kvi_locale.h"
#include "kvi_app.h"

#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_fileutils.h"

#include <QPushButton>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QFont>
#include <QTextCodec>
#include <QDialogButtonBox>
#include <QVBoxLayout>

KviTipWindow * g_pTipWindow = 0;

KviTipWindow::KviTipWindow()
: QDialog(0)
{
	m_pConfig = 0;
	
	QDialogButtonBox * buttonBox = new QDialogButtonBox();
	buttonBox->addButton(__tr2qs(">>"),QDialogButtonBox::AcceptRole);
	buttonBox->addButton(__tr2qs("Close"),QDialogButtonBox::RejectRole)->setDefault(true);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(nextTip()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

	m_pInfoBrowser = new QTextBrowser();
	
	QVBoxLayout * mainLayout = new QVBoxLayout();

	/*
	decide at startup event in kvs to show hide/tip?
	*/
	m_pShowAtStartupCheck = new QCheckBox(__tr2qs("Show at startup"),buttonBox);
	m_pShowAtStartupCheck->setChecked(KVI_OPTION_BOOL(KviOption_boolShowTipAtStartup));
	
	mainLayout->addWidget(m_pInfoBrowser);
	mainLayout->addWidget(buttonBox);
	
	setMinimumSize(500,200);
	resize(500,200);
	move(g_pApp->desktop()->width()/2 - 250, g_pApp->desktop()->height()/2 - 100);
	
	setLayout(mainLayout);
	
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_IDEA)));

	setCaption(__tr2qs("Did you know..."));
	nextTip();
}

KviTipWindow::~KviTipWindow()
{
	if(m_pConfig)closeConfig();
}

bool KviTipWindow::openConfig(const QString& filename,bool bEnsureExists)
{
	if(m_pConfig)closeConfig();

	m_szConfigFileName = filename;

	QString buffer;
	g_pApp->getReadOnlyConfigPath(buffer,m_szConfigFileName,KviApp::ConfigPlugins,true);

	if(bEnsureExists)
	{
		if(!KviFileUtils::fileExists(buffer))return false;
	}

	m_pConfig = new KviConfig(buffer,KviConfig::Read);
	
	return true;
}

void KviTipWindow::closeConfig()
{
	QString buffer;
	g_pApp->getLocalKvircDirectory(buffer,KviApp::ConfigPlugins,m_szConfigFileName);
	m_pConfig->setSavePath(buffer);
	delete m_pConfig;
    m_pConfig = 0;
}

void KviTipWindow::nextTip()
{
	if(!m_pConfig)
	{
		KviStr szLocale = KviLocale::localeName();
		KviStr szFile;
		szFile.sprintf("libkvitip_%s.kvc",szLocale.ptr());
		if(!openConfig(szFile.ptr(),true))
		{
			szLocale.cutFromFirst('.');
			szLocale.cutFromFirst('_');
			szLocale.cutFromFirst('@');
			szFile.sprintf("libkvitip_%s.kvc",szLocale.ptr());
			if(!openConfig(szFile.ptr(),true))
			{
				openConfig("libkvitip.kvc",false);
			}
		}
	}

	unsigned int uNumTips = m_pConfig->readUIntEntry("uNumTips",0);
	unsigned int uNextTip = m_pConfig->readUIntEntry("uNextTip",0);

	KviStr tmp(KviStr::Format,"%u",uNextTip);
	QString szTip = m_pConfig->readEntry(tmp.ptr(),__tr2qs("<b>Can't find any tip... :(</b>"));

	uNextTip++;
	if(uNextTip >= uNumTips)uNextTip = 0;
	m_pConfig->writeEntry("uNextTip",uNextTip);

	QString html = "<html><body><font size=\"4\">" + szTip + "</font></body></html>";
	m_pInfoBrowser->setHtml(html);
}

void KviTipWindow::closeEvent(QCloseEvent *e)
{
	KVI_OPTION_BOOL(KviOption_boolShowTipAtStartup) = m_pShowAtStartupCheck->isChecked();
	e->ignore();
	delete this;
	g_pTipWindow = 0;
}

/*
	@doc: tip.open
	@type:
		command
	@title:
		tip.open
	@short:
		Opens the "did you know..." tip window
	@syntax:
		tip.open [tip_file_name:string]
	@description:
		Opens the "did you know..." tip window.<br>
		If <tip_file_name> is specified , that tip is used instead of
		the default tips provided with kvirc.<br>
		<tip_file_name> must be a file name with no path and must refer to a
		standard KVIrc configuration file found in the global or local
		KVIrc plugin configuration directory ($KVIrcDir/config/modules).<br>
		Once the window has been opened, the next tip avaiable in the config file is shown.<br>
		This command works even if the tip window is already opened.<br>
*/


static bool tip_kvs_cmd_open(KviKvsModuleCommandCall * c)
{ 
	QString szTipfilename;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("filename",KVS_PT_STRING,KVS_PF_OPTIONAL,szTipfilename)
	KVSM_PARAMETERS_END(c)
	if(!g_pTipWindow)g_pTipWindow = new KviTipWindow();
	if (!szTipfilename.isEmpty()) g_pTipWindow->openConfig(szTipfilename);
	g_pTipWindow->nextTip();
	g_pTipWindow->show();
	return true;
}

static bool tip_module_init(KviModule *m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",tip_kvs_cmd_open);
	return true;
}

static bool tip_module_cleanup(KviModule *m)
{
	if(g_pTipWindow)g_pTipWindow->close();
	return true;
}

static bool tip_module_can_unload(KviModule *m)
{
	return (g_pTipWindow == 0);
}

KVIRC_MODULE(
	"Tip",                                              // module name
	"1.1.0",                                                // module version
	"Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"\"Did you know...\" tip",
	tip_module_init,
	tip_module_can_unload,
	0,
	tip_module_cleanup
)

