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
#include <string.h>

#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <clutter/clutter.h>

#include <cairo.h>
#include <cairo-xlib.h>
#include "gnome-app-store.h"

struct _GnomeAppStorePrivate
{
	gchar *			file_uri;
	gchar *			dir_uri;

	GSList *		apps;
	GHashTable *		id_to_name;
	GHashTable *		id_to_icon;
	GHashTable *		icon_store;
	GHashTable *		default_icon_store;
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
        GnomeAppStorePrivate *priv = store->priv;
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
				priv->apps = g_slist_append (priv->apps, app_id);
                                break;
                        case PKG_PKGNAME:
                                break;
                        case PKG_NAME:
//we have the lang here!
//                              printf ("pkg name %s\t", xmlNodeGetContent (node));
				g_hash_table_insert (priv->id_to_name, g_strdup (app_id), g_strdup (xmlNodeGetContent (node)));
                                break;
                        case PKG_SUMMARY:
                                break;
                        case PKG_ICON:
#ifdef DEBUG
                                printf ("icon %s\t", xmlNodeGetContent (node));
#endif
				g_hash_table_insert (priv->id_to_icon, g_strdup (app_id), g_strdup (xmlNodeGetContent (node)));
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
        GnomeAppStorePrivate *priv = store->priv;
        xmlDocPtr doc_ptr;
        xmlNodePtr root_node, apps_node, app_node;

        doc_ptr = xmlParseFile (priv->file_uri);

        root_node = xmlDocGetRootElement (doc_ptr);

        for (app_node = root_node->xmlChildrenNode; app_node; app_node = app_node->next) {
                if (strcmp (app_node->name, "application") == 0) {
                        add_app (store, app_node);
                }
        }

        xmlFreeDoc(doc_ptr);
}

static void
load_icons (GnomeAppStore *store)
{
	GDir *dir;
	const gchar *basename;
	gchar *filename;
	gchar *p, *content;
	GnomeAppStorePrivate *priv = store->priv;

	dir = g_dir_open (priv->dir_uri, 0, NULL);
	if (!dir) {
		printf ("cannot open dir\n");
			return;
	}
                
	while ((basename = g_dir_read_name (dir)) != NULL) {
		filename = g_build_filename (priv->dir_uri, basename, NULL);
		content = g_strdup (basename);
		p = strrchr (content, '.');
		if (p)
			*p = 0;
                        
		g_hash_table_insert (priv->icon_store, content, filename);
	}
	g_dir_close (dir);
}

static void
gnome_app_store_init (GnomeAppStore *store)
{
	GnomeAppStorePrivate *priv;

	store->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (store,
                                                   GNOME_TYPE_APP_STORE,
                                                   GnomeAppStorePrivate);

	priv->apps = NULL;
	priv->file_uri = NULL;
	priv->dir_uri = NULL;

	priv->id_to_name = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	priv->id_to_icon = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	priv->icon_store = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	priv->default_icon_store = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	priv->file_uri = g_strdup ("/tmp/appdata.xml");
	priv->dir_uri = g_strdup ("/tmp/icons");

	load_icons (store);
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
	GnomeAppStorePrivate *priv = store->priv;
/*FIXME: should free lots of things ... */
	if (priv->file_uri) {
		g_free (priv->file_uri);
		priv->file_uri = NULL;
	}

	G_OBJECT_CLASS (gnome_app_store_parent_class)->finalize (object);
}

static void
gnome_app_store_class_init (GnomeAppStoreClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = gnome_app_store_dispose;
	object_class->finalize = gnome_app_store_finalize;
	 
	g_type_class_add_private (object_class, sizeof (GnomeAppStorePrivate));
}

GnomeAppStore *
gnome_app_store_new (void)
{
	return g_object_new (GNOME_TYPE_APP_STORE, NULL);
}

/**
 * gnome_app_store_get_apps
 *
 * get all the apps from the given store
 *
 * Return value: (transfer none): GSList 
 */

GSList *
gnome_app_store_get_apps (GnomeAppStore *store)
{
        GnomeAppStorePrivate *priv = store->priv;

	return priv->apps;
}

const gchar *
gnome_app_store_get_name_from_id (GnomeAppStore *store, gchar *id)
{
        GnomeAppStorePrivate *priv = store->priv;

	return g_hash_table_lookup (priv->id_to_name, id);
}

ClutterActor *
create_default_icon ()
{
	ClutterActor *actor;

	actor = clutter_texture_new_from_file ("/tmp/icons/default.png", NULL);
	clutter_actor_set_width (actor, 64);
	clutter_actor_set_height (actor, 64);
	return actor;
}
/**
 * gnome_app_store_get_icon_from_id
 *
 * Make up a decent icon for this application, 
 * FIXME: add more args, for it at the given size.
 *
 * Return value: (transfer none): A floating #ClutterActor
 */

/*FIXME: should clean the is default to the server side */
ClutterActor *
gnome_app_store_get_icon_from_id (GnomeAppStore *store, gchar *id)
{
        GnomeAppStorePrivate *priv = store->priv;
	const char *icon_name, *icon_uri;
	gchar *p, *content;

	ClutterActor *tex = NULL;

	icon_name = g_hash_table_lookup (priv->id_to_icon, id);
	if (icon_name) {
		content = g_strdup (icon_name);
		p = strrchr (content, '.');
		if (p)
			*p = 0;
	} else {
//FIXME: better way to clean the default icon store 				
		g_hash_table_insert (priv->default_icon_store, g_strdup (id), g_strdup (id));
		return create_default_icon ();
	}
	icon_uri = g_hash_table_lookup (priv->icon_store, content);

	if (icon_uri)
		tex = clutter_texture_new_from_file (icon_uri, NULL);

	if (!tex) {
		g_hash_table_insert (priv->default_icon_store, g_strdup (id), g_strdup (id));
		tex = create_default_icon ();
	}

	g_free (content);

	clutter_actor_set_width (tex, 64);
	clutter_actor_set_height (tex, 64);

	return CLUTTER_ACTOR (tex);
}

gboolean
gnome_app_store_is_default_icon (GnomeAppStore *store, gchar *id)
{
        GnomeAppStorePrivate *priv = store->priv;
	if (g_hash_table_lookup (priv->default_icon_store, id))
		return TRUE;
	return FALSE;
}

guint
gnome_app_store_get_counts (GnomeAppStore *store)
{
        GnomeAppStorePrivate *priv = store->priv;

	return g_slist_length (priv->apps);
}

const gchar *
gnome_app_store_get_nth_app (GnomeAppStore *store, guint pos)
{
        GnomeAppStorePrivate *priv = store->priv;
	GSList *l;
	guint i;

	for (i = 0, l = priv->apps; (i < pos) && l; i++, l = l->next) {
	}

	return (gchar *) l->data;
}


