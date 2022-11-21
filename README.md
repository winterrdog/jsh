# jSH
A simple implementation of a Unix shell, like Bash or Fish or ZSH, in C.

It demonstrates the basics of how a shell works.
That is: read, parse, fork, exec, and wait.  Since its purpose is demonstration, it has a lot
limitations, including:

* Commands must be on a single line :(.
* Arguments must be separated by whitespace.
* No quoting arguments or escaping whitespace.
* No piping or redirection.
* Only builtins are: `cd`, `help`, `exit`.

# Running
-------

Use `make` to compile, and then `./jsh` to run. 

# Contributing
------------

Any pull requests that are related to bug fixes or feature implementations (and I'm sure there are still bugs in the
code!) are welcome with open arms :).

However, in the meantime, you can play with the code, make changes,
and explore new features!  That's the whole point of this project!  It's just
that other people are doing the same thing, and this project is might be a
starting point for your own exploration.

# License
-------

This code is under MIT license.
