# qGetMirrorList

A tool to get and manipulate Arch Linux pacman mirror list. It supports only those distributions which use Arch mirrors. Based on the online pacman [Mirrorlist Generator](https://www.archlinux.org/mirrorlist/).

## Dependencies

Besides Qt framework, two packages are needed for **rankmirrors** and **pkexec** binaries respectively.

* Qt >= 5.13.1
* pacman-contrib >= 1.1.0-1
* polkit >= 0.116-2

## Compilation

Execute `qmake` to generate a Makefile from the Qt project file, followed by `make` to do the actual compilation of the `qGetMirrorList` binary.

## How to use

The UI is quite self-explanatory, but a few comments can be made.

The first thing to do is get a complete, up to date mirror list by pressing the *Get mirror list* button. Once done, the table on the right is populated with mirrors and their properties, which can be filtered with the checkboxes and the list of countries on the left. More than one country can be selected. A mirror can be selected by pressing on its row. More than one mirror can be selected, while pressing on the table's horizontal header selects all of them. The button *Show all* can be pressed to return to the complete list.

The button *Rank selected* returns a list of the selected mirrors ordered by speed (note that depending on the number of selected mirrors this action can take long to finish; it can be cancelled). The button *Save selected* saves a mirror list with the selected mirrors on a chosen file. Finally, the button *Update* copies the selected mirrors to a mirror file with path `/etc/pacman.d/mirrorlist`, ready for pacman to use. This requires root privileges, so a suitable window appears for the user to enter his/her password.

## Screenshot

![Screenshot](https://i.imgur.com/1exqoqN.png)

## Credits

Use was made of [FAMFAMFAM](http://www.famfamfam.com/lab/icons/) icons.

## License

This software is open source and available under the [GPLv3 License](LICENSE)
