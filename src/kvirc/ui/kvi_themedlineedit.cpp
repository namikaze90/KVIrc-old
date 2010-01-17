//=============================================================================
//
//   File : kvi_themedlineedit.cpp
//   Creation date : Sun Jan 10 2010 12:17:00 by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_themedlineedit.h"
#include "kvi_options.h"
#include "kvi_settings.h"
#include "kvi_app.h"
#include "kvi_window.h"

#include <QPainter>
#include <QStyleOptionFrameV2>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

KviThemedLineEdit::KviThemedLineEdit(QWidget * par,const char * name)
: QLineEdit(par)
{
	setObjectName(name);
	setAutoFillBackground(false);

	QPalette pal = palette();
	pal.setBrush(QPalette::Active, QPalette::Base, Qt::transparent);
	pal.setBrush(QPalette::Inactive, QPalette::Base, Qt::transparent);
	pal.setBrush(QPalette::Disabled, QPalette::Base, Qt::transparent);

	setPalette(pal);

	applyOptions();
}

KviThemedLineEdit::~KviThemedLineEdit()
{
}

void KviThemedLineEdit::applyOptions()
{
	setFont(KVI_OPTION_FONT(KviOption_fontLabel));
	update();
}

void KviThemedLineEdit::paintEvent ( QPaintEvent * event )
{
	QPainter p(this);
	QRect r = rect();
	QPalette pal = palette();
	QStyleOptionFrameV2 option;

	option.initFrom(this);
	option.rect = contentsRect();
	option.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, this);
	option.midLineWidth = 0;
	option.state |= QStyle::State_Sunken;
	if(isReadOnly())
		option.state |= QStyle::State_ReadOnly;
	option.features = QStyleOptionFrameV2::None;

	r = style()->subElementRect(QStyle::SE_LineEditContents, &option, this);
	int left, right, top, bottom;
	getTextMargins(&left, &top, &right, &bottom);
	r.setX(r.x() + left);
	r.setY(r.y() + top);
	r.setRight(r.right() - right);
	r.setBottom(r.bottom() - bottom);
	p.setClipRect(r);

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p.save();
		p.setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p.fillRect(contentsRect(), col);
		p.restore();
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapToGlobal(contentsRect().topLeft());
		p.drawTiledPixmap(contentsRect(),*g_pShadedChildGlobalDesktopBackground,pnt);
	} else {
#endif

		if(KVI_OPTION_PIXMAP(KviOption_pixmapLabelBackground).pixmap())
		{
			p.drawTiledPixmap(contentsRect(),*(KVI_OPTION_PIXMAP(KviOption_pixmapLabelBackground).pixmap()));
		} else {
			p.fillRect(contentsRect(),KVI_OPTION_COLOR(KviOption_colorLabelBackground));
		}

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	QLineEdit::paintEvent(event);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_themedlineedit.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES