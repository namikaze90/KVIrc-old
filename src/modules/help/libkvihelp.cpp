//
//   File : libkvihelp.cpp
//   Creation date : Sun Aug 13 2000 03:00:00 by Szymon Stefanek
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

//#include "libkvihelp.h"

#include "kvi_module.h"


#include "helpwidget.h"
#include "helpwindow.h"
#include "index.h"
#include "kvi_app.h"

#include "kvi_frame.h"

// TODO: Qt4
#include <q3mimefactory.h>



#include <qsplitter.h>
Index        * g_pDocIndex = 0;
QList<KviHelpWidget*> * g_pHelpWidgetList = 0;
QList<KviHelpWindow*> * g_pHelpWindowList = 0;

/*
	@doc: help.search
	@type:
		command
	@title:
		help.search
	@short:
		Searches the documentation
	@syntax:
		help.search [-n] [-m] <key terms>
	@description:
		Finds the first available help browser in the current frame
		then searches the documentation for the specified <key terms>.
		If no help browser is available it creates one first:
		if the -m switch is present, the created browser is a MDI window,
		otherwise it is a static window.
		If the -n switch is present, the window creation is forced even
		if there are other help browsers already open.[br]
		The <key terms> are [b]space separated words[/b]
		to be matched inside the document body (logical AND).[br]
		This command is exported by the "help" module.
*/

/*tatic bool help_module_cmd_search(KviModule *m,KviCommand *c)
{
	ENTER_STACK_FRAME(c,"help_module_cmd_search");

	KviStr keys;
	if(!g_pZZZZZZUserParser->parseCmdFinalPart(c,keys))return false;

	if(keys.isEmpty())keys = "kvirc";

	if(!c->hasSwitch('n'))
	{
		// look for an already open help widget in this frame
		KviHelpWidget * w = (KviHelpWidget *)c->window()->frame()->child(
			"help_widget","KviHelpWidget");

		if(w)
		{
			w->doExactSearchFor(keys.ptr());
			return c->leaveStackFrame();
		}
	}

	if(c->hasSwitch('m'))
	{
		KviHelpWindow *w = new KviHelpWindow(c->window()->frame(),"Help browser");
		w->helpWidget()->doExactSearchFor(keys.ptr());
		c->window()->frame()->addWindow(w);
	} else {
		KviHelpWidget *w = new KviHelpWidget(c->window()->frame()->splitter(),
			c->window()->frame(),true);
		w->doExactSearchFor(keys.ptr());
		w->show();
	}

	return c->leaveStackFrame();
}*/
/*
static bool help_kvs_cmd_search(KviKvsModuleCommandCall * c)
{ 
}
*/
/*
	@doc: help.open
	@type:
		command
	@title:
		help.open
	@short:
		Shows a help document
	@syntax:
		help.open [-n] [-m] [document: string]
	@description:
		Finds the first available help browser in the current frame
		then opens the specified [document].
		If no [document] is specified it the documentation index is shown.
		If no help browser is available , a new one is created.
		[document] can be an absolute path or a relative one: in this case
		the document is searched in the KVIrc documentation directory.[br]
		The help browser has limited html browsing capabilities: you can
		use it to view simple html files on your filesystem.[br]
		This command is exported by the "help" module.
	@switches:
		!sw: -m | --mdi
		The created browser is a MDI window,
		otherwise it is a static window.
		!sw: -n | --new
		The window creation is forced even
		if there are other help browsers already open.[br]
*/


static bool help_kvs_cmd_open(KviKvsModuleCommandCall * c)
{ 
	QString doc,tmpDoc;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("document",KVS_PT_STRING,KVS_PF_OPTIONAL,tmpDoc)
	KVSM_PARAMETERS_END(c)
	doc=tmpDoc;
	if(doc.isEmpty())
		g_pApp->getGlobalKvircDirectory(doc,KviApp::Help,"index.html");
	else
		g_pApp->getGlobalKvircDirectory(doc,KviApp::Help,tmpDoc);

	Q3MimeSourceFactory * f = Q3MimeSourceFactory::defaultFactory();

	if(f)
	{
		if(!f->data(doc))
			g_pApp->getLocalKvircDirectory(doc,KviApp::Help,"nohelpavailable.html");
	}

	if(!c->switches()->find('n',"new"))
	{
		KviHelpWidget * w = (KviHelpWidget *)c->window()->frame()->child("help_widget","KviHelpWidget");
		if(w)
		{
			w->navigate(doc);
			return true;
		}
	}
	if(c->switches()->find('m',"mdi")) 
	{
		KviHelpWindow *w = new KviHelpWindow(c->window()->frame(),"Help browser");
		w->navigate(doc);
		c->window()->frame()->addWindow(w);
	} else {
		KviHelpWidget *w = new KviHelpWidget(c->window()->frame()->splitter(),
			c->window()->frame(),true);
		w->navigate(doc);
		w->show();
		//debug ("mostro");
	}
	return true;
}



static bool help_module_init(KviModule * m)
{
	QString szHelpDir,szDocList;
	
	g_pApp->getLocalKvircDirectory(szDocList,KviApp::Help,"help.doclist");
	g_pApp->getGlobalKvircDirectory(szHelpDir,KviApp::Help);
	
	g_pDocIndex = new Index(szHelpDir,szDocList);
	g_pDocIndex->setDocListFile(szDocList);
	
	g_pApp->getLocalKvircDirectory(szHelpDir,KviApp::Help,"help.dict");
	g_pDocIndex->setDictionaryFile(szHelpDir);
	

	
	g_pHelpWidgetList = new QList<KviHelpWidget*>;
	g_pHelpWindowList = new QList<KviHelpWindow*>;

	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",help_kvs_cmd_open);


	return true;
}

static bool help_module_cleanup(KviModule *m)
{
	if(g_pDocIndex) delete g_pDocIndex;
	qDeleteAll(*g_pHelpWidgetList);
	delete g_pHelpWidgetList;
	g_pHelpWidgetList = 0;
	qDeleteAll(*g_pHelpWindowList);
	delete g_pHelpWindowList;
	g_pHelpWindowList = 0;
	return true;
}

static bool help_module_can_unload(KviModule *m)
{
	return (g_pHelpWidgetList->isEmpty() && g_pHelpWindowList->isEmpty());
}

KVIRC_MODULE(
	"Help",                                                 // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Help browser extension",
	help_module_init,
	help_module_can_unload,
	0,
	help_module_cleanup
)
