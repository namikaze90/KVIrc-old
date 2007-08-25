//=============================================================================
//		
//   File : scripteditor.cpp
//   Created on Sun Mar 28 1999 16:11:48 CEST by Szymon Stefanek
//	 Code improvements by Carbone Alessandro & Tonino Imbesi
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2004 Szymon Stefanek <pragma at kvirc dot net>
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

#include "scripteditor.h"

#include <qlayout.h>
#include <qtoolbutton.h>
#include <qmessagebox.h>
#include <qtimer.h>

#include <qcursor.h>
#include <qfont.h>
#include <qrect.h>

#include "kvi_fileutils.h"
#include "kvi_locale.h"
#include "kvi_filedialog.h"
#include "kvi_qstring.h"
#include "kvi_config.h"
#include "kvi_module.h"
//
#include "kvi_app.h"
#include "kvi_console.h"
#include "kvi_window.h"
#include "kvi_iconmanager.h"
#include "kvi_kvs_kernel.h"

#include <QSet>
#include <QMenu>
#include <qlayout.h>

extern QSet<KviScriptEditorImplementation*> * g_pScriptEditorWindowList;
extern KviModule * g_pEditorModulePointer;


static QTextCharFormat keywordFormat;
static QTextCharFormat commandFormat;
static QTextCharFormat commentFormat;
static QTextCharFormat bracketFormat;
static QTextCharFormat punctuationFormat;
static QTextCharFormat quotationFormat;
static QTextCharFormat functionFormat;
static QTextCharFormat switchFormat;
static QTextCharFormat variableFormat;
static QTextCharFormat normalFormat;
static QColor          bgColor; 


KviScriptEditorWidgetColorOptions::KviScriptEditorWidgetColorOptions(QWidget * pParent)
: QDialog(pParent)
{
	m_pSelectorInterfaceList = new QVector<KviSelectorInterface*>;
	setCaption(__tr2qs_ctx("Preferences","editor"));
	QGridLayout * g = new QGridLayout(this,3,3,4,4);
/*
 * static QTextCharFormat keywordFormat;
static QTextCharFormat commandFormat;/
static QTextCharFormat commentFormat;
static QTextCharFormat bracketFormat;
static QTextCharFormat punctuationFormat;
static QTextCharFormat quotationFormat;
static QTextCharFormat functionFormat;
static QTextCharFormat switchFormat;
static QTextCharFormat variableFormat;
static QTextCharFormat normalFormat;/
static QColor          backgroundColor(255,255,255); 
*/
	KviTextCharFormatSelector * f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Normal text:","editor"),&normalFormat,true);
	g->addMultiCellWidget(f,0,0,0,2);
	m_pSelectorInterfaceList->append(f);
	
	f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Command:","editor"),&commandFormat,true);
	g->addMultiCellWidget(f,1,1,0,2);
	m_pSelectorInterfaceList->append(f);
	
	f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Bracket:","editor"),&bracketFormat,true);
	g->addMultiCellWidget(f,2,2,0,2);
	m_pSelectorInterfaceList->append(f);
	
	f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Punctuation:","editor"),&punctuationFormat,true);
	g->addMultiCellWidget(f,3,3,0,2);
	m_pSelectorInterfaceList->append(f);
	
	f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Quotation:","editor"),&quotationFormat,true);
	g->addMultiCellWidget(f,4,4,0,2);
	m_pSelectorInterfaceList->append(f);
	
	f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Function:","editor"),&functionFormat,true);
	g->addMultiCellWidget(f,5,5,0,2);
	m_pSelectorInterfaceList->append(f);
	
	f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Switch:","editor"),&switchFormat,true);
	g->addMultiCellWidget(f,6,6,0,2);
	m_pSelectorInterfaceList->append(f);
	
	f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Variable:","editor"),&variableFormat,true);
	g->addMultiCellWidget(f,7,7,0,2);
	m_pSelectorInterfaceList->append(f);
	
	f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Keyword:","editor"),&keywordFormat,true);
	g->addMultiCellWidget(f,8,8,0,2);
	m_pSelectorInterfaceList->append(f);
	
	f = new KviTextCharFormatSelector(this,__tr2qs_ctx("Comment:","editor"),&commentFormat,true);
	g->addMultiCellWidget(f,9,9,0,2);
	m_pSelectorInterfaceList->append(f);
		
	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK","editor"),this);
	b->setDefault(true);
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	g->addWidget(b,10,1);

	b = new QPushButton(__tr2qs_ctx("Cancel","editor"),this);
	connect(b,SIGNAL(clicked()),this,SLOT(reject()));
	g->addWidget(b,10,2);


	//g->setRowStretch(0,1);
	//g->setColStretch(0,1);
}

