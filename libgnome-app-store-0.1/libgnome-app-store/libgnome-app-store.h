/* libgnome-app-store.h - 

   Copyright 2011, Novell, Inc.

   The Libgnome appstore lib is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   The Libgnome appstore lib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: David Liang <dliang@novell.com>
*/

#ifndef __LIBGNOME_APP_STORE_H__
#define __LIBGNOME_APP_STORE_H__

#include <gdk/gdk.h>
#include <gio/gio.h>
#include <gdesktop-enums.h>

G_BEGIN_DECLS

#define LIBGNOME_TYPE_APP_STORE            (libgnome_app_store_get_type ())
#define LIBGNOME_APP_STORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBGNOME_TYPE_APP_STORE, LibgnomeAppStore))
#define LIBGNOME_APP_STORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  LIBGNOME_TYPE_APP_STORE, LibgnomeAppStoreClass))
#define LIBGNOME_IS_APP_STORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBGNOME_TYPE_APP_STORE))
#define LIBGNOME_IS_APP_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  LIBGNOME_TYPE_APP_STORE))
#define LIBGNOME_APP_STORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  LIBGNOME_TYPE_APP_STORE, LibgnomeAppStoreClass))

typedef struct _LibgnomeAppStore LibgnomeAppStore;
typedef struct _LibgnomeAppStoreClass LibgnomeAppStoreClass;

GType  		          libgnome_app_store_get_type              (void);
LibgnomeAppStore *        libgnome_app_store_new                   (void);

G_END_DECLS

#endif
