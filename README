Geany Development Plugin for Common Lisp
========================================

**This "works on my machine" but is not well tested.  Use at your own risk**

Requirements
------------

* Linux (may work on other *nix, but is untested)
* [[http://geany.org/ | geany]] the oldest version I've tested with is 1.26
* You will need the plugin-dev headers for the above as well.
* A lisp executable (currently only tested with sbcl)
* GNU Emacs
* A working network connection is required when configuring a project in Geany
  * The plugin downloads a local quicklisp install to the project directory

Installing
----------

* `make config PREFIX=$HOME; make install`
* Enable the plugin from the geany plugin manager
* Configure keybindings from the geany plugin manager
* If you wish the "Compile" command to work, copy the `filetypes.lisp` to
  `~/.config/geany/filedefs` I don't install this by default so as to not clobber any
  existing customizations that may already be there.

Current Features
----------------

* Automatic Setup of ASDF and quicklisp for new projects
* Default the "Make' command to load the project's system into connected image
* Jump to symbol definition
* Smart indenting
* Compile current file
  * Highlight errors and warnings
* 
* Completion of symbols
* REPL

Using
-----

This plugin will not do anything until a project has been configured to use the plugin.
To do so, open the project you wish to enable to do so and select "Project->Enable GLisp
on project" from the menu.  This will install a local quicklisp to PROJECT_ROOT/quicklisp,
and create an ASDF template using quickproject in PROJECT_ROOT/lisp.  It will then alter
the project file with an option that includes the command for launching your lisp
executable.  This defaults to "sbcl --no-sysinit --no-userinit" if you change to a
different lisp implementation, ensure that similar options are passed to prevent personal
and system configuration initialization scripts from runing.

Known Bugs
----------

* Tabstop for indentation is hard-coded to 8.  It is strongly recommended that you
  configure geany to use spaces only for indentation for the time being.
   * Note that geany autodetects indentation, so if you open up a file with tabs it may
     locally use tabs for indentation
   * You can select "Project->Apply default indentation" to set the local file's
     indentation to be the same as your project's settings.

Unknown Bugs
------------

Probably a lot.