KviScriptEditorWidgetColorOptions::~KviScriptEditorWidgetColorOptions()
{
	foreach(KviSelectorInterface* i,*m_pSelectorInterfaceList)
	{
		delete i;
	}
	delete m_pSelectorInterfaceList;
}

KviColorSelector * KviScriptEditorWidgetColorOptions::addColorSelector(QWidget * pParent,const QString & txt,QColor * pOption,bool bEnabled)
{
	KviColorSelector * s = new KviColorSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(s);
		return s;
}

void KviScriptEditorWidgetColorOptions::okClicked()
{
	foreach(KviSelectorInterface * i,*m_pSelectorInterfaceList)
	{
		i->commit();
	}

	accept();
}


KviScriptEditorWidget::KviScriptEditorWidget(QWidget * pParent)
: QTextEdit(pParent)
{
	setWordWrapMode(QTextOption::NoWrap);
	m_pParent=pParent;
	m_szHelp="Nothing";
	updateOptions();
	m_szFind="";
	
	//TODO: Completion
	
	//completelistbox=new KviCompletionBox(this);
	//connect (completelistbox,SIGNAL(selected(const QString &)),this,SLOT(slotComplete(const QString &)));
}

KviScriptEditorWidget::~KviScriptEditorWidget()
{

}


/*Q3PopupMenu * KviScriptEditorWidget::createPopupMenu( const QPoint& pos )
{
	Q3PopupMenu *pop=KviTalTextEdit::createPopupMenu(pos);

	pop->insertItem(__tr2qs("Context sensitive help"),this,SLOT(slotHelp()),Qt::CTRL+Qt::Key_H);
	pop->insertItem(__tr2qs("&Replace"),this,SLOT(slotReplace()),Qt::CTRL+Qt::Key_R);
	return pop;
}*/

void KviScriptEditorWidget::slotFind()
{
	m_szFind=((KviScriptEditorImplementation*)m_pParent)->getFindlineedit()->text();
	setText(text());
}

void KviScriptEditorWidget::slotReplace()
{
	KviScriptEditorReplaceDialog *dialog=new KviScriptEditorReplaceDialog(this,tr("Find & Repalce"));
	connect (dialog,SIGNAL(replaceAll(const QString &,const QString &)),m_pParent,SLOT(slotReplaceAll(const QString &,const QString &)));
	connect (dialog,SIGNAL(initFind()),m_pParent,SLOT(slotInitFind()));
	connect (dialog,SIGNAL(nextFind(const QString &)),m_pParent,SLOT(slotNextFind(const QString &)));
	if(dialog->exec()){};

}
void KviScriptEditorWidget::slotHelp()
{

}


void KviScriptEditorWidget::updateOptions()
{
	//setPaper(QBrush(g_clrBackground));
	//setFont(g_fntNormal);
	//setColor(g_clrNormalText);
	
	QPalette p = palette();
	p.setColor(QColorGroup::Background,bgColor);
	setPalette(p);
	
	setText(text()); // an "hack" to ensure Update all in the editor
	new KviScriptSyntaxHighlighter(document());
}

