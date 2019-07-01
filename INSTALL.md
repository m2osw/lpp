
# Dependencies

First you need `git` to retrieve the source like so:

    git clone https://github.com/m2osw/lpp.git

If you do not have `git` installed, you can run the following on Ubuntu
to get it from the default Ubuntu repository:

    sudo apt-get install git

I assume you were able to get the source. So you have at least `git`
in order to clone the source from https://github.com/m2osw/lpp

If you are on Ubuntu, now you can run

    bin/install-ubuntu-packages

On other OSes, check that script to see what is required. This works
on Ubuntu 16.04.

# Compiling

I wrote a simple shell script that should do all of the work for
you, assuming you have all the necessary dependencies. Just type:

    bin/build

Again, that works on Ubuntu, you may need to make a few tweaks to
the script so it works on other OSes.

# Installing

I have a convenient script named `mk` which I use while doing development.
That script has a few command line options you can use to re-compile
and install and run the tests too.

The command accepts various flags like so:

    ./mk             normal compile
    ./mk -l          normal compile, output through `less`
    ./mk -i          compile and install
    ./mk -t          compile and test
    ./mk -d          rebuild the documentation

You may add the `-r` option first to switch to the Release (by default
you work with the Debug version.) For instance, to install the Release
you do:

    ./mk -r -i

**IMPORTANT NOTE:** The installation happens locally. Look at
`../BUILD/Debug/dist` and `../BUILD/Release/dist` for a list of the
installed files. For example you can use the `find` instruction:

    find ../BUILD/Debug/dist | less -S

To install in another location, you want to update the `bin/build`
script or create your own script. For our Debian package, the Debian
build environment is used so it uses a standard destination for the
files as required by that OS.

