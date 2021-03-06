#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <argtable2.h>

#define SHOWDATE_VERSION_MAJOR 0
#define SHOWDATE_VERSION_MINOR 1
#define SHOWDATE_VERSION_MICRO 2

#define APPLICATION_NAME "showdate"

#define DEFAULT_DATETIME_FORMAT "%Y-%m-%d %H:%M:%S"

#define DATETIME_ENTITY_SECOND  0
#define DATETIME_ENTITY_MINUTE  1
#define DATETIME_ENTITY_HOUR    2
#define DATETIME_ENTITY_DAY     3
#define DATETIME_ENTITY_COUNT   4

const time_t datetime_entity_value[DATETIME_ENTITY_COUNT] = {
  1,
  60,
  3600,
  86400
};

int main (int argc, char **argv)
{
  int argerrors;
  struct arg_lit* help;
  struct arg_lit* version;
  struct arg_str* format;
  struct arg_lit* show_utc;
  struct arg_str* use_file_created;
  struct arg_str* use_file_modified;
  struct arg_int* datetime_add[DATETIME_ENTITY_COUNT];
  struct arg_int* datetime_sub[DATETIME_ENTITY_COUNT];
  struct arg_end* end;
  void* argtable[] = {
    help              = arg_lit0("h", "help",             "print this help and exit"),
    version           = arg_lit0("v", "version",          "print version information and exit"),
    format            = arg_str0("f", "format",   "fmt",  "strftime() date/time format, default: \"" DEFAULT_DATETIME_FORMAT "\""),
    show_utc          = arg_lit0("u", "utc",              "show UTC time instead of local time"),
    use_file_created  = arg_str0("c", "created",  "file", "use file creation date/time"),
    use_file_modified = arg_str0("m", "modified", "file", "use file modification date/time"),
    datetime_add[DATETIME_ENTITY_DAY]    = arg_int0(NULL, "add-days",    "n", "add n days to date/time"),
    datetime_sub[DATETIME_ENTITY_DAY]    = arg_int0(NULL, "sub-days",    "n", "subtract n days from date/time"),
    datetime_add[DATETIME_ENTITY_HOUR]   = arg_int0(NULL, "add-hours",   "n", "add n hours to date/time"),
    datetime_sub[DATETIME_ENTITY_HOUR]   = arg_int0(NULL, "sub-hours",   "n", "subtract n hours from date/time"),
    datetime_add[DATETIME_ENTITY_MINUTE] = arg_int0(NULL, "add-minutes", "n", "add n minutes to date/time"),
    datetime_sub[DATETIME_ENTITY_MINUTE] = arg_int0(NULL, "sub-minutes", "n", "subtract n minutes from date/time"),
    datetime_add[DATETIME_ENTITY_SECOND] = arg_int0(NULL, "add-seconds", "n", "add n seconds to date/time"),
    datetime_sub[DATETIME_ENTITY_SECOND] = arg_int0(NULL, "sub-seconds", "n", "subtract n seconds from date/time"),
    end               = arg_end(20)
  };
  const char* progname = APPLICATION_NAME;
  //check argtable entries were sucessfully allocated
  if (arg_nullcheck(argtable) != 0) {
    fprintf(stderr, "Memory allocation error\n");
    arg_freetable(argtable, sizeof(argtable) / sizeof(*argtable));
    return 1;
  }
  //parse command line arguments
  argerrors = arg_parse(argc, argv, argtable);
  //show help if requested (takes precedence over error reporting)
  if (help->count > 0) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    printf("Display date/time in the specified format\n");
    arg_print_glossary(stdout, argtable, "  %-19s %s\n");
    arg_freetable(argtable, sizeof(argtable) / sizeof(*argtable));
    return 0;
  }
  //show version information if requested (takes precedence over error reporting)
  if (version->count > 0) {
    printf("%s %i.%i.%i\n", progname, SHOWDATE_VERSION_MAJOR, SHOWDATE_VERSION_MINOR, SHOWDATE_VERSION_MICRO);
    arg_freetable(argtable, sizeof(argtable) / sizeof(*argtable));
    return 0;
  }
  //check if parser failed
  if (argerrors > 0) {
    arg_print_errors(stderr, end, progname);
    fprintf(stderr, "For information on correct usage run: '%s --help'\n", progname);
    arg_freetable(argtable, sizeof(argtable) / sizeof(*argtable));
    return 2;
  }
  //check for conflicting options
  if (use_file_created->count > 0 && use_file_modified->count > 0) {
    fprintf(stderr, "Only file creation or modified date/time may be specified, not both\n");
    arg_freetable(argtable, sizeof(argtable) / sizeof(*argtable));
    return 4;
  }

  //get timestamp
  time_t timestamp;
  if (use_file_created->count > 0) {
    struct stat fileinfo;
    if (stat(use_file_created->sval[0], &fileinfo) != 0) {
      fprintf(stderr, "Error accessing file %s\n", use_file_created->sval[0]);
      arg_freetable(argtable, sizeof(argtable) / sizeof(*argtable));
      return 5;
    }
    timestamp = fileinfo.st_ctime;
  } else if (use_file_modified->count > 0) {
    struct stat fileinfo;
    if (stat(use_file_modified->sval[0], &fileinfo) != 0) {
      fprintf(stderr, "Error accessing file %s\n", use_file_modified->sval[0]);
      arg_freetable(argtable, sizeof(argtable) / sizeof(*argtable));
      return 5;
    }
    timestamp = fileinfo.st_mtime;
  } else {
    timestamp = time(NULL);
  }

  //adjust timestamp if requested
  int i;
  int j;
  for (i = 0; i < DATETIME_ENTITY_COUNT; i++) {
    for (j = 0; j < datetime_add[i]->count; j++)
      timestamp += datetime_entity_value[i] * datetime_add[i]->ival[j];
    for (j = 0; j < datetime_sub[i]->count; j++)
      timestamp -= datetime_entity_value[i] * datetime_sub[i]->ival[j];
  }

  //print timestamp
  char buf[128];
  struct tm* timeinfo;
  timeinfo = (show_utc->count > 0 ? gmtime(&timestamp) : localtime(&timestamp));
  strftime(buf, sizeof(buf), (format->count > 0 ? format->sval[0] : DEFAULT_DATETIME_FORMAT), timeinfo);
  printf("%s\n", buf);

  //cleanup
  arg_freetable(argtable, sizeof(argtable) / sizeof(*argtable));
  return 0;
}