/*void KviScriptEditorWidget::keyPressEvent(QKeyEvent * e)
{
	if(e->state() == Qt::ControlButton)
	{
		switch(e->key())
		{
			case Qt::Key_B:
				insert("$b");
				return;
			case Qt::Key_K:
				insert("$k");
				return;
			case Qt::Key_O:
				insert("$o");
				return;
			case Qt::Key_U:
				insert("$u");
				return;
			break;
		}
	}

	QTextEdit::keyPressEvent(e);
}*/


KviScriptSyntaxHighlighter::KviScriptSyntaxHighlighter(QTextDocument *parent)
     : QSyntaxHighlighter(parent)
{
  
}



#define IN_COMMENT 1
#define IN_LINE 2
#define IN_STRING 4

void KviScriptSyntaxHighlighter::highlightBlock(const QString &text)
{
	int endStateOfLastPara = previousBlockState();
	const QChar * pBuf = (const QChar *)text.ucs2();
	const QChar * c = pBuf;
	
	if(endStateOfLastPara < 0)
	{
		endStateOfLastPara = 0;
	}
	setCurrentBlockState(endStateOfLastPara);
	
	if(!c)return;
	
	
	bool bNewCommand = !(endStateOfLastPara & IN_LINE);
	bool bInComment = endStateOfLastPara & IN_COMMENT;
	bool bInString = endStateOfLastPara & IN_STRING;
	
	const QChar * pBegin;


	while(c->unicode())
	{
		if(bInComment)
		{
			pBegin = c;
			while(c->unicode() && (c->unicode() != '*'))c++;
			if(!c->unicode())
			{
				setFormat(pBegin - pBuf,c - pBegin,commentFormat);
				setCurrentBlockState(IN_COMMENT);
				return ;
			}
			c++;
			if(c->unicode() == '/')
			{
				// end of the comment!
				c++;
				setFormat(pBegin - pBuf,c - pBegin,commentFormat);
				bInComment = false;
				bNewCommand = true;
			}
			continue;
		}

		if(c->isSpace())
		{
			while(c->unicode() && c->isSpace())c++;
			if(!c->unicode())continue;
		}

		pBegin = c;

		// this does not break the bNewCommand flag
		if((c->unicode() == '{') || (c->unicode() == '}'))
		{
			c++;
			setFormat(pBegin - pBuf,1,bracketFormat);
			continue;
		}


		if(bNewCommand)
		{
			bNewCommand = false;

			if(c->unicode() == '#')
			{
				if(c > pBuf)
				{
					const QChar * prev = c - 1;
					if((prev->unicode() == ']') || (prev->unicode() == '}'))
					{
						// array or hash count
						c++;
						setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
						continue;
					}
				}
				// comment until the end of the line
				while(c->unicode())c++;
				setFormat(pBegin - pBuf,c - pBegin,commentFormat);
				continue;
			}
			if(c->unicode() == '/')
			{
				c++;
				if(c->unicode() == '/')
				{
					while(c->unicode())c++;
					setFormat(pBegin - pBuf,c - pBegin,commentFormat);
					continue;
				} else if(c->unicode() == '*')
				{
					c++;
					setFormat(pBegin - pBuf,c - pBegin,commentFormat);
					bInComment = true;
					continue;
				}
				c--;
			}
			if(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '_')))
			{
				c++;
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '.') || (c->unicode() == '_') || (c->unicode() == ':')))c++;
				setFormat(pBegin - pBuf,c - pBegin,keywordFormat);
				// special processing for callbacks and magic commands
				if(pBegin->unicode() == 'e')
				{
					if(c - pBegin == 4)
					{
						// might be "else"
						QString tmp(pBegin,4);
						if(tmp.lower() == "else")bNewCommand = true;
						continue;
					}
				}
				else
				if(pBegin->unicode() == 'f')
				{
					if(c - pBegin == 8)
					{
						// might be "function"
						QString tmp(pBegin,8);
						if(tmp.lower() == "function")bNewCommand = true;
						continue;
					}
				}

				if(pBegin->unicode() == 'i')
				{
					if(c - pBegin == 8)
					{
						// might be "internal"
						QString tmp(pBegin,8);
						if(tmp.lower() == "internal")bNewCommand = true;
						continue;
					}
				}

				// not an else or special command function... FIXME: should check for callbacks.. but that's prolly too difficult :)
				continue;
			}
		}
		if(c->unicode() == '$')
		{
			c++;
			if(c->unicode() == '$')
			{
				c++;
				setFormat(pBegin - pBuf,c - pBegin,keywordFormat);
			} else {
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '.') || (c->unicode() == '_') || (c->unicode() == ':')))c++;
				setFormat(pBegin - pBuf,c - pBegin,functionFormat);
			}
			continue;
		}
		
		if(c->unicode() == '-')
		{
			QChar * pTmp =(QChar *) c;
			c++;
			if(c->unicode() == '-')	c++;
			if(c->isLetter())
			{
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '_')))c++;
				setFormat(pBegin - pBuf,c - pBegin,switchFormat);
				continue;
			} else {
				while(c!=pTmp) c--;
			}
		}

		if(c->unicode() == '%')
		{
			c++;
			if(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == ':') || (c->unicode() == '_')))
			{
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == ':') || (c->unicode() == '_')))c++;
				setFormat(pBegin - pBuf,c - pBegin,variableFormat);
				continue;
			}
			c--;
		}

		if(!c->unicode())continue;

		if(c->isLetterOrNumber() || c->unicode() == '_')
		{
			c++;
			while(c->unicode() && c->isLetterOrNumber() || (c->unicode() == '_'))c++;
			setFormat(pBegin - pBuf,c - pBegin,quotationFormat);
			continue;
		}

		if(c->unicode() == '\\')
		{
			c++;
			setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
			// the next char is to be interpreted as normal text
			pBegin = c;
			if(c->unicode() && (c->unicode() != '\n'))
			{
				c++;
				setFormat(pBegin - pBuf,c - pBegin,quotationFormat);
				continue;
			}
			// this is never returned since Qt sux in string processing
			// it sets the newlines to spaces and we have no secure way to undestand that this was the end of a line
			setCurrentBlockState(IN_LINE);
			return;
		}

		if(c->unicode() == '"')
		{
			bInString = !bInString;
			c++;
			setFormat(pBegin - pBuf,c - pBegin,quotationFormat);
			continue;
		} else if(c->unicode() == ';')
		{
			if(!bInString)	bNewCommand = true; // the next will be a new command
		}

		c++;
		//if(bInString)
		{
			setFormat(pBegin - pBuf,c - pBegin,quotationFormat);
		}/* else {
			setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
		}*/
	}

	/*bool i=TRUE;
	QString szFind=((KviScriptEditorWidget *)textEdit())->m_szFind;
	if (!szFind.isEmpty())
	{
		int index=0;
		while (i)
		{
			index=text.find(szFind,index,false);
			if (index != -1)
			{
				setFormat(index,szFind.length(),g_clrFind);
				index += szFind.length();
			}
		else i=false;
		}
	}*/
	if(bInString)
	{
		setCurrentBlockState(IN_LINE | IN_STRING);
		return ;
	} else { 
		setCurrentBlockState(0);
		return;
	}
}

