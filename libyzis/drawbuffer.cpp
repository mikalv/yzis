/*  This file is part of the Yzis libraries
*  Copyright (C) 2006-2008 Loic Pauleve <panard@inzenet.org>
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

#include "drawbuffer.h"

#include "debug.h"
#define dbg()    yzDebug("YDrawBuffer")
#define err()    yzError("YDrawBuffer")

#include "drawline.h"
#include "drawcell.h"
#include "view.h"
#include "viewcursor.h"

/************************
 * YDrawBuffer
 ************************/

YDrawBuffer::YDrawBuffer( const YView* view, int columns, int lines ) :
	mContent(),
	mScreenOffset(0,0),
	mEOLCell(),
	mView(view)
{
	mFirstBufferLine = 0;
	mScreenTopBufferLine = 0;
	mEOLCell.step(" ");
	setScreenSize(columns, lines);
	mContent << (YDrawSection() << YDrawLine());
}
YDrawBuffer::~YDrawBuffer()
{
}

void YDrawBuffer::setScreenSize( int columns, int lines ) 
{
	YASSERT(mScreenWidth > 0);
	YASSERT(mScreenHeight > 0);
	mScreenWidth = columns;
	mScreenHeight = lines;
}

int YDrawBuffer::currentHeight() const
{
	int dy = 0;
	for ( int bl = mScreenTopBufferLine; dy < mScreenHeight && bl-mFirstBufferLine < mContent.count(); ++bl ) {
		dy += mContent[bl-mFirstBufferLine].count();
	}
	return dy;
}

int YDrawBuffer::setBufferDrawSection( int bl, YDrawSection ds, int* shift )
{
	int lid = bl - mFirstBufferLine;
	YASSERT(lid <= mContent.count());
	/* compute screenY */
	int dy = 0;
	for ( int i = 0; i < lid; ++i ) {
		dy += mContent[i].count();
	}
	YASSERT(dy < mScreenHeight);
	int m_shift = 0;
	/* apply section */
	if ( lid == mContent.count() ) {
		mContent << ds;
	} else {
		 /* section size changed? */
		m_shift = ds.count() - mContent[lid].count();
		mContent.replace(lid, ds);
		if ( m_shift > 0 ) { 
			/* remove out of screen lines */
			int my_h = currentHeight();
			int i = mContent.size() - 1;
			while ( my_h - mContent[i].count() > mScreenHeight ) {
				my_h -= mContent[i].count();
				mContent.takeAt(i);
			}
		}
	}
	if ( shift ) *shift = m_shift;
	return dy;
}
YInterval YDrawBuffer::deleteFromBufferDrawSection( int bl )
{
	int lid = bl - mFirstBufferLine;
	YASSERT(0 <= lid)
	YInterval affected;
	if ( lid >= mContent.count() ) {
		return affected;
	}
	/* compute screenY */
	int dy = bufferDrawSectionScreenLine(bl);
	affected.setFrom(YBound(YCursor(0,dy)));
	while ( lid < mContent.count() ) {
		dy += mContent[lid].count();
		mContent.takeAt(lid);
	}
	affected.setTo(YBound(YCursor(0, dy), true));
	return affected;
}

YInterval YDrawBuffer::addSelection( yzis::SelectionType sel, const YInterval& i, yzis::IntervalType itype )
{
	YDrawBufferIterator it = iterator(i, itype);
	if ( !it.isValid() ) {
		return YInterval();
	}
	YCursor begin(/*TODO*/0, it.screenLine());
	int lastScreenLine = 0;
	int lastScreenColumn = screenWidth(); /*TODO*/
	for ( ; it.isValid(); it.next() ) {
		it.cell()->addSelection(sel);
		lastScreenLine = it.screenLine();
		/*lastScreenColumn = it.screenColumn() + it.cell()->width();*/
	}
	return YInterval(begin, YCursor(lastScreenColumn, lastScreenColumn));
}

