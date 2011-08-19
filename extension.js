// Sample extension code, makes clicking on the panel show a message
const Overview = imports.ui.overview;
const St = imports.gi.St;
const Mainloop = imports.mainloop;
const IconGrid = imports.ui.iconGrid;
const Main = imports.ui.main;
const Shell = imports.gi.Shell;
const Store = imports.gi.GnomeAppStore;

function MyAppIcon(app, params) {
    this._init(app, params);
}

MyAppIcon.prototype = {
    __proto__:  IconGrid.BaseIcon.prototype,

    _init : function(store, id, params) {
        let label = store.get_name_from_id (id);

        IconGrid.BaseIcon.prototype._init.call(this,
                                               label,
                                               params);
    },
};

function MyAppWellIcon(app, iconParams) {
    this._init(app, iconParams);
}

MyAppWellIcon.prototype = {
    _init : function(store, id, iconParams) {
        this.actor = new St.Button({ style_class: 'myapp-well-app',
                                     reactive: true,
                                     button_mask: St.ButtonMask.ONE | St.ButtonMask.TWO,
                                     can_focus: true,
                                     x_fill: true,
                                     y_fill: true });
        this.actor._delegate = this;

        this.icon = new MyAppIcon(store, id, iconParams);
        this.actor.set_child(this.icon.actor);

        this.actor.label_actor = this.icon.label;
    }
}

// Put your extension initialization code here
function main() {
//-------------------app store -----------
   let box1 = new St.BoxLayout({ vertical: true });
   let grid = new IconGrid.IconGrid({ xAlign: St.Align.START });
   box1.add(grid.actor, { y_align: St.Align.START, expand: true });

   let store = new Store.AppStore ();
   for (let i=0; i<20; i++) {
	let id = store.get_nth_app (i);
	let name = store.get_name_from_id (id);
   	let text1 = new St.Label({ style_class: 'corn-label', text: name });
   	grid.addItem (text1);
    }

//   let appIcon = new MyAppWellIcon(store, id);
//   grid.addItem(appIcon.actor);

   let selector = new Shell.GenericContainer({ name: 'gnome-app-store' });

   Main.overview.viewSelector.addViewTab('gnome-app-store', _("GNOME App Store"), box1);

}
