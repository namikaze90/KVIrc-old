//mdm:
//   File : class_wizard.cpp
//   Creation date : Mon Apr 10 19:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2005 Szymon Stefanek (pragma at kvirc dot net)
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


#include "class_wizard.h"

#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"

#include <QWizard>
#include <QWizardPage>
#include <QPushButton>

/*
	@doc:   wizard
	@keyterms:
		wizard object class,
	@title:
		wizard class
	@type:
		class
	@short:
		The wizard class provides a framework for wizard dialogs.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		A wizard is an input dialog that consists of a sequence of dialog pages.It walks the user through a process step by step.[br]
	@functions:
	!fn: $addPage(<page_widget:object>,<title:string>)
	Add a page with title.<page_widget> must be a widget's object.
	!fn: $insertPage(<page_widget>,<title:string>,<index:integer>)
	Inserts a page at the position <index>. If index is -1, page will appended to the end of the wizard's page sequenze.
	!fn: $removePage(<page_widget:object>)
	Remove a page from the wizard sequence.
	!fn: $setTitle(<page>,<title:string>)
	Sets the title for page page to title text.
	!fn: $setEnabled(<page_widget>,<btn:string>,<bEnabled:boolean>)
	If enable is set to 1, page page has a btn button; otherwise page has no btn button.[br]
	btn can be Back, Next, Finish, Cancel and Help.[br]
	!fn: $setText(<btn:string>,<text:string>)
	Set the text for button btn, which can be Back, Next, Finish, Cancel and Help.[br]
	!fn: $acceptEvent()
	This function is called when the user click on the Finish button.
	!fn: $rejectEvent()
	This function is called when thie user click on the Reject button.
	!fn: $nextClickedEvent()
	This function is called when the user click on the Next button.
	!fn: $backClickedEvent()
	This function is called when thie user click on the Back button.
	@signals:
	!sg: $nextClicked()
	This signal is emitted by the default implementation of [classfnc]$nextClickedEvent[/classfnc]().
	!sg: $backClicked()
	This signal is emitted by the default implementation of [classfnc]$backClickedEvent[/classfnc]().

*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_wizard,"wizard","widget")

	KVSO_REGISTER_HANDLER(KviKvsObject_wizard,"addPage", functionaddPage)
	//KVSO_REGISTER_HANDLER(KviKvsObject_wizard,"insertPage", functioninsertPage)
	//KVSO_REGISTER_HANDLER(KviKvsObject_wizard,"removePage", functionremovePage)
	//KVSO_REGISTER_HANDLER(KviKvsObject_wizard,"setTitle", functionsetTitle)

	KVSO_REGISTER_HANDLER(KviKvsObject_wizard,"setEnabled", functionsetEnabled);
	KVSO_REGISTER_HANDLER(KviKvsObject_wizard,"setText", functionsetText);

	KVSO_REGISTER_HANDLER(KviKvsObject_wizard,"nextClickedEvent", functionnextClickedEvent);
	KVSO_REGISTER_HANDLER(KviKvsObject_wizard,"backClickedEvent", functionbackClickedEvent);


	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_wizard,"acceptEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_wizard,"rejectEvent")

KVSO_END_REGISTERCLASS(KviKvsObject_wizard)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_wizard,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_wizard)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_wizard)

KVSO_END_CONSTRUCTOR(KviKvsObject_wizard)

bool KviKvsObject_wizard::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	setObject(new KviKvsMdmWizard(parentScriptWidget(), name(),this), true);

	return true;
}

bool KviKvsObject_wizard::functionaddPage(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *ob;
	QString szLabel;
	QWizardPage * page;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs("Can't add a non-widget object"));
		return true;
	}
	page=(QWizardPage *)ob->object();
	page->setTitle(szLabel);
	((QWizard *)widget())->addPage(page);
	return true;
}

/*
bool KviKvsObject_wizard::functioninsertPage(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *ob;
	QString szLabel;
	kvs_int_t uIndex;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs("Can't add a non-widget object"));
		return true;
	}
	((QWizard *)widget())->insertPage(((QWidget *)(ob->object())),szLabel,uIndex);
	return true;
}

bool KviKvsObject_wizard::functionremovePage(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs("Not a widget object"));
		return true;
	}
	((QWizard *)widget())->removePage(((QWidget *)(ob->object())));
	return true;
}
*/