YInterval YDrawBuffer::delSelection( yzis::SelectionType sel, const YInterval& i, yzis::IntervalType itype )
{
	YDrawBufferIterator it = iterator(i, itype);
	if ( !it.isValid() ) {
		return YInterval();
	}
	YCursor begin(/*TODO*/0, it.screenLine());
	int lastScreenLine = 0;
	int lastScreenColumn = screenWidth(); /*TODO*/
	for ( ; it.isValid(); it.next() ) {
		it.cell()->delSelection(sel);
		lastScreenLine = it.screenLine();
		/*lastScreenColumn = it.screenColumn() + it.cell()->width();*/
	}
	return YInterval(begin, YCursor(lastScreenColumn, lastScreenColumn));
}

YDebugStream& operator<< ( YDebugStream& out, const YDrawBuffer& buff )
{
	int dy = 0;
    for ( int i = 0; i < buff.mContent.size(); ++i ) {
		for ( int j = 0; j < buff.mContent[i].count(); ++j ) {
			out << dy++ << ": " << buff.mContent[i][j] << endl;
		}
    }
    return out;
}

void YDrawBuffer::setEOLCell( const YDrawCell& cell )
{
	mEOLCell = cell;
}

YDrawBufferConstIterator YDrawBuffer::const_iterator( const YInterval& i, yzis::IntervalType itype )
{
	YDrawBufferConstIterator it = YDrawBufferConstIterator(this);
	it.setup(i, itype);
	return it;
}
YDrawBufferIterator YDrawBuffer::iterator( const YInterval& i, yzis::IntervalType itype )
{
	YDrawBufferIterator it = YDrawBufferIterator(this);
	it.setup(i, itype);
	return it;
}

const YDrawSection YDrawBuffer::bufferDrawSection( int bl ) const
{
	int sid = bl - mFirstBufferLine;
	YASSERT(0 <= sid)
	YASSERT(sid < mContent.count());
	return mContent[sid];
}
int YDrawBuffer::bufferDrawSectionScreenLine( int bl ) const
{
	int sid = bl - mFirstBufferLine;
	YASSERT(0 <= sid)
	YASSERT(sid < mContent.count());
	int sl = 0;
	for ( int i = 0; i < sid; ++i ) {
		sl += mContent[i].count();
	}
	return sl;
}

int YDrawBuffer::screenTopBufferLine() const
{
	return mScreenTopBufferLine;
}
int YDrawBuffer::screenBottomBufferLine() const
{
	int height = 0;
	for ( int sid = mScreenTopBufferLine - mFirstBufferLine; sid < mContent.count(); ++sid ) {
		height += mContent[sid].count();
		if ( height >= mScreenHeight ) {
			return sid + mFirstBufferLine;
		}
	}
	return mContent.count() - 1 + mFirstBufferLine;
}

bool YDrawBuffer::scrollForViewCursor( const YViewCursor& vc, int* scroll_horizontal, int* scroll_vertical )
{
	int sid;
	targetBufferLine( vc.line(), &sid );
	*scroll_horizontal = 0;
	int oldScreenTopBufferLine = mScreenTopBufferLine;
	if ( vc.line() < screenTopBufferLine() ) {
		mScreenTopBufferLine = vc.line();
		int delta = 0;
		for ( int bl = mScreenTopBufferLine; bl < oldScreenTopBufferLine; ++bl ) {
			delta += mContent[bl-mFirstBufferLine].count();
		}
		*scroll_vertical = delta;
		return true;
	} else if ( vc.line() > screenBottomBufferLine() ) {
		int height = 0;
		int bl = vc.line();
		for ( ; bl >= 0; --bl ) {
			height += mContent[bl-mFirstBufferLine].count();
			if ( height == mScreenHeight ) {
				mScreenTopBufferLine = bl;
				break;
			} else if ( height > mScreenHeight ) {
				mScreenTopBufferLine = bl + 1; //TODO: what if height(vc.line()) > mScreenHeight?
				break;
			}
		}
		if ( bl < 0 ) mScreenTopBufferLine = 0;

		YASSERT(mScreenTopBufferLine >= oldScreenTopBufferLine);
		int delta = 0;
		for ( int bl = mScreenTopBufferLine; bl > oldScreenTopBufferLine; --bl ) {
			delta += mContent[bl-mFirstBufferLine].count();
		}
		*scroll_vertical = delta;
		return true;
	}
	return false;
}