// 22.02.2005 :: 00:01
// valgrind --leak-check=yes --num-callers=10 -v kvirc -f
//
//==30299== Warning: SIGSEGV not in user code; either from syscall kill()
//==30299==    or possible Valgrind bug.  This message is only shown 3 times.
//==30299== Warning: SIGSEGV not in user code; either from syscall kill()
//==30299==    or possible Valgrind bug.  This message is only shown 3 times.
//==30299== Warning: SIGSEGV not in user code; either from syscall kill()
//==30299==    or possible Valgrind bug.  This message is only shown 3 times.


KviScriptEditorImplementation::KviScriptEditorImplementation(QWidget * par)
:KviScriptEditor(par)
{
	if(g_pScriptEditorWindowList->isEmpty())loadOptions();
	g_pScriptEditorWindowList->insert(this);
	m_lastCursorPos=QPoint(0,0);
	QGridLayout * g = new QGridLayout(this,2,3,0,0);

	m_pFindLineedit = new QLineEdit(" ",this);

	m_pFindLineedit->setText("");

	m_pEditor = new KviScriptEditorWidget(this);
	g->addMultiCellWidget(m_pEditor,0,0,0,3);
	g->setRowStretch(0,1);

	QToolButton * b = new QToolButton(Qt::DownArrow,this,"dsa2");

	b->setMinimumWidth(24);
	g->addWidget(b,1,0);

	QMenu * pop = new QMenu(b);
	pop->addAction(__tr2qs_ctx("&Open...","editor"),this,SLOT(loadFromFile()));
	pop->addAction(__tr2qs_ctx("&Save As...","editor"),this,SLOT(saveToFile()));
	pop->addSeparator();
	pop->addAction(__tr2qs_ctx("&Configure Editor...","editor"),this,SLOT(configureColors()));
	b->setMenu(pop);

	g->setColStretch(1,1);
	g->setColStretch(2,10);
	g->addWidget(m_pFindLineedit,1,2);
	QLabel *lab= new QLabel("find",this);
	lab->setText(tr("Find"));

	g->addWidget(lab,1,1);
	m_pRowColLabel = new QLabel("0",this);
	m_pRowColLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pRowColLabel->setMinimumWidth(80);
	g->addWidget(m_pRowColLabel,1,3);

	connect(m_pFindLineedit,SIGNAL(returnPressed()),m_pEditor,SLOT(slotFind()));
	connect(m_pFindLineedit,SIGNAL(returnPressed()),this,SLOT(slotFind()));
	connect(m_pEditor,SIGNAL(keyPressed()),this,SLOT(updateRowColLabel()));
	connect(m_pEditor,SIGNAL(textChanged()),this,SLOT(updateRowColLabel()));
	connect(m_pEditor,SIGNAL(selectionChanged()),this,SLOT(updateRowColLabel()));
	m_lastCursorPos = QPoint(-1,-1);
}

