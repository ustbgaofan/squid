
/*
 * $Id: http_range_test.cc,v 1.5 2003/03/10 04:57:29 robertc Exp $
 *
 * DEBUG: section 64    HTTP Range Header
 * AUTHOR: Alex Rousskov
 *
 * SQUID Web Proxy Cache          http://www.squid-cache.org/
 * ----------------------------------------------------------
 *
 *  Squid is the result of efforts by numerous individuals from
 *  the Internet community; see the CONTRIBUTORS file for full
 *  details.   Many organizations have provided support for Squid's
 *  development; see the SPONSORS file for full details.  Squid is
 *  Copyrighted (C) 2001 by the Regents of the University of
 *  California; see the COPYRIGHT file for full details.  Squid
 *  incorporates software developed and/or copyrighted by other
 *  sources; see the CREDITS file for full details.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 *
 */

#include "squid.h"
#include "Mem.h"
//#include "Store.h"
#include "HttpHeaderRange.h"
//#include "client_side_request.h"
#include "ACLChecklist.h"

/* Stub routines */
SQUIDCEXTERN void
cachemgrRegister(const char *, const char *, OBJH *, int, int)
{}

SQUIDCEXTERN void httpHeaderPutStr(HttpHeader * hdr, http_hdr_type type, const char *str)
{
    fatal ("dummy function\n");
}

#if STDC_HEADERS
SQUIDCEXTERN void
storeAppendPrintf(StoreEntry *, const char *,...)
#else
SQUIDCEXTERN void storeAppendPrintf()
#endif
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN void eventAdd(const char *name, EVH * func, void *arg, double when, int)
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN void
storeBuffer(StoreEntry * e)
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN void
storeBufferFlush(StoreEntry * e)
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN void storeAppend(StoreEntry *, const char *, int)
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN void storeAppendVPrintf(StoreEntry *, const char *, va_list ap)
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN void httpHeaderDelAt(HttpHeader * hdr, HttpHeaderPos pos)
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN HttpHeaderEntry *httpHeaderGetEntry(const HttpHeader * hdr, HttpHeaderPos * pos)
{
    fatal ("dummy function\n");
    return NULL;
}

int aclCheckFast(const acl_access *A, ACLChecklist *)
{
    fatal ("dummy function\n");
    return 0;
}

void
ACLChecklist::operator delete (void *address)
{
    safe_free( address);
}

ACLChecklist::~ACLChecklist(){}

SQUIDCEXTERN void fatal (char const *msg)
{
    printf ("%s\n",msg);
    exit (1);
}

SQUIDCEXTERN ACLChecklist *aclChecklistCreate(const acl_access *,
        request_t *,
        const char *ident)
{
    fatal ("dummy function\n");
    return NULL;
}

SQUIDCEXTERN String httpHeaderGetList(const HttpHeader * hdr, http_hdr_type id)
{
    fatal ("dummy function\n");
    return String();
}

SQUIDCEXTERN int httpHeaderHas(const HttpHeader * hdr, http_hdr_type type)
{
    fatal ("dummy function\n");
    return 0;
}

SQUIDCEXTERN void httpHdrContRangeDestroy(HttpHdrContRange * crange)
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN void httpHdrContRangeSet(HttpHdrContRange *, HttpHdrRangeSpec, ssize_t)
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN void httpHeaderPutContRange(HttpHeader * hdr, const HttpHdrContRange * cr)
{
    fatal ("dummy function\n");
}

SQUIDCEXTERN HttpHdrContRange *httpHdrContRangeCreate(void)
{
    fatal ("dummy function\n");
    return NULL;
}

void
testRangeParser(char const *rangestring)
{
    String aString (rangestring);
    HttpHdrRange *range = HttpHdrRange::ParseCreate (&aString);

    if (!range)
        exit (1);

    HttpHdrRange copy(*range);

    assert (copy.specs.count == range->specs.count);

    HttpHdrRange::iterator pos = range->begin();

    assert (*pos);

    range->deleteSelf();
}

HttpHdrRange *
rangeFromString(char const *rangestring)
{
    String aString (rangestring);
    HttpHdrRange *range = HttpHdrRange::ParseCreate (&aString);

    if (!range)
        exit (1);

    return range;
}

void
testRangeIter ()
{
    HttpHdrRange *range=rangeFromString("bytes=0-3, 1-, -2");
    assert (range->specs.count == 3);
    size_t counter = 0;
    HttpHdrRange::iterator i = range->begin();

    while (i != range->end()) {
        ++counter;
        ++i;
    }

    assert (counter == 3);
    i = range->begin();
    assert (i - range->begin() == 0);
    ++i;
    assert (i - range->begin() == 1);
    assert (i - range->end() == -2);
}

void
testRangeCanonization()
{
    HttpHdrRange *range=rangeFromString("bytes=0-3, 1-, -2");
    assert (range->specs.count == 3);

    /* 0-3 needs a content length of 4 */
    /* This passes in the extant code - but should it? */

    if (!range->canonize(3))
        exit(1);

    assert (range->specs.count == 3);

    range->deleteSelf();

    range=rangeFromString("bytes=0-3, 1-, -2");

    assert (range->specs.count == 3);

    /* 0-3 needs a content length of 4 */
    if (!range->canonize(4))
        exit(1);

    range->deleteSelf();

    range=rangeFromString("bytes=3-6");

    assert (range->specs.count == 1);

    /* 3-6 needs a content length of 4 or more */
    if (range->canonize(3))
        exit(1);

    range->deleteSelf();

    range=rangeFromString("bytes=3-6");

    assert (range->specs.count == 1);

    /* 3-6 needs a content length of 4 or more */
    if (!range->canonize(4))
        exit(1);

    range->deleteSelf();

    range=rangeFromString("bytes=1-1,2-3");

    assert (range->specs.count == 2);

    if (!range->canonize(4))
        exit(1);

    assert (range->specs.count == 2);

    range->deleteSelf();
}

int
main (int argc, char **argv)
{
    Mem::Init();
    /* enable for debugging to console */
    //    _db_init (NULL, NULL);
    //    Debug::Levels[64] = 9;
    testRangeParser ("bytes=0-3");
    testRangeParser ("bytes=-3");
    testRangeParser ("bytes=1-");
    testRangeParser ("bytes=0-3, 1-, -2");
    testRangeIter ();
    testRangeCanonization();
    return 0;
}
