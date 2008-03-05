#ifndef _SCRIPTEDITOR_H_
#define _SCRIPTEDITOR_H_

//=============================================================================
//
//   File : scripteditor.h
//   Created on Sun Mar 28 1999 16:11:48 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_scripteditor.h"

#include "kvi_qcstring.h"
#include "kvi_selectors.h"

#include <QLabel>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QDialog>
#include <QCheckBox>
#include <QEvent>

class QTimer;

class KviScriptEditorWidget : public QTextEdit
{
	Q_OBJECT
	//Q/_PROPERTY(bool contextSensitiveHelp READ contextSensitiveHelp)
public:
	KviScriptEditorWidget(QWidget * pParent);
	virtual ~KviScriptEditorWidget();
public:
	void updateOptions();
	void find1();
	QString m_szFind;
public slots:
	void slotFind();
	void slotHelp();
	void slotReplace();
//	void slotComplete(const QString &str);

signals:
	void keyPressed();
protected:
//	virtual void keyPressEvent(QKeyEvent * e);

	//Q3PopupMenu *createPopupMenu( const QPoint& pos );
	QWidget *m_pParent;
	QString m_szHelp;

};


class KviScriptSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	 KviScriptSyntaxHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text);

private:

};

class KviScriptEditorWidgetColorOptions : public QDialog
{
	Q_OBJECT
public:
	KviScriptEditorWidgetColorOptions(QWidget * pParent);
	~KviScriptEditorWidgetColorOptions();
private:
	QVector<KviSelectorInterface*> * m_pSelectorInterfaceList;
protected:
	KviColorSelector * addColorSelector(QWidget * pParent,const QString & txt,QColor * pOption,bool bEnabled);

protected slots:
	void okClicked();
};

class KviScriptEditorImplementation : public KviScriptEditor
{
	Q_OBJECT
public:
	KviScriptEditorImplementation(QWidget * par);
	virtual ~KviScriptEditorImplementation();
protected:
	KviScriptEditorWidget * m_pEditor;
	QLabel                * m_pRowColLabel;
	QPoint                  m_lastCursorPos;
public:
	virtual void setText(const QString &txt);
	virtual void getText(QString &txt);
	virtual void setText(const KviQCString &txt);
	virtual void getText(KviQCString &txt);
	virtual void setFindText(const QString & text);
	virtual void setEnabled(bool bEnabled);
	virtual void setFocus();
	virtual bool isModified();
	void setFindLineeditReadOnly(bool b);
	void setCursorPosition(QPoint);
	QPoint getCursor();
	QLineEdit *m_pFindlineedit;
	QLineEdit * getFindlineedit();
protected:
	virtual void focusInEvent(QFocusEvent *e);
	void loadOptions();
	void saveOptions();
protected slots:
	void saveToFile();
	void loadFromFile();
	void configureColors();
	void updateRowColLabel();
	void slotFind();
	void slotReplaceAll(const QString &,const QString &);
	void slotInitFind();
	void slotNextFind(const QString &);
signals:
	void find( const QString &);
	void replaceAll( const QString &, const QString &);
	void initFind();
	void nextFind(const QString &);
};

class KviScriptEditorReplaceDialog: public QDialog
{
	Q_OBJECT
public:
	KviScriptEditorReplaceDialog( QWidget* parent = 0, const char* name = 0);
	~KviScriptEditorReplaceDialog();
	QLineEdit *m_pFindlineedit;
	QLineEdit *m_pReplacelineedit;
protected:
	QLabel *findlabel;
	QLabel *replacelabel;
	QPushButton *replacebutton;
	QPushButton *replace;
	QPushButton *findNext;

//	QPushButton *cancelbutton;
	KviStyledCheckBox *checkReplaceAll;
	QWidget *m_pParent;
protected slots:
	void textChanged(const QString &);
	void slotReplace();
	void slotNextFind();
signals:
	void replaceAll( const QString &,const QString &);
	void initFind();
	void nextFind(const QString &);
	
};


#endif //!_SCRIPTEDITOR_H_
