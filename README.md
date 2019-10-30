# qGetMirrorList

A tool to get and manipulate the latest Arch Linux Pacman mirror list. It targets those Linux distributions which use Arch mirrors. qGetMirrorList gets available data from the [Mirror Status service](https://www.archlinux.org/mirrors/status/) through the [JSON interface](https://www.archlinux.org/mirrors/status/json/) and lets the user generate customized Pacman mirror lists. It is inspired by the Python script [reflector](https://xyne.archlinux.ca/projects/reflector/) by [Xyne](https://xyne.archlinux.ca/).

## Dependencies

Required:

* Qt >= 5.13.1

Optional:

* rsync >= 3.1.3
* polkit >= 0.116

## Compilation

Execute `qmake` to generate a Makefile from the Qt project file, followed by `make` to do the actual compilation of the `qgetmirrorlist` binary.

## How to use

The UI is quite self-explanatory, but a few comments can be made.

### Get and select mirrors

The first thing to do is getting a complete, up to date mirror list by pressing the **Get mirror list** button. Once done, the table on the right is populated with mirrors and their properties. A mirror can be selected by pressing on its row. More than one mirror can be selected. Pressing on the table's top-left corner selects and deselects all of them. One can reorder the rows of the table, just drag and drop the numbered vertical headers. Note that mirror sorting done by pressing the column headers overrides custom drag and drop ordering.

### Mirror filters

All check boxes and the country list on the left group are used to filter the mirror list. More than one country can be selected. One can filter by protocol and IP version. Other filters include whether the mirrors are active, host ISOs and are syncing or out of sync. The button **Show all** can be pressed to undo all filters and return to the complete list.

### Table display

The table can display up to 12 columns, which can be toggled on/off with the correspoding check boxes in the **Columns** group, which is collapsible. The rows of the table can be sorted by pressing the columnn headers. Drag and drop of the headers reorders the columns. The meaning of the least obvious columns follows:

* Completion percentage: The number of mirror checks that have successfully connected and disconnected from the given URL. If this is below 100%, the mirror may be unreliable.
* Score: A very rough calculation for ranking mirrors. Lower is better. Note that out of sync mirrors have a value of 0.
* Speed: Download speed of the mirror. This column gets populated when some mirrors are selected and the button **Rank selected** is pressed. Units are KiB/s.
* Last sync: Date and time of last mirror synchronization. Missing if mirror is out of sync.
* Delay: The calculated average mirroring delay. Due to the timing of mirror checks, any value under one hour should be viewed as ideal.

For information about these features and more check the [Mirror Status service](https://www.archlinux.org/mirrors/status/).

### Actions

* **Rank selected**: Obtain the speed of the selected mirrors by downloading from them the `extra.db` database (a file less than 2MB in size). Support for rsync protocol is available if `rsync` optional dependency is met. Please use this action with care, do not rank many mirrors at once. This action can be canceled.
* **Save selected**: Save a mirror list with the selected mirrors on a chosen file. Do not select any mirror with rsync protocol if you plan to use the mirror list with Pacman. Use of rsync mirrors is supported by [powerpill](https://xyne.archlinux.ca/projects/powerpill/).
* **Update**: Copy the selected (http/https) mirrors to a mirror file with path `/etc/pacman.d/mirrorlist`, ready for Pacman to use. This requires root privileges, so a suitable window appears for the user to enter his/her password. This action is disabled if `polkit` optional dependency is not met.

### Tip

Get the mirror list and filter it according to your needs, e.g. select mirrors in or close to your country. Rank chosen mirrors and sort them by speed. Select those you prefer and save/update the mirror list.

## Screenshot

![Screenshot](https://i.imgur.com/3VUTQCp.jpg)

## Credits

Use was made of [FAMFAMFAM](http://www.famfamfam.com/lab/icons/) icons.

## License

This software is open source and available under the [GPLv3 License](LICENSE)