KviScriptEditorImplementation::~KviScriptEditorImplementation()
{
	g_pScriptEditorWindowList->remove(this);
	if(g_pScriptEditorWindowList->isEmpty())saveOptions();
}

void KviScriptEditorImplementation::loadOptions()
{
	QString tmp;
	g_pEditorModulePointer->getDefaultConfigFileName(tmp);

	KviConfig cfg(tmp,KviConfig::Read);
	
	cfg.setGroup("Colors");
#define READ_ENTRY(x,y) y = cfg.readTextCharFormatEntry((x),(y))
	READ_ENTRY("Keyword",keywordFormat);
	READ_ENTRY("Command",commandFormat);
	READ_ENTRY("Comment",commentFormat);
	READ_ENTRY("Bracket",bracketFormat);
	READ_ENTRY("Punctuation",punctuationFormat);
	READ_ENTRY("Quotation",quotationFormat);
	READ_ENTRY("Function",functionFormat);
	READ_ENTRY("Switch",switchFormat);
	READ_ENTRY("Variable",variableFormat);
	READ_ENTRY("Normal",normalFormat);
	bgColor = cfg.readColorEntry("Background",palette().color(QPalette::Background));
	
	debug("settings must be loaded");
}

bool KviScriptEditorImplementation::isModified()
{
	return m_pEditor->isModified();
}

