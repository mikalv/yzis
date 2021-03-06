/* This file is part of the Yzis libraries
*  Copyright (C) 2004 Adam Connell <adam@argoncorp.com>,
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

#ifndef YZ_LINE_SEARCH_H
#define YZ_LINE_SEARCH_H

/* Yzis */
class YViewCursor;
class YCursor;
class YView;

/* Qt */
#include <QString>

/**
 * Line search
 */
class YLineSearch
{

public:
    /**
     * Each search is bound to a view
     */
    YLineSearch(const YView *_view);
    ~YLineSearch();

    /**
     * Searches start from current cursor location to line boundary
     * for times instances of ch and return a YCursor
     * containing location of successful search
     * found is modified to indicate search success/failure
     * times instances of ch must be found for a search to be successful
     */

    /**
     * Search from cursor to end of line for times instances of ch
     */
    YCursor forward(const QString& ch, bool& found, unsigned int times);

    /**
     * Search from cursor to end of line for times instances of ch
     */
    YCursor forwardBefore(const QString& ch, bool& found, unsigned int times);

    /**
     * Return location of ch searching backwards
     */
    YCursor reverse(const QString& ch, bool& found, unsigned int times);

    /**
     * Return locate right after ch
     */
    YCursor reverseAfter(const QString& ch, bool& found, unsigned int times);

    /**
     * Searches for the next instance of a previously searched character
     */
    YCursor searchAgain(bool &found, unsigned int times);

    /**
     * Searches for previously searched character in opposite direction
     */
    YCursor searchAgainOpposite(bool &found, unsigned int times);

    /**
     * Defines types of searches for history
     * Don't change the order b/c will break searchAgainOpposite
     */
    enum SearchType {
        SearchForward = 0,
        SearchForwardBefore,
        SearchBackward,
        SearchBackwardAfter
    };

private:

    /**
     * View we are working for
     */
    const YView* mView;

    /**
     * Have we searched for anything yet?
     */
    bool mFirstTime;

    /**
     * Direction of last search forward/backwards
     */
    SearchType mType;

    /**
     * Last character that was searched for
     */
    QString mPrevSearched;

    /**
     * Record search information
     */
    void updateHistory(const QString&, SearchType);
};

#endif /*  YZ_LINE_SEARCH_H */
