/**
 * YZBuffer implementation
 */


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "yz_buffer.h"
#include "yz_events.h"
#include "yz_view.h"


/*
 * C++ api
 */

YZBuffer::YZBuffer(char *_path)
{
	path	= _path;
	view_nb	= 0;
	lines	= 0;

	line_first = line_last = NULL; // linked lines

	if (_path!=NULL) load();
}

void YZBuffer::addchar (int x, int y, unicode_char_t c)
{
	yz_event e;
	/* do the actual modification */

	debug("addchar");
	//post_event(e); /* inform the views */
}


void YZBuffer::chgchar (int x, int y, unicode_char_t c)
{
	yz_event e;
	/* do the actual modification */

	debug("addchar");
	//post_event(e); /* inform the views */
}

void YZBuffer::post_event(yz_event e)
{
	int l = e.u.setline.y;

	/* quite basic, we just check that the line is currently displayed */
	for (int i=0; i<view_nb; i++) {
		YZView *v = view_list[i];
		if (v->is_line_visible(l))
			view_list[i]->post_event(e);
	}
}

void YZBuffer::add_view (YZView *v)
{
	if (view_nb>YZ_MAX_VIEW)
		panic("no more rom for new view in YZBuffer");

	//debug("adding view %p, number is %d", v, view_nb);
	view_list[view_nb++] = v;

	update_view(view_nb-1);
} 

void YZBuffer::update_view(int view_nb)
{
	int y;
	YZView *view = view_list[view_nb];
	for (y=view->get_current(); y<lines && y<view->get_current()+view->get_lines_displayed(); y++) {

		YZLine *l = find_line( view->get_current()+y);
		yz_assert(l, "find_line failed");
		/* post an event about updating this line */
		yz_event e;

		e.id			= YZ_EV_SETLINE;
		e.u.setline.y		= y;
		e.u.setline.line	= l;

		view->post_event(e);
	}
}

YZLine	*YZBuffer::find_line(int line)
{
	/* sub-optimal, i know */

	YZLine *l;
	for (l=line_first; l; l=l->next())
		if (l->line==line) return l;
		else if (l->line>line) return NULL;
}


void YZBuffer::load(void)
{
	FILE *f;
	size_t len;
	char buf[YZ_MICROBUFFER_DEFAULT_SIZE+2];
	char *ptr;
	int dismiss=false;

	debug("entering");
	if (!path) {
		error("called though path is null, ignored");
		return;
	}

	f = fopen(path, "r");
	if ( !f) {
		error("Can't open file, errno is %d", errno);
		return;
	}

	len = 0; // len is the number of valid byte in buf[]
	lines	= 0;
	do { // read the whole file
		int a;
		a = fread(buf, sizeof(char), YZ_MICROBUFFER_DEFAULT_SIZE-len, f);
		debug("read %d bytes from file", a); len +=a;


		while (1) {
			for (ptr=buf; (ptr-buf)<len && *ptr!='\n'; ptr++); // find the next '\n'
	
			if (len<=0) break;
			/* quickly handle dismiss case */
			if (dismiss) 
				if ( (ptr-buf)>=len ) {
					/* we are at the end of the buffer, dismiss everything..*/
					len =0;
					debug("deleting.. continue");
					break;
				}
				else {
					/* *ptr == 0, dismiss until \0, included  */
					ptr++;
					len -= (ptr-buf); memcpy(buf, ptr, len); // remove handled part
					debug("removing %d bytes from buf", ptr-buf);
					debug("deleting.. end");
					dismiss = false;
					continue;
				}

			if ( (ptr-buf)>=len) {
				/* we reached the end of the buffer */
				if ( (ptr-buf)<YZ_MICROBUFFER_DEFAULT_SIZE )
					break; // read some more data, the buffer is too small

				/* this line is definetely too long */
				YZLine *line = new YZLine(lines++, buf, len); // ptr-buf == len, here
				len=0;
				/* add the new line */
				add_line(line);
				debug("adding a looooong microbuffer");
				dismiss = true;
				debug("deleting... begin");
				continue;
			}


			yz_assert( *ptr=='\n', "this should not happen, *ptr== %d, ", *ptr);
			// here *ptr='\n'
			ptr++;
			/* we found a whole line, use it */
			YZLine *line = new YZLine(lines++, buf, ptr-buf);
			len -= (ptr-buf); memcpy(buf, ptr, len); // remove handled part
			debug("removing %d bytes from buf", ptr-buf);
			/* add the new line */
			add_line(line);
			debug("adding a normal microbuffer");
		} // while (1)

	} while (!feof(f));

	/* flush the buffer */
	debug("flushing what remains in buffer");
	while (len>0) {
		for (ptr=buf; (ptr-buf)<len && *ptr!='\n'; ptr++); // find the next '\n'

		/* quickly handle dismiss case */
		if (dismiss) 
			if ( (ptr-buf)>=len ) {
				/* we are at the end of the buffer, dismiss everything..*/
				len =0;
				debug("deleting.. continue");
				break;
			}
			else {
				/* *ptr == 0, dismiss until \0, included  */
				ptr++;
				len -= (ptr-buf); memcpy(buf, ptr, len); // remove handled part
				debug("removing %d bytes from buf", ptr-buf);
				debug("deleting.. end");
				dismiss = false;
				continue;
			}

		if ( (ptr-buf)>=len && (ptr-buf)>=YZ_MICROBUFFER_DEFAULT_SIZE ) {
			/* this line is definetely too long */
			YZLine *line = new YZLine(lines++, buf, len); // ptr-buf == len, here
			/* add the new line */
			add_line(line);
			debug("adding a looooong microbuffer");
			break; // nothing left in the buffer
		}


		// everything left has no \n in it, and len should be >0
		yz_assert(len>0, "oops, len is not >0 here, it should though");
		ptr++;
		YZLine *line = new YZLine(lines++, buf, len);
		add_line(line);
		debug("adding a normal microbuffer");
	} // while (1)


	debug("closing input file");
	fclose(f);
}


void YZBuffer::save(void)
{
	if (!path) {
		error("called though path is null, ignored");
		return;
	}
}


/*
 * C api
 */

/*
 * constructors
 */
yz_buffer create_empty_buffer(void) { return (yz_buffer)(new YZBuffer()); }
yz_buffer create_buffer(char *_path) { return (yz_buffer)(new YZBuffer(_path)); }


/*
 * functions
 */
void buffer_addchar(yz_buffer b , int x, int y, unicode_char_t c)
{
	CHECK_BUFFER(b);
	buffer->addchar(x,y,c);
}


void buffer_chgchar(yz_buffer b, int x, int y, unicode_char_t c)
{
	CHECK_BUFFER(b);
	buffer->chgchar(x,y,c);
}


void buffer_load(yz_buffer b)
{
	CHECK_BUFFER(b);
	buffer->load();
}

void buffer_save(yz_buffer b)
{
	CHECK_BUFFER(b);
	buffer->save();
}