void KviScriptEditorImplementation::slotFind()
{
	emit find(m_pFindLineedit->text());

}
void KviScriptEditorImplementation::slotNextFind(const QString &text)
{
//	emit nextFind(const QString &text);

}void KviScriptEditorImplementation::slotInitFind()
{
	emit initFind();

}
void KviScriptEditorImplementation::slotReplaceAll(const QString &txt,const QString &txt1)
{
	emit replaceAll(txt,txt1);
}
void KviScriptEditorImplementation::saveOptions()
{
	QString tmp;
	g_pEditorModulePointer->getDefaultConfigFileName(tmp);

	KviConfig cfg(tmp,KviConfig::Write);
	
	cfg.setGroup("Colors");
	
	cfg.writeEntry("Keyword",keywordFormat);
	cfg.writeEntry("Command",commandFormat);
	cfg.writeEntry("Comment",commentFormat);
	cfg.writeEntry("Bracket",bracketFormat);
	cfg.writeEntry("Punctuation",punctuationFormat);
	cfg.writeEntry("Quotation",quotationFormat);
	cfg.writeEntry("Function",functionFormat);
	cfg.writeEntry("Switch",switchFormat);
	cfg.writeEntry("Variable",variableFormat);
	cfg.writeEntry("Normal",normalFormat);
	cfg.writeEntry("Background",bgColor);
}

void KviScriptEditorImplementation::setFocus()
{
	m_pEditor->setFocus();
}



void KviScriptEditorImplementation::focusInEvent(QFocusEvent *)
{
	m_pEditor->setFocus();
}


void KviScriptEditorImplementation::setEnabled(bool bEnabled)
{
	QWidget::setEnabled(bEnabled);
	m_pEditor->setEnabled(bEnabled);
	m_pRowColLabel->setEnabled(bEnabled);
}

void KviScriptEditorImplementation::saveToFile()
{
	QString fName;
	if(KviFileDialog::askForSaveFileName(fName,
		__tr2qs_ctx("Choose a Filename - KVIrc","editor"),
		QString::null,
		QString::null,false,true,true))
	{
		QString buffer = m_pEditor->text();

		//if(tmp.isEmpty())tmp = "";
		//KviStr buffer = tmp.utf8().data();
		if(!KviFileUtils::writeFile(fName,buffer))
		{
			QString tmp;
			QMessageBox::warning(this,
				__tr2qs_ctx("Save Failed - KVIrc","editor"),
				KviQString::sprintf(tmp,__tr2qs_ctx("Can't open the file %s for writing.","editor"),&fName));
		}
	}
}

void KviScriptEditorImplementation::setText(const KviQCString &txt)
{
	m_pEditor->clear();
	m_pEditor->insertPlainText(txt);
	updateRowColLabel();
}

void KviScriptEditorImplementation::getText(KviQCString &txt)
{
	txt = m_pEditor->text();
}
QLineEdit * KviScriptEditorImplementation::getFindlineedit()
{
	return m_pFindLineedit;
}
void KviScriptEditorImplementation::setText(const QString &txt)
{
	m_pEditor->setPlainText(txt);
	updateRowColLabel();
}

void KviScriptEditorImplementation::getText(QString &txt)
{
	txt = m_pEditor->text();
}
void KviScriptEditorImplementation::setFindText(const QString &txt)
{
	m_pFindLineedit->setText(txt);
	m_pEditor->slotFind();

}

void KviScriptEditorImplementation::setFindLineeditReadOnly(bool b)
{
	m_pFindLineedit->setReadOnly(b);
	
}


void KviScriptEditorImplementation::updateRowColLabel()
{
}

QPoint KviScriptEditorImplementation::getCursor()
{
		return m_lastCursorPos;
}
void KviScriptEditorImplementation::setCursorPosition(QPoint pos)
{
}

void KviScriptEditorImplementation::loadFromFile()
{
	QString fName;
	if(KviFileDialog::askForOpenFileName(fName,
		__tr2qs_ctx("Load Script File - KVIrc","editor"),
		QString::null,
		QString::null,false,true))
	{
		QString buffer;
		if(KviFileUtils::loadFile(fName,buffer))
		{
			m_pEditor->setText(buffer);
//			m_pEditor->moveCursor(KviTalTextEdit::MoveEnd,false);
			updateRowColLabel();
		} else {
			QString tmp;
			QMessageBox::warning(this,
				__tr2qs_ctx("Open Failed - KVIrc","editor"),
				KviQString::sprintf(tmp,__tr2qs_ctx("Can't open the file %s for reading.","editor"),&fName));
		}
	}
}

