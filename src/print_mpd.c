// vim:ts=8:expandtab
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <mpd/client.h>
#include <libnotify/notify.h>

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

static char *prev_song;

void format_string(
        struct mpd_song *song,
        const char *format,
        char *str,
        char **output
) {
        char *outwalk = *output;
        assert(outwalk == str);

        const char *walk;

        for (walk = format; *walk != '\0'; walk++) {
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

}

void send_notification(
        struct mpd_song *song,
        const char *header_format,
        const char *body_format
) {
        char *outwalk;

        char header[4096];
        outwalk = header;
        format_string(song, header_format, header, &outwalk);

        char body[4096];
        outwalk = body;
        format_string(song, body_format, body, &outwalk);

        NotifyNotification *song_notif = notify_notification_new(header, body, "dialog-information");
        notify_notification_show(song_notif, NULL);
        g_object_unref(G_OBJECT(song_notif));
}

void print_mpd(
        yajl_gen json_gen,
        char *buffer,
        const char *format,
        const char *format_stopped,
        const char *notif_header_format,
        const char *notif_body_format
) {
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

        const char *walk;

        for (walk = format; *walk != '\0'; walk++) {
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


        const char *uri = mpd_song_get_uri(song);

        // Determine if this is a different song than the last time we checked
        if (prev_song != NULL && strcmp(prev_song, uri) == 0)
                goto out;

        // Otherwise, update the text and send a notification
        send_notification(song, notif_header_format, notif_body_format);

        // Copy the new URI into the static prev_song string
        free(prev_song);
        if ((prev_song = (char *)malloc(sizeof(char) * strlen(uri))) == NULL)
                goto out;
        strcpy(prev_song, uri);

out:
        mpd_song_free(song);
        mpd_connection_free(conn);
        OUTPUT_FULL_TEXT(buffer);
        return;
}

