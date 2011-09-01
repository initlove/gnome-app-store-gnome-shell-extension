#include <stdlib.h>
#include <math.h>
#include <gmodule.h>
#include <clutter/clutter.h>

#include "gnome-app-store.h"
#include "gnome-app-stage.h"

int
main (int argc, char *argv[])
{
	ClutterActor *stage;
	ClutterActor *scroll, *box;
	gchar *id;
	gint i;

	if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
	  return 1;

	stage = clutter_stage_new ();
	clutter_stage_set_title (CLUTTER_STAGE (stage), "Scrolling");
	clutter_actor_set_size (stage, 800, 600);
	g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

#if 1
	/* scroll: the group that contains the scrolling viewport; we set its
	 * size to be the same as one rectangle, position it in the middle of
	 * the stage and set it to clip its contents to the allocated size
	 */
	scroll = gnome_app_stage_new ();

	clutter_container_add_actor (CLUTTER_CONTAINER (stage), scroll);
	      
	GnomeAppStore *store;
	store = gnome_app_store_new ();
	guint  counts = gnome_app_store_get_counts (store);
printf ("counts %d\n", counts);
counts = 30;
	for (i = 0; i < counts; i++)
	  {
	              id = gnome_app_store_get_nth_app (store, i);
	        box = gnome_app_store_get_icon_from_id (store, id);
		if (gnome_app_store_is_default_icon (store, id) == FALSE) {
			gnome_app_stage_add_actor (GNOME_APP_STAGE (scroll), box);
		}
	  }
#else
	GnomeAppStore *store;
	store = gnome_app_store_new ();
	id = gnome_app_store_get_nth_app (store, i);
	box = gnome_app_store_get_icon_from_id (store, id);
	clutter_container_add_actor (CLUTTER_CONTAINER (stage), box);
#endif
	clutter_actor_show (stage);

	clutter_main ();

	return EXIT_SUCCESS;
}
