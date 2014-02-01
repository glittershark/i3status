// vim:ts=8:expandtab
#include <stdio.h>
#include <string.h>
#include <mpd/client.h>

#include "i3status.h"

#define OUTPUT_OPTION(option_name, tag_name) \
if (BEGINS_WITH(walk+1, option_name)) { \
        const char *value = mpd_song_get_tag(song, tag_name, 0); \
        if (value == NULL) { \
                *(outwalk++) = '?'; \
        } else { \
                outwalk += sprintf(outwalk, "%s", value); \
        } \
        walk += strlen(option_name); \
}

void print_mpd(yajl_gen json_gen, char *buffer, const char *format, const char *format_stopped) {
        const char *walk;
        char *outwalk = buffer;

        struct mpd_connection *conn;
        struct mpd_song *song;

        /* Use defaults */
        if ((conn = mpd_connection_new(NULL, 0, 0)) == NULL) {
                outwalk += sprintf(outwalk, "%s", format_stopped);
                OUTPUT_FULL_TEXT(buffer);
                return;
        }

        /* Get current song */
        song = mpd_run_current_song(conn);
        if (song == NULL) {
                outwalk += sprintf(outwalk, "%s", format_stopped);
                mpd_connection_free(conn);
                OUTPUT_FULL_TEXT(buffer);
                return;
        }

        walk = format;

        for (; *walk != '\0'; walk++) {
                if (*walk != '%') {
                        *(outwalk++) = *walk;
                        continue;
                }

                OUTPUT_OPTION("artist", MPD_TAG_ARTIST)
                OUTPUT_OPTION("album_artist", MPD_TAG_ALBUM_ARTIST)
                else OUTPUT_OPTION("album", MPD_TAG_ALBUM)
                OUTPUT_OPTION("title", MPD_TAG_TITLE)
                OUTPUT_OPTION("track", MPD_TAG_TRACK)
                OUTPUT_OPTION("name", MPD_TAG_NAME)
                OUTPUT_OPTION("genre", MPD_TAG_GENRE)
                OUTPUT_OPTION("date", MPD_TAG_DATE)
                OUTPUT_OPTION("composer", MPD_TAG_COMPOSER)
                OUTPUT_OPTION("performer", MPD_TAG_PERFORMER)
                OUTPUT_OPTION("comment", MPD_TAG_COMMENT)
                OUTPUT_OPTION("disc", MPD_TAG_DISC)
        }

        mpd_song_free(song);
        mpd_connection_free(conn);
        OUTPUT_FULL_TEXT(buffer);
        return;
}

