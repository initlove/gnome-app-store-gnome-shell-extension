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

function _showHello() {
    let text = new St.Label({ style_class: 'helloworld-label', text: "David said: Harmony world!" });
    let monitor = global.get_primary_monitor();
    global.stage.add_actor(text);
    text.set_position(Math.floor (monitor.width / 2 - text.width / 2), Math.floor(monitor.height / 2 - text.height / 2));
    Mainloop.timeout_add(3000, function () { text.destroy(); });
}

// Put your extension initialization code here
function main() {
    Main.panel.actor.reactive = true;
    Main.panel.actor.connect('button-release-event', _showHello);

  
   let hotCornerButton = Main.panel.button;

   let logo = new St.Icon({ icon_type: St.IconType.FULLCOLOR, icon_size: hotCornerButton.height, icon_name: 'suse' });
   let box = new St.BoxLayout();

   let text = new St.Label({ style_class: 'corn-label', text: "dl" });
   box.add_actor(text);
   box.add_actor(logo);

   Main.panel.button.set_child(box);

   let store = new Store.AppStore ();
   let id = "gnome-volume-control.desktop";     
   let name;
   name = store.get_name_from_id (id);

   let text1 = new St.Label({ style_class: 'corn-label', text: name });
   let logo1 = new St.Icon({ icon_type: St.IconType.FULLCOLOR, icon_size: hotCornerButton.height, icon_name: 'suse' });
   let box1 = new St.BoxLayout({ vertical: true });

   let grid = new IconGrid.IconGrid({ xAlign: St.Align.START });
   box1.add(grid.actor, { y_align: St.Align.START, expand: true });
   grid.addItem (text1);
   grid.addItem (logo1);

//   let appIcon = new MyAppWellIcon(store, id);
//   grid.addItem(appIcon.actor);

   let appSystem = Shell.AppSystem.get_default();

   let selector = new Shell.GenericContainer({ name: 'gnome-app-store' });

   Main.overview.viewSelector.addViewTab('gnome-app-store', _("GNOME App Store"), box1);
}
