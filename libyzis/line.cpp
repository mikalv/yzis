/* This file is part of the Yzis libraries
*  Copyright (C) 2004 Lucijan Busch <luci@yzis.org>
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

/* Yzis */
#include "line.h"
#include "debug.h"

/* Qt */
#include <qregexp.h>

#define dbg()    yzDebug("YLine")
#define err()    yzError("YLine")

YLine::YLine(const QString &l) :
    m_flags(YLine::FlagVisible)
{
    setData(l);
    m_initialized = false;
}

YLine::YLine()
{
    setData("");
    m_initialized = false;
}

YLine::~YLine()
{}

void YLine::setData(const QString &data)
{
    mData = data;
    uint len = data.length();

    if(len == 0) {
        len++;    //make sure to return a non empty array ... (that sucks)
    }

    mAttributes.resize(len);

    for(uint i = 0; i < len; i++) {
        mAttributes.data()[ i ] = 0;
    }
}

int YLine::firstChar() const
{
    return nextNonSpaceChar(0);
}

int YLine::lastChar() const
{
    return previousNonSpaceChar(mData.length() - 1);
}

int YLine::nextNonSpaceChar(uint pos) const
{
    int length = (int)mData.length();

    for(int i = pos; i < length; ++i) {
        if(!mData[i].isSpace()) {
            return i;
        }
    }

    return -1;
}

int YLine::previousNonSpaceChar(uint pos) const
{
    if(pos >= (uint)mData.length()) {
        pos = mData.length() - 1;
    }

    for(int i = pos; i >= 0; --i) {
        if(!mData[i].isSpace()) {
            return i;
        }
    }

    return -1;
}

void YLine::addAttribute(int start, int length, int attribute)
{
    if((mAttributesList.size() > 2) && (mAttributesList[mAttributesList.size() - 1] == attribute)
       && (mAttributesList[mAttributesList.size() - 3] + mAttributesList[mAttributesList.size() - 2]
           == start)) {
        mAttributesList[mAttributesList.size() - 2] += length;
        return ;
    }

    mAttributesList.resize(mAttributesList.size() + 3);
    mAttributesList[mAttributesList.size() - 3] = start;
    mAttributesList[mAttributesList.size() - 2] = length;
    mAttributesList[mAttributesList.size() - 1] = attribute;
}

