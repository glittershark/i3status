// vim:ts=8:expandtab
#include <stdio.h>
#include <string.h>
#include <mpd/client.h>

#include "i3status.h"

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
                goto out;
        }

        walk = format;

        for (; *walk != '\0'; walk++) {
                if (*walk != '%') {
                        *(outwalk++) = *walk;
                        continue;
                }

                if (BEGINS_WITH(walk+1, "artist")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
                        walk += strlen("artist");
                }

                if (BEGINS_WITH(walk+1, "album_artist")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_ALBUM_ARTIST, 0));
                        walk += strlen("album_artist");
                } else if (BEGINS_WITH(walk+1, "album")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));
                        walk += strlen("album");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "track")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                        walk += strlen("track");
                }

                if (BEGINS_WITH(walk+1, "title")) {
                        outwalk += sprintf(outwalk, "%s", mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
                        walk += strlen("title");
                }
        }

out:
        mpd_song_free(song);
        mpd_connection_free(conn);
        OUTPUT_FULL_TEXT(buffer);
        return;
}

