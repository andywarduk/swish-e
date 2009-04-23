/* httpserver.h
$Id$


    This file is part of Swish-e.

    Swish-e is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Swish-e is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along  with Swish-e; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
    
    See the COPYING file that accompanies the Swish-e distribution for details
    of the GNU GPL and the special exception available for linking against
    the Swish-e library.
    
** Mon May  9 18:19:34 CDT 2005
** added GPL


**/

#ifndef __HTTPSERVER_H
#define __HTTPSERVER_H

typedef struct httpserverinfo {
    char *baseurl;
    
    time_t lastretrieval;
    
    char *useragent;
    struct robotrules *robotrules;
    
    struct httpserverinfo *next;
} httpserverinfo;

typedef struct robotrules {
    char *disallow;
    struct robotrules *next;
} robotrules;



httpserverinfo *getserverinfo (SWISH *sw, char *url);
SWINT_T urldisallowed (SWISH *sw, char *url);
SWINT_T equivalentserver (SWISH *sw, char *url, char *baseurl);


#endif

