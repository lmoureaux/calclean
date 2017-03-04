# Helper tools for HB {#hbanalyzer}

The number of hot cells in HB is strongly dependant on the threshold energy and
the value of @f$\eta@f$. Any energy threshold should therefore depend on
@f$\eta@f$. Finding such thresholds by hand is unrealistic. The helper tools can
be used to find them in a semi-automated way.

@tableofcontents

@note
Helper tools must be compiled using @c make from the top-level directory. The
interpreter is simply too slow to run them.

The HB helper tool is made of two parts: a C++ program, compiled from
@ref tools/hbanalyzer.cpp, and a [Lua](http://www.lua.org/) script,
@c tools/hbanalyzer.lua. The C++ program is used to extract information from a
ROOT file, which the script can then format in several different ways.

# Background {#background}

The basic assumption behind the HB helper tools is that the signal is
distributed uniformly in @f$\phi@f$, while the noise is not. The tools will
therefore try to find the lowest energy threshold for which the @f$\phi@f$
distribution is uniform. This is done independently for every @f$\eta@f$ bin.

The tools use P-values as a measure of uniformity. Distributions with small
P-values are considered non-uniform; the threshold is set to 1% by default.

One quickly notices, however, that disregarding a few cells can lower the energy
thresholds by a big factor, up to 2 in certain @f$\eta@f$ regions. The tools
support removing up to 19 cells per @f$\eta@f$ bin (out of 72). Cell removal
always happens at a fixed energy, by removing the cells with the highest hit
counts on the whole dataset. The basic unit here is the number of cells to be
removed.

# Extracting data {#extracting}

The first (and longest) part of the process is data extraction from a ROOT file.
This is done using the following command:

~~~
tools/hbanalyzer data.root [pvalue] >output_file
~~~

The file @c data.root should contain a @c TTree named @c CaloTree, as expected
by @ref calo::towerset "towerset". You are of course free to give it another
name. The optional parameter @c pvalue is the P-value above which a distribution
is considered uniform (see above). The default is 0.01. Since the tool writes
the data to standard output, the last "parameter" redirects it to the file of
your choice.

A typical invocation, reading data from @c data.root, setting the P-value to 5%
and writing to @c hbanalyzer.dat, would be as follows:

~~~
tools/hbanalyzer data.root 0.05 >hbanalyzer.dat
~~~

## Errors {#extracting-errors}

The tool's internal optimization process can sometimes fail, typically because
the P-value is too high. When this happens, an error message will be printed to
the terminal and the program will stop with a non-zero exit code.

Other errors most likely come from the ROOT file not being usable by
@ref calo::towerset "towerset".

# Formatting results {#formatting}

Once you have extracted data from your ROOT file, you'll need to use the
[Lua](http://www.lua.org/) script @c tools/hbanalyzer.lua in order to format it
to your needs. A general invocation of @c tools/hbanalyzer.lua looks like this:

~~~
lua tools/hbanalyzer.lua hbanalyzer.dat action args... >some_output_file
~~~

The file @c hbanalyzer.dat is the output from @c tools/hbanalyzer we created
before. @c action describes in which format you want the data to be printed, and
@c args is a set of additional arguments. The set of available actions is
documented below.

Most (but not all) formats use a file that specifies the number of cells to
remove in every @f$\eta@f$ bin. An example file can be found at
@c tools/hbanalyzer_counts.dat.

## Slices {#formatting-slices}

~~~
lua tools/hbanalyzer.lua hbanalyzer.dat slice N
~~~

This operation will print the energy thresholds above which the @f$\phi@f$
distribution is uniform in every @f$\eta@f$ bin. The parameter @c N is the
number of cells to remove; the more removed cells, the lower the thresholds.

Example output:

~~~
ieta    eta     energy
0       0.0425  1.45
1       0.1275  1.37
...
~~~

## Energy thresholds {#formatting-energy}

~~~
lua tools/hbanalyzer.lua hbanalyzer.dat energy counts.dat
~~~

This operation will print the energy thresholds above which the @f$\phi@f$
distribution is uniform in every @f$\eta@f$ bin. The file @c counts.dat should
contain the number of hot cells to remove in every @f$\eta@f$ bin.

Example output:

~~~
ieta    eta     numhot  energy
-17     -1.4025 0       0
-16     -1.3175 5       1.11
...
~~~

## Hot cells location {#formatting-hotcells}

~~~
lua tools/hbanalyzer.lua hbanalyzer.dat hotcells counts.dat
~~~

This operation will print the location of removed ("hot") cells. The file
@c counts.dat should contain the number of hot cells to remove in every
@f$\eta@f$ bin.

Example output:

~~~
ieta    eta     iphi    phi
0       0.0425  16      1.4398966328953
1       0.1275  16      1.4398966328953
...
~~~