void KviScriptEditorImplementation::configureColors()
{
	KviScriptEditorWidgetColorOptions dlg(this);
	if(dlg.exec() == QDialog::Accepted)

	{
		m_pEditor->updateOptions();
		saveOptions();
	}
}
KviScriptEditorReplaceDialog::KviScriptEditorReplaceDialog( QWidget* parent, const char* name)
    : QDialog( parent)
{
	m_pParent=parent;
	emit initFind();
	setPaletteForegroundColor( QColor( 0, 0, 0 ) );
	setPaletteBackgroundColor( QColor( 236, 233, 216 ) );
	QGridLayout *layout = new QGridLayout( this, 1, 1, 11, 6, "replace layout"); 
 
	m_pFindlineedit = new QLineEdit( this, "findlineedit" );

	layout->addMultiCellWidget( m_pFindlineedit, 2, 2, 1, 2 );

	m_pReplacelineedit = new QLineEdit( this, "replacelineedit" );

	layout->addMultiCellWidget( m_pReplacelineedit, 3, 3, 1, 2 );

   	m_pFindlineedit->setFocus();

    QLabel *findlabel = new QLabel( this, "findlabel" );
	findlabel->setText(tr("Word to Find"));

	layout->addWidget( findlabel, 2, 0 );

    QLabel *replacelabel = new QLabel( this, "replacelabel" );
	replacelabel->setText(tr("Replace with"));

	layout->addWidget( replacelabel, 3, 0 );

    QPushButton *cancelbutton = new QPushButton( this, "cancelButton" );
	cancelbutton->setText(tr("&Cancel"));
	layout->addWidget( cancelbutton, 5, 2 );

    replacebutton = new QPushButton( this, "replacebutton" );
	replacebutton->setText(tr("&Replace"));
	replacebutton->setEnabled( FALSE );
	layout->addWidget( replacebutton, 5, 0 );

	checkReplaceAll = new KviStyledCheckBox( this, "replaceAll" );
	checkReplaceAll->setText(tr("&Replace in all Aliases"));
	layout->addWidget( checkReplaceAll, 4, 0 );
	
	findNext = new QPushButton(this, "findNext(WIP)" );	
	findNext->setText(tr("&Findnext"));
	layout->addWidget( findNext, 2, 3 );
	findNext->setEnabled(false);

	replace = new QPushButton(this, "replace" );	
	replace->setText(tr("&Replace(WIP)"));
	layout->addWidget( replace, 3, 3 );
	replace->setEnabled(false);


	// signals and slots connections
	connect( replacebutton, SIGNAL( clicked() ), this, SLOT( slotReplace() ) );
	connect( findNext, SIGNAL( clicked() ),this,SLOT( slotNextFind()));
	connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( m_pFindlineedit, SIGNAL( textChanged(const QString &)), this, SLOT( textChanged(const QString &)));

}


KviScriptEditorReplaceDialog::~KviScriptEditorReplaceDialog()
{
}

void KviScriptEditorReplaceDialog::textChanged(const QString &txt)
{
	if (!txt.isEmpty()) replacebutton->setEnabled(TRUE);
	else replacebutton->setEnabled(FALSE);

}
void KviScriptEditorReplaceDialog::slotReplace()
{
	QString txt=((KviScriptEditorWidget *)m_pParent)->text();
	if (checkReplaceAll->isChecked()) emit replaceAll(m_pFindlineedit->text(),m_pReplacelineedit->text());
	txt.replace(m_pFindlineedit->text(),m_pReplacelineedit->text(),false);
	((KviScriptEditorWidget *)m_pParent)->setText(txt);
	((KviScriptEditorWidget *)m_pParent)->setModified(true);
	m_pFindlineedit->setText("");
	m_pReplacelineedit->setText("");
	setTabOrder(m_pFindlineedit,m_pReplacelineedit);
}


void KviScriptEditorReplaceDialog::slotNextFind()
{
	emit nextFind(m_pFindlineedit->text());
}
