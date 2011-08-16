/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

Author: David Liang <dliang@novell.com>

*/
#include <glib/gstdio.h>
#include <gio/gio.h>
#include <gdesktop-enums.h>

#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <cairo.h>
#include <cairo-xlib.h>
#include "libgnome-app-store.h"

struct _LibgnomeAppStore
{
	GObject                 parent_instance;
	char *			filename;
};

struct _LibgnomeAppStoreClass
{
	GObjectClass parent_class;
};

enum {
	CHANGED,
	TRANSITIONED,
	N_SIGNALS
};

static guint signals[N_SIGNALS] = { 0 };

G_DEFINE_TYPE (LibgnomeAppStore, libgnome_app_store, G_TYPE_OBJECT)


static void
libgnome_app_store_init (LibgnomeAppStore *bg)
{
}

static void
libgnome_app_store_dispose (GObject *object)
{
	G_OBJECT_CLASS (libgnome_app_store_parent_class)->dispose (object);
}

static void
libgnome_app_store_finalize (GObject *object)
{
	G_OBJECT_CLASS (libgnome_app_store_parent_class)->finalize (object);
}

static void
libgnome_app_store_class_init (LibgnomeAppStoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = libgnome_app_store_dispose;
	object_class->finalize = libgnome_app_store_finalize;
}

LibgnomeAppStore *
libgnome_app_store_new (void)
{
	return g_object_new (LIBGNOME_TYPE_APP_STORE, NULL);
}


