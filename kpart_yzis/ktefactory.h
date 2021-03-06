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


#ifndef YZIS_PART_KTE_FACTORY_H
#define YZIS_PART_KTE_FACTORY_H

#include <ktexteditor/factory.h>

class KTEFactory : public KTextEditor::Factory
{
    Q_OBJECT
public:
    KTEFactory(QObject* parent = 0);
    virtual ~KTEFactory()
    {}
    ;

    virtual KTextEditor::Editor* editor();
    virtual KParts::Part* createPartObject(QWidget*, QObject*, const char*, const QStringList&);
};


#endif // YZIS_PART_KTE_FACTORY_H

