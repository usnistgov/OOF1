INSTALLATION NOTES FOR OOF AND PPM2OOF

0.  Unpack the source files, with "tar -xvf distributionfile", or
    "zcat distributionfile.gz | tar -xvf -" if the file is compressed.
    This will create a directory called something like
    "oof-dist1.001".

1.  Run the configure script.  If you are compiling more than one
    version (eg, using the same copy of the source code for different
    machine architectures, or compiling both the thermal and
    elastic-only versions) you'll want to do each compilation in a
    separate directory.  Make a compilation directory anywhere you
    like;  we'll assume it's oof-dist1.001/build.  You can compile in
    the source directory (oof-dist1.001) if you like, though.

    If your C++ compiler isn't the standard one on your system, set
    the CXX environment variable to the name of your C++ compiler, eg,
    "setenv CXX pgCC".

    In the compilation directory, run "configure" (ie, ../configure if
    you're compiling in oof-dist1.001/build) with the appropriate
    options.  The more important options are these:
     --with-cxxflags=-DTHERMAL      (include thermal conductivity code)
     --with-libdirs=-L<library directories>   (where to find libraries)
     --with-extra-libs=-l<libraries>          (additional libraries to include) 
     --with-blas=< >   (how to find the BLAS library)
         For example, if your BLAS library is in /home/me/myblas/libmyblas.a,
         and requires the Fortran library, use
          --with-blas="-L/home/me/myblas -lmyblas -lftn".
     --help                  (get a list of all the configuration options)

    Note that the optimization is on by default and debugging is off.

    Note that if you use the --with-blas option, you have to also
    supply the names of all the other libraries that your BLAS library
    requires, as shown in the example above.

    If your compiler is gcc 3.x (probably for all x, at least for
    x=1),  you'll have to include -DNEED_FDSTREAM in the
    --with-cxxflags option.  This works around a non-standard C++
    library feature that gcc 3 doesn't support.

    If your compiler is gcc 3.3, you'll have to include
    -DDONT_HAVE_STRSTREAM_H in the --with-cxxflags option.  This flag
    is included automatically on Macintosh OS X 10.3.

    Note: to include multiple flags, use quotation marks, like this:
         configure --with-cxxflags='-flag1 -flag2'
    or this:
         configure '--with-cxxflags=-flag1 -flag2'

    As examples, here are the configure arguments that have been used on
    various systems:

    For SGI, linking in a static fortran version of the BLAS library:
	../configure  --with-libdirs=-L/users/langer/FE/lib/libn32o \
                      --with-blas='-B static -lblas -Bdynamic -lftn'
    ~langer/FE/lib/libn32o contains static versions of the BLAS and
    XForms libraries.  If your executable won't be distributed, you
    probably don't need this.

    For Sun, 
       setenv CXX g++
       ../configure  --with-libdirs=-L/users/langer/FE/lib/lib-solaris2 \
                      --x-includes=/usr/include/X11
    Again, the compiler is pointed toward static versions of the libraries.
    The default X11 header files are in /usr/openwin/include/X11, but
    those don't work with g++.

    For DEC Alpha running Linux,
       ../configure  --with-cxxoptimize=-O1
    On our system, the BLAS and XForms libraries are installed in the
    standard places, but the compiler has a bug that creates bad code
    at the standard optimization.

    On a PC laptop running Linux,
       ../configure
    Since this version isn't distributed, the standard shared libraries
    can be used.

    On a PC running FreeBSD,
       ../configure --with-cxxflags=-D__cplusplus \
		    --with-libdirs=-L/usr/local/lib \
		    --with-blas="-lblas -lf2c"
    (thanks to Konrad Heuer from Gottingen).

    On a Macintosh running OS X, you will need to install an X server.
    Apple's X11.app is distributed with OS X 10.3 and later. For
    earlier versions of OS X, we've used fink
    (http://fink.sourceforge.net) to install the xfree86 server, as
    well as the xforms library.  (But see the note about xforms in
    item 2, below.)  The blas library is included as part of the
    veclib framework with OS X 10.2 and later.  For earlier versions
    of OS X, you can get the blas library by installing the atlas
    package from fink.  (I don't know if veclib exists for G3
    processors -- if you don't have a G4 you may need atlas even with
    10.2.  Atlas is currently in the unstable distribution on fink.
    Instructions for using unstable packages are at
    http://fink.sourceforge.net/faq/usage-fink.php#unstable.)
 
    Here are the configure settings we've used:
    On a Macintosh running OS X 10.1,
      ../configure  '--with-blas=-L/sw/lib -lf77blas -latlas -lg2c' \
                    --host=powerpc
    On a Macintosh, running OS X 10.3,
      ../configure -with-blas="-faltivec -framework vecLib" \
                   -with-cxxflags="-no-cpp-precomp"

2.  If the configure script fails, it's probably because you don't
    have all the relevant libraries.  Look in config.log for clues to
    what went wrong.  The two libraries that might cause problems are
    the XForms library for the user interface code, and the BLAS
    library for basic linear algebra.  If you have the libraries but
    they're not in standard places or don't have standard names,
    you'll need to use the --with-blas, --with-libdirs and --with-libs
    flags to tell configure where to find them.  If you don't have the
    libraries, you'll have to get them.

    XForms is (was?) available from http://world.std.com/~xforms/. OOF
    was developed with version 0.89, but appears to work with later
    versions, which may be obtained at
    http://savannah.nongnu.org/projects/xforms/.

    Most installations already have BLAS installed.  If you need it,
    you can get it from http://www.netlib.org/.  Optimized versions
    for some Pentium chips can be found at
    http://www.cs.utk.edu/~ghenry/distrib/.

3.  Compile the programs.  If your compilation directory isn't the
    distribution directory, you'll have to use the GNU make utility.
    Type "make all", "make oof", or "make ppm2oof" in the compilation
    directory.  The executable programs will be XOOF/oof and
    XPPM2OOF/ppm2oof. Feel free to move them to a more convenient
    locations. 

4.  If you need to edit the Makefiles to get the programs to compile,
    please tell us at oof_manager@ctcms.nist.gov.  Be sure to tell us
    what kind of computer you're using.  Adjustments to Makefiles that
    are permanent should be made to 'Makefile.in' in the appropriate
    source directory (oof-dist1.001/XOOF or oof-dist1.001/XPPM2OOF).
    Temporary changes can be made to the Makefile in the build dir. To
    re-build makefiles using the same configuration arguments, run
    './config.status' in the build dir. You don't need to do the full
    configure unless configure.in has to be changed for some reason.

5.  If you need help, send mail to oof_manager@ctcms.nist.gov.  Please
    tell us what kind of computer, what operating system, and which
    OOF distribution you have.  Let us know what arguments you
    supplied to configure.  Send us the exact error messages, if any.
    If configure failed, include the config.log file.
