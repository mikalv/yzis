/* This file is part of the Yzis libraries
*  Copyright (C) 2004 Philippe Fremy <phil@freehackers.org>,
*  Copyright (C) 2004-2005 Mickael Marchand <marchand@kde.org>
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

/**
 * This file contains the classes necessary to handle undo/redo
 */

#include "undo.h"
#include "buffer.h"
#include "action.h"
#include "view.h"
#include "debug.h"

#define dbg()    yzDebug("YBufferOperation")
#define err()    yzError("YBufferOperation")

QString YBufferOperation::toString() const
{
    QString ots;

    switch(type) {
    case OpAddRegion:
        ots = "OpAddText";
        break;

    case OpDelRegion:
        ots = "OpDelRegion";
        break;
    }

    return QString("%1 %2 '%3'").arg(ots).arg(interval.toString()).arg(data.join("\\n"));
}

void YBufferOperation::performOperation(YView* pView, bool opposite)
{
    OperationType t = type;
    yzDebug("YZUndoBuffer") << "YBufferOperation: " << (opposite ? "undo " : "redo ") << toString() << endl;

    if(opposite == true) {
        switch(type) {
        case OpAddRegion:
            t = OpDelRegion;
            break;

        case OpDelRegion:
            t = OpAddRegion;
            break;
        }
    }

    switch(t) {
    case OpAddRegion:
        pView->buffer()->insertRegion(interval.fromPos(), data);
        break;

    case OpDelRegion:
        pView->buffer()->deleteRegion(interval);
        break;
    }

    // yzDebug("YZUndoBuffer") << "YBufferOperation::performOperation Buf -> '" << buf->getWholeText() << "'\n";
}

// -------------------------------------------------------------------
//                          YZUndoItem
// -------------------------------------------------------------------

UndoItem::UndoItem()
{
    startCursorX = startCursorY = 0;
    endCursorX = endCursorY = 0;
}

// -------------------------------------------------------------------
//                          YZUndoBuffer
// -------------------------------------------------------------------

YZUndoBuffer::YZUndoBuffer(YBuffer * buffer)
    : mBuffer(buffer), mFutureUndoItem(0L)
{
    mCurrentIndex = 0;
    mInsideUndo = false;
    // Create the mFutureUndoItem
    commitUndoItem(0, 0);
}

YZUndoBuffer::~YZUndoBuffer()
{
    if(mFutureUndoItem) {
        foreach(YBufferOperation *operation, *mFutureUndoItem) {
            delete operation;
        }

        delete mFutureUndoItem;
    }

    foreach(UndoItem *item, mUndoItemList) {
        delete item;
    }
}

void YZUndoBuffer::commitUndoItem(uint cursorX, uint cursorY)
{
    if(mInsideUndo == true) {
        return ;
    }

    if(mFutureUndoItem && mFutureUndoItem->count() == 0) {
        return ;
    }

    if(mFutureUndoItem) {
        removeUndoItemAfterCurrent();
        mFutureUndoItem->endCursorX = cursorX;
        mFutureUndoItem->endCursorY = cursorY;
        mUndoItemList.push_back(mFutureUndoItem);
        mCurrentIndex = mUndoItemList.size();
        //  yzDebug("YZUndoBuffer") << "UndoItem::commitUndoItem" << toString() << endl;
    }

    mFutureUndoItem = new UndoItem();
    mFutureUndoItem->startCursorX = cursorX;
    mFutureUndoItem->startCursorY = cursorY;
}

void YZUndoBuffer::addBufferOperation(YBufferOperation::OperationType type,
                                      const YRawData& data,
                                      const YInterval& interval)
{
    if(mInsideUndo == true) {
        return ;
    }

    YASSERT(mFutureUndoItem != NULL);
    YBufferOperation *bufOperation = new YBufferOperation();
    bufOperation->type = type;
    bufOperation->data = data;
    bufOperation->interval = interval;
    mFutureUndoItem->push_back(bufOperation);
    removeUndoItemAfterCurrent();
}

void YZUndoBuffer::removeUndoItemAfterCurrent()
{
    while((uint)mUndoItemList.size() > mCurrentIndex) {
        mUndoItemList.pop_back();
    }

    //delete pointer XXX
}

template <typename T>
static QList<T> reverse(const QList<T> &yzlist)
{
    QList<T> rev;

    foreach(T t, yzlist) {
        rev.push_front(t);
    }

    return rev;
}

void YZUndoBuffer::undo(YView* pView)
{
    if(mayUndo() == false) {
        // notify the user that undo is not possible
        return ;
    }

    setInsideUndo(true);
    pView->setPaintAutoCommit(false);
    UndoItem *item = mUndoItemList[ mCurrentIndex - 1 ];
    UndoItemBase reversed = reverse(*item);

    foreach(YBufferOperation *operation, reversed) {
        operation->performOperation(pView, true);
    }

    /*
    UndoItem * undoItem = mUndoItemList.at(mCurrentIndex-1);
    UndoItemContentIterator it( *undoItem );
    it.toBack();
    while( it.hasPrevious() ) {
     bufOp = it.previous();
     bufOp->performOperation( pView, true );
    }
    */
    mCurrentIndex--;
    pView->gotoLinePosition(item->endCursorY, item->endCursorX);
    pView->commitPaintEvent();
    setInsideUndo(false);
}

void YZUndoBuffer::redo(YView* pView)
{
    if(mayRedo() == false) {
        // notify the user that undo is not possible
        return ;
    }

    setInsideUndo(true);
    pView->setPaintAutoCommit(false);
    ++mCurrentIndex;
    UndoItem * undoItem = mUndoItemList[ mCurrentIndex - 1 ];

    foreach(YBufferOperation *operation, *undoItem) {
        operation->performOperation(pView, false);
    }

    setInsideUndo(false);
    pView->commitPaintEvent();
}

bool YZUndoBuffer::mayRedo() const
{
    bool ret;
    ret = mCurrentIndex < (uint)mUndoItemList.count();
    return ret;
}

bool YZUndoBuffer::mayUndo() const
{
    bool ret;
    ret = mCurrentIndex > 0;
    return ret;
}

QString YZUndoBuffer::undoItemToString(UndoItem * undoItem) const
{
    QString s;
    QString offsetS = "  ";
    s += offsetS + offsetS + "UndoItem:\n";

    if(! undoItem) {
        return s;
    }

    s += offsetS + offsetS + QString("start cursor: line %1 col %2\n").arg(undoItem->startCursorX).arg(undoItem->startCursorY);

    foreach(YBufferOperation*operation, *undoItem) {
        s += offsetS + offsetS + offsetS + operation->toString() + '\n';
    }

    s += offsetS + offsetS + QString("end cursor: line %1 col %2\n").arg(undoItem->endCursorX).arg(undoItem->endCursorY);
    return s;
}

QString YZUndoBuffer::toString(const QString& msg) const
{
    QString s = msg + " YZUndoBuffer:\n";
    QString offsetS = "  ";
    s += offsetS + "mUndoItemList\n";

    foreach(UndoItem *it, mUndoItemList) {
        s += undoItemToString(it);
    }

    s += offsetS + "mFutureUndoItem\n";
    s += undoItemToString(mFutureUndoItem);
    s += offsetS + "current UndoItem\n";
    s += (mCurrentIndex > 0) ? undoItemToString(mUndoItemList[ mCurrentIndex - 1 ])
         : offsetS + offsetS + "None\n";
    s += '\n';
    return s;
}

