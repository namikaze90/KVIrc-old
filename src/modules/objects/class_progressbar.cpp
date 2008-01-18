//
//   File : class_progressbar.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005 
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

#include "class_progressbar.h"

#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"

#include <QProgressBar>

/*
	@doc:	progressbar
	@keyterms:
		progressbar object class,
	@title:
		progressbar class
	@type:
		class
	@short:
		Provides a horizontal progress bar.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This widget can be used to display a horizontal progress bar.
	@functions:
		!fn: $setValue(<value:uinteger>)
		Set the current value.
		See also [classfnc]$setRange[/classfnc]()
		!fn: $setMinimum(<min_steps:uinteger>)
		The progress bar uses the concept of steps; you give it the total number of steps
		and the number of steps completed so far and it will display the percentage of steps that have been completed.
		See also [classfnc]$setMaximum[/classfnc](), [classfnc]$setRange[/classfnc]()
		!fn: $setMaximum(<max_steps:uinteger>)
		See also [classfnc]$setMinimum[/classfnc](), [classfnc]$setRange[/classfnc]()
		!fn: $reset()
		Reset the progress bar.
		!fn: $setTextVisible(<bVisible,boolean>)
		Sets whether the current completed percentage is displayed.
		See also [classfnc]$isTextVisible[/classfnc]()
		!fn: <boolean> $isTextVisible()
		This property holds whether the current completed percentage is displayed.
		See also [classfnc]$setTextVisible[/classfnc]()
		!fn: $setTextDirection(<text:string>)
		Sets the reading direction of the text for vertical progress bar.
		Possible values are TopToBottom or BottomToTop.
		See also [classfnc]$isTextVisible[/classfnc], [classfnc]$orientation[/classfnc]
		!fn: <string> $text()
		This holds the descriptive text shown with the progress bar.
		See also [classfnc]$setTextVisibile[/classfnc](), [classfnc]$setOrientation[/classfnc]()
		!fn: $setOrientation(<text:string>)
		Sets the orientation of the progress bar.
		Possible values are Horizontal or Vertical
		See also [classfnc]$orientation()[/classfnc]
		!fn: <string> $orientation()
		This property holds the orientation of the progress bar.
		See also [classfnc]$setOrientation[/classfnc](), [classfnc]$setTextDirection[/classfnc]
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_progressbar,"progressbar","widget")
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setValue", functionSetValue)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setMinimum", functionSetMinimum)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setMaximum", functionSetMaximum)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setRange", functionSetRange)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"reset",functionReset)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setTextVisible", functionSetTextVisible)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"isTextVisible", functionTextVisible)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setTextDirection", functionSetTextDirection)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"text",functionText)
	KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"setOrientation", functionSetOrientation)
	//KVSO_REGISTER_HANDLER(KviKvsObject_progressbar,"orientation", functionOrientation)
KVSO_END_REGISTERCLASS(KviKvsObject_progressbar)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_progressbar,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_progressbar)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_progressbar)

KVSO_END_CONSTRUCTOR(KviKvsObject_progressbar)

bool KviKvsObject_progressbar::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{	
	QProgressBar *pbar=new QProgressBar(parentScriptWidget());
	pbar->setObjectName(name());
	setObject(pbar,true);

	return true;
}

bool KviKvsObject_progressbar::functionSetValue(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iValue;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("value",KVS_PT_UNSIGNEDINTEGER,0,iValue)
	KVSO_PARAMETERS_END(c)
	if (widget()) ((QProgressBar *)widget())->setValue(iValue);
	return true;
}

bool KviKvsObject_progressbar::functionSetMinimum(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iSteps;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("min_steps",KVS_PT_UNSIGNEDINTEGER,0,iSteps)
	KVSO_PARAMETERS_END(c)
	if(widget()) ((QProgressBar *)widget())->setMinimum(iSteps);
	return true;
}

bool KviKvsObject_progressbar::functionSetMaximum(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t iSteps;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("max_steps",KVS_PT_UNSIGNEDINTEGER,0,iSteps)
	KVSO_PARAMETERS_END(c)
	if(widget()) ((QProgressBar *)widget())->setMaximum(iSteps);
	return true;
}

bool KviKvsObject_progressbar::functionSetRange(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t minSteps;
	kvs_uint_t maxSteps;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("min_steps",KVS_PT_UNSIGNEDINTEGER,0,minSteps)
		KVSO_PARAMETER("max_steps",KVS_PT_UNSIGNEDINTEGER,0,maxSteps)
	KVSO_PARAMETERS_END(c)
	if(widget()) ((QProgressBar *)widget())->setRange(minSteps,maxSteps);
	return true;
}

bool KviKvsObject_progressbar::functionReset(KviKvsObjectFunctionCall *c)
{
	if (widget()) ((QProgressBar *)widget())->reset();
	return true;
}

bool KviKvsObject_progressbar::functionSetTextVisible(KviKvsObjectFunctionCall *c)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if(widget()) ((QProgressBar *)widget())->setTextVisible(bEnabled);
	return true;
}

bool KviKvsObject_progressbar::functionTextVisible(KviKvsObjectFunctionCall *c)
{
	if (widget()) c->returnValue()->setBoolean(((QProgressBar *)widget())->isTextVisible());
	return true;
}

bool KviKvsObject_progressbar::functionSetTextDirection(KviKvsObjectFunctionCall *c)
{
	QProgressBar::Direction szDirection;
	QString szTextDir;
	
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("szTextDir",KVS_PT_STRING,0,szTextDir)
	KVSO_PARAMETERS_END(c)

	if(szDirection=="TopToBottom")
		szDirection = QProgressBar::TopToBottom;
	else if(szDirection=="BottomToTop")
		szDirection = QProgressBar::BottomToTop;
	else c->warning(__tr2qs("Unknown text direction '%Q'"),&szTextDir);

	if(widget()) ((QProgressBar *)widget())->setTextDirection(szDirection);
	return true;
}

bool KviKvsObject_progressbar::functionText(KviKvsObjectFunctionCall *c)
{
	if(widget()) c->returnValue()->setString(((QProgressBar *)widget())->text());
	return true;
}

bool KviKvsObject_progressbar::functionSetOrientation(KviKvsObjectFunctionCall *c)
{
	Qt::Orientation szOrientation;
	QString szTextOrientation;

	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("szTextOrientation",KVS_PT_STRING,0,szTextOrientation)
	KVSO_PARAMETERS_END(c)

	if(szOrientation=="Horizontal") szOrientation = Qt::Horizontal;
	else if(szOrientation=="Vertical") szOrientation = Qt::Vertical;
	else c->warning(__tr2qs("Unknown text orientation '%Q'"),&szTextOrientation);

	if(widget()) ((QProgressBar *)widget())->setOrientation(szOrientation);
	return true;
}

/*
bool KviKvsObject_progressbar::functionOrientation(KviKvsObjectFunctionCall *c)
{
	if(widget()) c->returnValue()->setString(((QProgressBar *)widget())->orientation());
	return true;
}
*/