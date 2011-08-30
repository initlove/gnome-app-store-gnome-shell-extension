#include <stdio.h>
#include <string.h>

#ifndef GMENU_I_KNOW_THIS_IS_UNSTABLE
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#endif
#include <gmenu-tree.h>

#include "local-app.h"

static GSList *local_app_store = NULL;

static void
gather_entries_recurse (GMenuTreeDirectory *trunk)
{
        GSList *contents;
        GSList *iter;
        contents = gmenu_tree_directory_get_contents (trunk);

        for (iter = contents; iter; iter = iter->next) {
                GMenuTreeItem *item = iter->data;
                GMenuTreeEntry *entry;
                GMenuTreeDirectory *dir;
		const gchar *id;

                switch (gmenu_tree_item_get_type (item)) {
                        case GMENU_TREE_ITEM_ENTRY:
				entry = GMENU_TREE_ENTRY (item);
				id = gmenu_tree_entry_get_desktop_file_id (entry);
				local_app_store = g_slist_prepend (local_app_store, g_strdup (id));
                                break;
                        case GMENU_TREE_ITEM_DIRECTORY:
                                dir = GMENU_TREE_DIRECTORY (item);
                                gather_entries_recurse (dir);
                                break;
                        default:
                                break;
                }
		gmenu_tree_item_unref (item);
	}
	g_slist_free (contents);
}

static void
init_local_app_store ()
{
        GMenuTree *apps_tree;
        GMenuTreeDirectory *trunk;

        apps_tree = gmenu_tree_lookup ("applications.menu", GMENU_TREE_FLAGS_INCLUDE_NODISPLAY);
        trunk = gmenu_tree_get_root_directory (apps_tree);
	gather_entries_recurse (trunk);

        gmenu_tree_item_unref (trunk);
	gmenu_tree_unref (apps_tree);
}

/*FIXME: add monitor here */
gboolean
is_local_app (const char *id)
{
	if (!local_app_store)
		init_local_app_store ();

	GSList *l;
	gchar *local_id;
	for (l = local_app_store; l; l = l->next) {
		local_id = (gchar *) l->data;
		if (strcmp (id, local_id) == 0)
			return TRUE;
	}
	return FALSE;
}

void
debug_local_apps ()
{
	if (!local_app_store)
		init_local_app_store ();

	GSList *l;
	for (l = local_app_store; l; l = l->next) {
		printf ("id <%s>\n", l->data);
	}
}
