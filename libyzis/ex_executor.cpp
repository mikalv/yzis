/* This file is part of the Yzis libraries
 *  Copyright (C) 2003 Yzis Team <yzis-dev@yzis.org>
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

#include "ex_executor.h"
#include "debug.h"

YZExExecutor::YZExExecutor() {
}

YZExExecutor::~YZExExecutor() {
}

QString YZExExecutor::write( YZView *view, const QString& inputs ) {
	QRegExp rx ( "^(\\d*)(\\S+) (.*)$");
	rx.search( inputs );
	if ( rx.cap(3) != QString::null ) view->myBuffer()->setPath(rx.cap(3));
	view->myBuffer()->save();
	yzDebug() << "File saved as " << view->myBuffer()->fileName() << endl;
	return QString::null;
}

QString YZExExecutor::buffernext( YZView *view, const QString& ) {
	yzDebug() << "Switching buffers (actually sw views) ..." << endl;
	view->mySession()->setCurrentView(view->mySession()->nextView());
	return QString::null;
}

QString YZExExecutor::quit ( YZView *view, const QString& ) {
	view->mySession()->mGUI->quit();
	return QString::null;
}

QString YZExExecutor::edit ( YZView *view, const QString& inputs ) {
	int idx = inputs.find(" ");
	if ( idx == -1 ) return QString::null; //XXX display error : "No filename given"
	QString path = inputs.mid( idx + 1 ); //extract the path 
	yzDebug() << "New buffer / view : " << path << endl;
	YZBuffer *b = view->mySession()->mGUI->createBuffer( path );
	//TODO
//	YZView* v = view->mySession()->mGUI->createView(b);
//	view->mySession()->mGUI->setCurrentView(v);
	return QString::null;
}
