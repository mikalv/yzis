/* This file is part of the Yzis libraries
 *  Copyright (C) 2005 Scott Newton <scottn@ihug.co.nz>
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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

/**
 * $Id$
 */

#ifndef YZISINFORECORD_H
#define YZISINFORECORD_H

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluevector.h>

#include "cursor.h"

class YZYzisinfoRecord {
	public:
		YZYzisinfoRecord();
		YZYzisinfoRecord( YZYzisinfoRecord & copy );
		virtual ~YZYzisinfoRecord();
		virtual YZYzisinfoRecord & operator=( YZYzisinfoRecord & copy );
		QString & keyword();
		void setKeyword( const QString & keyword );

	protected:
		QString mKeyword;
};

#endif // YZISINFORECORD_H