bool YDrawBuffer::targetBufferLine( int bline, int* sid )
{
	YASSERT(bline >= 0);
	if ( bline < mFirstBufferLine ) {
		for ( int bl = mFirstBufferLine - 1; bl >= bline; --bl ) {
			mContent.insert(0, mView->drawSectionOfBufferLine(bl));
		}
		mFirstBufferLine = bline;
	} else if ( bline - mFirstBufferLine >= mContent.count() ) {
		for ( int bl = mContent.count() + mFirstBufferLine; bl <= bline; ++bl ) {
			mContent << mView->drawSectionOfBufferLine(bl);
		}
	}
	*sid = bline - mFirstBufferLine;
	return true;
}
bool YDrawBuffer::targetBufferColumn( int bcol, int sid, int* lid, int* cid, int* bshift, int* column ) const
{
	YASSERT(0 <= bcol);
	bool lid_found = false;
	int w = 0;
	int my_lid = 0;
	for( ; !lid_found && my_lid < mContent[sid].count(); ++my_lid ) {
		int lw = mContent[sid][my_lid].length();
		if ( w + lw > bcol ) {
			lid_found = true;
			break;
		} else {
			w += lw;
		}
	}
	if ( !lid_found && my_lid > 0 ) --my_lid;
	bool found = false;
	int my_column = 0;
	if ( column != NULL ) {
		my_column = my_lid * mScreenWidth;
	}
	if ( lid_found ) {
		int my_cid = 0;
		for( ; !found && my_cid < mContent[sid][my_lid].count(); ++my_cid ) {
			int cw = mContent[sid][my_lid][my_cid].length();
			if ( w + cw > bcol ) {
				*bshift = bcol - w;
				if ( column != NULL ) {
					my_column += mContent[sid][my_lid][my_cid].widthForLength(bcol - w);
				}
				found = true;
				break;
			} else {
				w += cw;
				if ( column != NULL ) {
					my_column += mContent[sid][my_lid][my_cid].width();
				}
			}
		}
		*lid = my_lid;
		*cid = my_cid;
	}
	if ( column != NULL ) {
		*column = my_column;
	}
	return found;
}

bool YDrawBuffer::targetScreenLine( int sline, int* sid, int* lid, int* bline ) const
{
	YASSERT(0 <= sline);
	YASSERT(sline < screenHeight());
	sline += mScreenTopBufferLine;
	bool found = false;
	int my_sid = 0;
	int my_lid = 0;
	int h = 0;
	for ( ; !found && my_sid < mContent.count(); ++my_sid ) {
		int sh = mContent[my_sid].count();
		if ( h + sh > sline ) {
			my_lid = sline - h;
			found = true;
			break;
		} else {
			h += sh;
		}
	}
	YASSERT(0 <= my_lid);
	YASSERT(my_lid < mContent[my_sid].count());
	*sid = my_sid;
	*lid = my_lid;
	if ( bline != NULL ) *bline = my_sid + mFirstBufferLine;
	return found;
}
bool YDrawBuffer::targetScreenColumn( int scol, int sid, int lid, int* cid, int* sshift, int* position ) const
{
	YASSERT(0 <= scol);
	YASSERT(scol < screenWidth());
	bool found = false;
	int my_cid = 0;
	int w = 0;
	int my_position = 0;
	if ( position != NULL ) {
		for ( int i = 0; i < lid; ++i ) {
			my_position += mContent[sid][i].length();
		}
	}
	for( ; !found && my_cid < mContent[sid][lid].count(); ++my_cid ) {
		int cw = mContent[sid][lid][my_cid].width();
		if ( w + cw > scol ) {
			*sshift = scol - w;
			if ( position != NULL ) {
				my_position += mContent[sid][lid][my_cid].lengthForWidth(scol - w);
			}
			found = true;
			break;
		} else {
			w += cw;
			if ( position != NULL ) {
				my_position += mContent[sid][lid][my_cid].length();
			}
		}
	}
	*cid = my_cid;
	if ( position != NULL ) *position = my_position;
	return found;
}

