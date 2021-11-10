//  Copyright 2021 Eremey Remzin
#include "test_utiles.h"

#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

static int check_type(const char* file_name) {
    char* text_format = ".txt";

    if (strlen(file_name) < strlen(text_format) + 1) {
        return 0;
    }

    size_t j = 0;
    for (size_t i = strlen(file_name) - strlen(text_format);
         i < strlen(file_name); ++i) {
        if (file_name[i] != text_format[j]) {
            return 0;
        }
        ++j;
    }
    return 1;
}

int free_directory(char path[200]) {
    DIR *directory = opendir(path);
    if (directory == NULL) {
        return -1;
    }
    char (*file_paths)[512] = malloc(0);
    size_t number_of_files = 0;
    struct dirent *file = NULL;

    while ((file = readdir(directory)) != NULL) {
        if (check_type(file->d_name) == 1) {
            ++number_of_files;
            file_paths = realloc(file_paths, number_of_files * sizeof(*file_paths));
            file_paths[number_of_files-1][0] = '\0';
            strcat(file_paths[number_of_files-1], path);
            strcat(file_paths[number_of_files-1], "/");
            strcat(file_paths[number_of_files-1], file->d_name);
        }
    }

    for (int i = 0; i < number_of_files; ++i) {
        if(remove(file_paths[i]) == -1){
            closedir(directory);
            free(file_paths);
            return -1;
        }
    }
    closedir(directory);
    free(file_paths);
    return 0;
}

static int add_1mb_file(char file_path[256], char word[10]) {
    FILE *f = fopen(file_path, "w");
    if (f == NULL) {
        return -1;
    }

    char text[1005] = "";
    for (int i = 0; i < 1005 / strlen(word) - 1; ++i) {
        strcat(text, word);
    }
    fprintf(f, "%s", text);
    fclose(f);
    return 0;
}

static int add_1mb_file_with_keywords(char file_path[256] , size_t number) {
    size_t free_bytes = 1000;
    char word[10] = "Test";
    free_bytes -= number * strlen(word);
    FILE *f = fopen(file_path, "w");
    if (f == NULL) {
        return -1;
    }

    char text[1000] = "";
    for (int i = 0; i < number ; ++i) {
        strcat(text, word);
    }

    char* noise = "-----";
    for (int i = 0; i < free_bytes / strlen(noise) ; ++i) {
        strcat(text, noise);
    }
    fprintf(f, "%s", text);
    fclose(f);
    return 0;
}

int add_250k_Test(char path[200]) {
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/1MB_filled_Test.txt");

    char keyword[10] = "Test";
    add_1mb_file(file_path, keyword);
    return 0;
}

int add_empty_file(char path[200]) {
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/empty_file.txt");
    FILE *f = fopen(file_path, "w");
    if (f == NULL) {
        return -1;
    }
    fprintf(f, " ");
    fclose(f);
    return 0;
}


int add_filled_noise_file(char path[200]) {
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/1MB_filled_Noise.txt");

    char noise[10] = "-----";
    add_1mb_file(file_path, noise);

    return 0;
}

int add_filled_keyword_file_with_spaces(char path[200]) {
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/1MB_filled_Test_with_spaces.txt");

    char keyword[10] = "Test ";
    add_1mb_file(file_path, keyword);

    return 0;
}

int add_1MB_files(char path[200], size_t quantity) {
    if(quantity > 9){
        return -1;
    }
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/1MB_kw_nubmer__.txt");
    for (size_t i = 0; i < quantity; ++i) {
        file_path[strlen(file_path) - (1 + strlen(".txt"))] = (char)i +'0';
        add_1mb_file_with_keywords(file_path, i + 1);
    }
    return 0;
}

/*
* Author:  David Robert Nadeau
        * Site:    http://NadeauSoftware.com/
* License: Creative Commons Attribution 3.0 Unported License
*          http://creativecommons.org/licenses/by/3.0/deed.en_US
*/
double getCPUTime() {
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    FILETIME createTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if ( GetProcessTimes( GetCurrentProcess( ),
        &createTime, &exitTime, &kernelTime, &userTime ) != -1 )
    {
        SYSTEMTIME userSystemTime;
        if ( FileTimeToSystemTime( &userTime, &userSystemTime ) != -1 )
            return (double)userSystemTime.wHour * 3600.0 +
                (double)userSystemTime.wMinute * 60.0 +
                (double)userSystemTime.wSecond +
                (double)userSystemTime.wMilliseconds / 1000.0;
    }

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
    /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    /* Prefer high-res POSIX timers, when available. */
    {
        clockid_t id;
        struct timespec ts;
#if _POSIX_CPUTIME > 0
        /* Clock ids vary by OS.  Query the id, if possible. */
        if ( clock_getcpuclockid( 0, &id ) == -1 )
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
        /* Use known clock id for AIX, Linux, or Solaris. */
        id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
        /* Use known clock id for BSD or HP-UX. */
            id = CLOCK_VIRTUAL;
#else
            id = (clockid_t)-1;
#endif
        if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
            return (double)ts.tv_sec +
                   (double)ts.tv_nsec / 1000000000.0;
    }
#endif

#if defined(RUSAGE_SELF)
    {
        struct rusage rusage;
        if ( getrusage( RUSAGE_SELF, &rusage ) != -1 )
            return (double)rusage.ru_utime.tv_sec +
                   (double)rusage.ru_utime.tv_usec / 1000000.0;
    }
#endif

#if defined(_SC_CLK_TCK)
    {
        const double ticks = (double)sysconf( _SC_CLK_TCK );
        struct tms tms;
        if ( times( &tms ) != (clock_t)-1 )
            return (double)tms.tms_utime / ticks;
    }
#endif

#if defined(CLOCKS_PER_SEC)
    {
        clock_t cl = clock( );
        if ( cl != (clock_t)-1 )
            return (double)cl / (double)CLOCKS_PER_SEC;
    }
#endif

#endif

    return -1;      /* Failed. */
}
