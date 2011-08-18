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
#include <gio/gio.h>
#include <gdesktop-enums.h>
#include <glib/gdir.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <cairo.h>
#include <cairo-xlib.h>
#include "gnome-app-store.h"

struct _GnomeAppStore
{
	GObject                 parent_instance;
	gchar *			file_uri;
	gchar *			dir_uri;

	GSList *		apps;
	GHashTable *		id_to_name;
	GHashTable *		id_to_icon;
};

struct _GnomeAppStoreClass
{
	GObjectClass parent_class;
};

enum {
	CHANGED,
	TRANSITIONED,
	N_SIGNALS
};

static guint signals[N_SIGNALS] = { 0 };

G_DEFINE_TYPE (GnomeAppStore, gnome_app_store, G_TYPE_OBJECT)

typedef enum _PKG_KEY_WORDS {
        PKG_ID = 0,
        PKG_PKGNAME,
        PKG_NAME,
        PKG_SUMMARY,
        PKG_ICON,
        PKG_APPCATEGORIES,
        PKG_MIMETYPES,
        PKG_APPCATEGORY,
        PKG_MIMETYPE,
        PKG_LAST,
} PKG_KEY_WORDS;

static PKG_KEY_WORDS
get_type_from_name (gchar *name)
{
        gchar *key_words [] = {"id", "pkgname", "name", "summary", "icon",
                        "appcategories", "mimetypes", "appcategory", "mimetype", NULL};
        int i;

        for (i = 0; i < PKG_LAST; i++) {
                if (strcmp (name, key_words [i]) == 0) {
                        return i;
                }
        }

        return PKG_LAST;
}

static void
add_app (GnomeAppStore *store, xmlNodePtr app_node)
{
        xmlNodePtr node, sub_node;
        PKG_KEY_WORDS type;
#ifdef DEBUG
        printf ("node infos: \t");
#endif
	gchar *app_id = NULL;

        for (node = app_node->xmlChildrenNode; node; node = node->next) {
                type = get_type_from_name ((gchar *)(node->name));
                switch (type) {
                        case PKG_ID:
#ifdef DEBUG
                                printf ("desktp id %s\t", xmlNodeGetContent (node));
#endif
				app_id = g_strdup (xmlNodeGetContent (node));
				store->apps = g_slist_append (store->apps, app_id);
                                break;
                        case PKG_PKGNAME:
                                break;
                        case PKG_NAME:
//we have the lang here!
//                              printf ("pkg name %s\t", xmlNodeGetContent (node));
				g_hash_table_insert (store->id_to_name, g_strdup (app_id), g_strdup (xmlNodeGetContent (node)));
                                break;
                        case PKG_SUMMARY:
                                break;
                        case PKG_ICON:
#ifdef DEBUG
                                printf ("icon %s\t", xmlNodeGetContent (node));
#endif
				g_hash_table_insert (store->id_to_icon, g_strdup (app_id), g_strdup (xmlNodeGetContent (node)));
                                break;
                        case PKG_APPCATEGORIES:
#ifdef DEBUG
                                printf ("cate is <");
                                for (sub_node = node->xmlChildrenNode; sub_node; sub_node = sub_node->next) {
                                        if (get_type_from_name ((gchar *)(sub_node->name)) == PKG_APPCATEGORY)
                                                printf ("%s  ", xmlNodeGetContent (sub_node));
                                }
                                printf (">\t");
#endif
                                break;
                        case PKG_MIMETYPES:
#ifdef DEBUG
                                printf ("mime is <");
                                for (sub_node = node->xmlChildrenNode; sub_node; sub_node = sub_node->next) {
                                        if (get_type_from_name ((gchar *)(sub_node->name)) == PKG_MIMETYPE)
                                                printf ("%s  ", xmlNodeGetContent (sub_node));
                                }
                                printf (">\t");
#endif
                                break;
                        default:
                                break;
                }
        }
#ifdef DEBUG
        printf ("\n");
#endif
}

static void
load_apps (GnomeAppStore *store)
{
        xmlDocPtr doc_ptr;
        xmlNodePtr root_node, apps_node, app_node;

        doc_ptr = xmlParseFile (store->file_uri);

        root_node = xmlDocGetRootElement (doc_ptr);

        for (app_node = root_node->xmlChildrenNode; app_node; app_node = app_node->next) {
                if (strcmp (app_node->name, "application") == 0) {
                        add_app (store, app_node);
                }
        }

        xmlFreeDoc(doc_ptr);
}

static void
gnome_app_store_init (GnomeAppStore *store)
{
	store->apps = NULL;
	store->file_uri = NULL;
	store->dir_uri = NULL;

	store->id_to_name = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	store->id_to_icon = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	store->file_uri = g_strdup ("/tmp/appdata.xml");
	load_apps (store);
}

static void
gnome_app_store_dispose (GObject *object)
{
	G_OBJECT_CLASS (gnome_app_store_parent_class)->dispose (object);
}

static void
gnome_app_store_finalize (GObject *object)
{
	GnomeAppStore *store = GNOME_APP_STORE (object);

	if (store->file_uri) {
		g_free (store->file_uri);
		store->file_uri = NULL;
	}

	G_OBJECT_CLASS (gnome_app_store_parent_class)->finalize (object);
}

static void
gnome_app_store_class_init (GnomeAppStoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_store_dispose;
	object_class->finalize = gnome_app_store_finalize;
}

GnomeAppStore *
gnome_app_store_new (void)
{
	return g_object_new (GNOME_TYPE_APP_STORE, NULL);
}

GSList *
gnome_app_store_get_apps (GnomeAppStore *store)
{
	return store->apps;
}

const gchar *
gnome_app_store_get_name_from_id (GnomeAppStore *store, gchar *id)
{
	return g_hash_table_lookup (store->id_to_name, id);
}

const gchar *
gnome_app_store_get_icon_from_id (GnomeAppStore *store, gchar *id)
{
	return g_hash_table_lookup (store->id_to_icon, id);
}
