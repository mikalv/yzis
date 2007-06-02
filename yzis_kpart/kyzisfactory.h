/* This file is part of the Yzis libraries
 *  Copyright (C) 2003-2005 Mickael Marchand <marchand@kde.org>
 *  Copyright (C) 2007 Lothar Braun <lothar@lobraun.de>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/


#ifndef _KYZIS_FACTORY_H_
#define _KYZIS_FACTORY_H_

#include "kyziseditor.h"

#include <ktexteditor/factory.h>


class KYZisFactory : public KTextEditor::Factory {
	Q_OBJECT
	public:
		KYZisFactory(QObject* parent = 0);
		virtual ~KYZisFactory() {};

		virtual KTextEditor::Editor* editor() { return KYZisEditor::self(); }

		virtual KParts::Part* createPartObject(QWidget*, QObject*, const char*, const QStringList&);
};


#endif