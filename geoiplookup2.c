/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <maxminddb.h>

/* Prototypes */
static char *get_myvalues (MMDB_lookup_result_s result, ...);

/********************
*  main()          *
********************/

int main(int argc, char **argv)
{
    char filename[100] = "";    /* arbitrary size; default GeoLite2-Country.mmdb pathname is 44 */
    char ip_address[39] = "";   /* max size of an IPv6 address */
    char *country = NULL;
    char *iso_code = NULL;
    int status = 0;
    int gai_error = 0;
    int mmdb_error = 0;
    int exit_code = 0;
    MMDB_s mmdb;

    /* check number of command line args */
    if(argc < 2)
      {
    printf("Usage: %s ip_addr [db_file]\n", argv[0]);
    exit(1);
      }

    strncpy(ip_address, argv[1], 39);

    /* if no db file specified, use the default */
    if(argc == 2)
      strncpy(filename, "/usr/local/var/GeoIP/GeoLite2-Country.mmdb", 100);
    else
      strncpy(filename, argv[2], 100);

    status = MMDB_open(filename, MMDB_MODE_MMAP, &mmdb);

    if (MMDB_SUCCESS != status)
      {
        fprintf(stderr, "\nCan't open %s - %s\n", filename, MMDB_strerror(status));

        if (MMDB_IO_ERROR == status)
      {
            fprintf(stderr, "IO error: %s\n", strerror(errno));
      }
        exit(1);
      }

    MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb, ip_address, &gai_error, &mmdb_error);

    if (0 != gai_error)
      {
        fprintf(stderr,"\nError from getaddrinfo for %s - %s\n\n", ip_address, gai_strerror(gai_error));
        exit(2);
      }

    if (MMDB_SUCCESS != mmdb_error)
      {
        fprintf(stderr, "\nError from libmaxminddb: %s\n\n", MMDB_strerror(mmdb_error));
        exit(3);
      }

    MMDB_entry_data_s entry_data;

    if (result.found_entry)
      {
        status = MMDB_get_value(&result.entry, &entry_data, NULL);

        if (MMDB_SUCCESS != status)
         {
            fprintf(stderr, "Error looking up the entry data - %s\n", MMDB_strerror(status));
            exit_code = 4;
            goto end;
         }

       if(entry_data.has_data)
        {
          country = get_myvalues(result, "country", "names", "en",  NULL);
          iso_code= get_myvalues(result, "country", "iso_code", NULL);
          printf("%s, %s\n", iso_code, country);
         }
      }
    else
      {
        fprintf(stderr, "\nNo entry for this IP address (%s) was found\n\n", ip_address);
        exit_code = 5;
      }

    end:
        MMDB_close(&mmdb);
        exit(exit_code);
}

/********************
*  get_myvalues()  *
********************/

static char *get_myvalues (MMDB_lookup_result_s result, ...)
{
MMDB_entry_data_s entry_data;
char *value = NULL;
int status = 0;
va_list keys;

va_start (keys, result);

status = MMDB_vget_value (&result.entry, &entry_data, keys);

if (status != MMDB_SUCCESS)
   printf("Error from libmaxminddb: %s\n", MMDB_strerror (status));

va_end (keys);

if (entry_data.type != MMDB_DATA_TYPE_UTF8_STRING)
  printf("Invalid data UTF8 GeoIP2 data %d:\n", entry_data.type);

if ((value = strndup (entry_data.utf8_string, entry_data.data_size)) == NULL)
  printf("Unable to allocate buffer %s: ", strerror (errno));

return value;
}
