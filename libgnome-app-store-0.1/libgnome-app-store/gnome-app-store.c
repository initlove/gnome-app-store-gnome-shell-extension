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

#include <clutter/clutter.h>


#include "local-app.h"
#include "gnome-app-config.h"
#include "gnome-app-store.h"

struct _GnomeAppStorePrivate
{
	gchar *			cache_dir;

	GSList *		apps;
	GSList *		default_icon_store;
	GHashTable *		id_to_name;
	GHashTable *		id_to_pkgname;
	GHashTable *		id_to_icon;
	GHashTable *		icon_store;
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
	gchar *p;

        for (node = app_node->xmlChildrenNode; node; node = node->next) {
                type = get_type_from_name ((gchar *)(node->name));
                switch (type) {
                        case PKG_ID:
				p = xmlNodeGetContent (node);
#ifdef DEBUG
                                printf ("desktp id %s\t", p);
#endif
				/*filter the local app
					FIXME: should add a monitor to refresh */
				if (is_local_app (p)) {
					return;
				}

				app_id = g_strdup (p);
				priv->apps = g_slist_append (priv->apps, app_id);
                                break;
                        case PKG_PKGNAME:
#ifdef DEBUG
				printf ("pkgfull %s\t", xmlNodeGetContent (node));
#endif
				g_hash_table_insert (priv->id_to_pkgname, g_strdup (app_id), g_strdup (xmlNodeGetContent (node)));
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
	gchar *file_uri;

	file_uri = g_build_filename (priv->cache_dir, "appdata.xml", NULL);

        doc_ptr = xmlParseFile (file_uri);

        root_node = xmlDocGetRootElement (doc_ptr);

        for (app_node = root_node->xmlChildrenNode; app_node; app_node = app_node->next) {
                if (strcmp (app_node->name, "application") == 0) {
                        add_app (store, app_node);
                }
        }

        xmlFreeDoc(doc_ptr);
	g_free (file_uri);
}

static void
load_icons (GnomeAppStore *store)
{
	GDir *dir;
	const gchar *basename;
	gchar *filename;
	gchar *p, *content;
	GnomeAppStorePrivate *priv = store->priv;

	gchar *icon_dir;
	icon_dir = g_build_filename (priv->cache_dir, "icons", NULL);
	dir = g_dir_open (icon_dir, 0, NULL);

	if (!dir) {
		printf ("cannot open dir\n");
		g_free (icon_dir);
		return;
	}
                
	while ((basename = g_dir_read_name (dir)) != NULL) {
		filename = g_build_filename (icon_dir, basename, NULL);
		content = g_strdup (basename);
		p = strrchr (content, '.');
		if (p)
			*p = 0;
                        
		g_hash_table_insert (priv->icon_store, content, filename);
	}
	g_dir_close (dir);
	g_free (icon_dir);
}

static void
gnome_app_store_init (GnomeAppStore *store)
{
	GnomeAppStorePrivate *priv;

	store->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (store,
                                                   GNOME_TYPE_APP_STORE,
                                                   GnomeAppStorePrivate);

	priv->apps = NULL;
	priv->default_icon_store = NULL;

	priv->id_to_name = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	priv->id_to_pkgname = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	priv->id_to_icon = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	priv->icon_store = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

	GnomeAppConfig *conf;
	conf = gnome_app_config_new ();
	priv->cache_dir = gnome_app_config_get_cache_dir (conf);
	g_object_unref (conf);

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
	if (priv->cache_dir) {
		g_free (priv->cache_dir);
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
gnome_app_store_get_pkgname_from_id (GnomeAppStore *store, const gchar *id)
{
        GnomeAppStorePrivate *priv = store->priv;

	return g_hash_table_lookup (priv->id_to_pkgname, id);
}

const gchar *
gnome_app_store_get_name_from_id (GnomeAppStore *store, const gchar *id)
{
        GnomeAppStorePrivate *priv = store->priv;

	return g_hash_table_lookup (priv->id_to_name, id);
}

static gboolean
on_app_clicked (gpointer userdata)
{
	gchar *pkgname = (gchar *) userdata;
	printf ("on app clicked %s\n", pkgname);
                
	gchar *cmd;
	cmd = g_strdup_printf ("/sbin/YaST2 -i %s &", pkgname);
	system (cmd);
	g_free (cmd);
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
gnome_app_store_get_icon_from_id (GnomeAppStore *store, const gchar *id)
{
        GnomeAppStorePrivate *priv = store->priv;
	const char *icon_name, *icon_uri;
	const char *app_name, *pkg_name;

	icon_name = g_hash_table_lookup (priv->id_to_icon, id);
	if (icon_name) {
		gchar *p, *content;
		content = g_strdup (icon_name);
		p = strrchr (content, '.');
		if (p)
			*p = 0;
		icon_uri = g_hash_table_lookup (priv->icon_store, content);
		g_free (content);
	} else {
		icon_uri = NULL;
	}

	app_name = gnome_app_store_get_name_from_id (store, id);

	ClutterActor *actor, *box, *text;
	ClutterActor *icon = NULL; //*FIXME we have icon, but can not be recognized by clutter
	ClutterLayoutManager *layout;
	ClutterAction *action;

	layout = clutter_box_layout_new ();
	clutter_box_layout_set_vertical (CLUTTER_BOX_LAYOUT (layout), TRUE);
	clutter_box_layout_set_spacing (CLUTTER_BOX_LAYOUT (layout), 6);

	box = clutter_box_new (layout);
	text = clutter_text_new ();
	clutter_text_set_ellipsize (CLUTTER_TEXT (text), PANGO_ELLIPSIZE_END);
	clutter_text_set_text (CLUTTER_TEXT (text), app_name);
	clutter_actor_set_width (text, 64);
	clutter_container_add_actor (CLUTTER_CONTAINER (box), text);

	if (icon_uri)
		icon = clutter_texture_new_from_file (icon_uri, NULL);

	if (!icon) {
//FIXME: better way to clean the default icon store 				
		priv->default_icon_store = g_slist_prepend (priv->default_icon_store, g_strdup (id));
		gchar *default_icon;
		default_icon = g_build_filename (priv->cache_dir, "icons", "default.png", NULL); 
		icon = clutter_texture_new_from_file (default_icon, NULL);
		g_free (default_icon);
	}
	clutter_actor_set_width (icon, 64);
	clutter_actor_set_height (icon, 64);
	clutter_container_add_actor (CLUTTER_CONTAINER (box), icon);
	clutter_actor_set_reactive (box, TRUE);

	pkg_name = gnome_app_store_get_pkgname_from_id (store, id);
 	g_signal_connect_swapped (box, "button-press-event",
                            G_CALLBACK (on_app_clicked), pkg_name);

	return CLUTTER_ACTOR (box);
}

/*FIXME: should be used after 
	gnome_app_store_get_icon_from_id
*/
gboolean
gnome_app_store_is_default_icon (GnomeAppStore *store, const gchar *id)
{
        GnomeAppStorePrivate *priv = store->priv;
	GSList *l;

	for (l = priv->default_icon_store; l; l = l->next) {
		if (strcmp (id, l->data) == 0) {
			return TRUE;
		}
	}

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