/*
bool KviKvsObject_wizard::functionsetTitle(KviKvsObjectFunctionCall *c)
{
	KviKvsObject *ob;
	QString szTitle;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("title",KVS_PT_STRING,0,szTitle)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs("Widget object required"));
		return true;
	}
	((QWizard *)widget())->setTitle(((QWidget *)(ob->object())),szTitle);
	return true;
}
*/

bool KviKvsObject_wizard::functionsetEnabled(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KviKvsObject *ob;
	kvs_hobject_t hObject;
	QString btnName;
	QWizard::WizardButton btnType;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("btn",KVS_PT_STRING,0,btnName)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!widget())return true;
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs("Widget object required"));
		return true;
	}

	if(btnName.length()==0)
	{
		c->warning(__tr2qs("Button name required"));
		return true;
	}

	if(btnName=="Back") btnType=QWizard::BackButton;
	else if(btnName=="Next") btnType=QWizard::NextButton;
	else if(btnName=="Finish") btnType=QWizard::FinishButton;
	else if(btnName=="Cancel") btnType=QWizard::CancelButton;
	else if(btnName=="Help") btnType=QWizard::HelpButton;
	else {
		c->warning(__tr2qs("Button name mismatch"));
		return true;
	}

	((QWizard *)widget())->button(btnType)->setEnabled(bEnabled);
	return true;
}

bool KviKvsObject_wizard::functionsetText(KviKvsObjectFunctionCall *c)
{
	QString szText;
	QString btnName;
	QWizard::WizardButton btnType;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("btn",KVS_PT_STRING,0,btnName)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;

	if(btnName.length()==0)
	{
		c->warning(__tr2qs("Button name required"));
		return true;
	}

	if(btnName=="Back") btnType=QWizard::BackButton;
	else if(btnName=="Next") btnType=QWizard::NextButton;
	else if(btnName=="Finish") btnType=QWizard::FinishButton;
	else if(btnName=="Cancel") btnType=QWizard::CancelButton;
	else if(btnName=="Help") btnType=QWizard::HelpButton;
	else {
		c->warning(__tr2qs("Button name mismatch"));
		return true;
	}

	((QWizard *)widget())->button(btnType)->setText(szText);
	return true;
}

bool KviKvsObject_wizard::accept()
{
	KviKvsVariantList * pParams = 0;
	callFunction(this,"acceptEvent",0,pParams);
	return true;
}

bool KviKvsObject_wizard::reject()
{
	KviKvsVariantList * pParams = 0;
	callFunction(this,"rejectEvent",0,pParams);
	return true;
}

bool KviKvsObject_wizard::functionnextClickedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("nextClicked",c);
	return true;
}

void KviKvsObject_wizard::nextClicked()
{
	KviKvsVariantList *params=0;
	callFunction(this,"nextClickedEvent",params);
}

bool KviKvsObject_wizard::functionbackClickedEvent(KviKvsObjectFunctionCall *c)
{
	emitSignal("backClicked",c);
	return true;
}

void KviKvsObject_wizard::backClicked()
{
	KviKvsVariantList *params=0;
	callFunction(this,"backClickedEvent",params);
}

KviKvsMdmWizard::KviKvsMdmWizard(QWidget * par,const char * name,KviKvsObject_wizard * parent)
:QWizard(par)
{
	this->setObjectName(name);
	m_pParentScript=parent;
	connect (this->button(QWizard::BackButton),SIGNAL(clicked()),this,SLOT(slotBackClicked()));
	connect (this->button(QWizard::NextButton),SIGNAL(clicked()),this,SLOT(slotNextClicked()));
}

void KviKvsMdmWizard::accept()
{
	if(m_pParentScript->accept())QWizard::accept();

}

void KviKvsMdmWizard::reject()
{
	if(m_pParentScript->reject())QWizard::reject();

}

void KviKvsMdmWizard::slotNextClicked()
{
	m_pParentScript->nextClicked();
}

void KviKvsMdmWizard::slotBackClicked()
{
	m_pParentScript->backClicked();
}

KviKvsMdmWizard::~KviKvsMdmWizard()
{
}
