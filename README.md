# showdate
Command line utility to display date/time.
This can be either the current date/time, or creation or modifification date/time of a file.
By default the local time is show, but it's also possible to show UTC time (Coordinated Universal Time).

Usage: showdate [-hvu] [-f fmt] [-c file] [-m file]
Display a date in the specified format
  -h, --help          print this help and exit
  -v, --version       print version information and exit
  -f, --format=fmt    strftime() date/time format, default: "%Y-%m-%d %H:%M:%S"
  -u, --utc           show UTC time instead of local time
  -c, --created=file  use file creation date/time
  -m, --modified=file use file modification date/time
