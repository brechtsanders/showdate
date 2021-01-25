# showdate
Command line utility to display date/time.
The date/time to be displayed can be either the current date/time (default), or the creation or modification date/time of a file or directory.
By default the local time is shown, but it's also possible to show UTC time (Coordinated Universal Time).
The format used can be specified, using the same format string as the C function `strftime()`.

## command line help

```
Usage: showdate [-hvu] [-f fmt] [-c file] [-m file]
Display date/time in the specified format
  -h, --help          print this help and exit
  -v, --version       print version information and exit
  -f, --format=fmt    strftime() date/time format, default: "%Y-%m-%d %H:%M:%S"
  -u, --utc           show UTC time instead of local time
  -c, --created=file  use file creation date/time
  -m, --modified=file use file modification date/time
```
