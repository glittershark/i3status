// vim:ts=8:expandtab
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <mpd/client.h>
#include <libnotify/notify.h>

#include "i3status.h"

#define MPD_OUTPUT_OPTION(option_name, tag_name) \
if (BEGINS_WITH(walk + 1, option_name)) { \
        if ((value = mpd_song_get_tag(song, tag_name, 0)) == NULL) { \
                *(outwalk++) = '?'; \
        } else { \
                outwalk += sprintf(outwalk, "%s", value); \
        } \
        walk += strlen(option_name); \
}

static char *prev_song;
static struct mpd_connection *conn = NULL;

void mpd_format_string(
        struct mpd_song *song,
        const char *format,
        char *str,
        char **output
) {
        char *outwalk = *output;
        assert(outwalk == str);

        const char *walk;
        const char *value;

        for (walk = format; *walk != '\0'; walk++) {
                if (*walk != '%') {
                        *(outwalk++) = *walk;
                        continue;
                }

                MPD_OUTPUT_OPTION("artist", MPD_TAG_ARTIST)
                MPD_OUTPUT_OPTION("album_artist", MPD_TAG_ALBUM_ARTIST)
                else MPD_OUTPUT_OPTION("album", MPD_TAG_ALBUM)
                MPD_OUTPUT_OPTION("title", MPD_TAG_TITLE)
                MPD_OUTPUT_OPTION("track", MPD_TAG_TRACK)
                MPD_OUTPUT_OPTION("name", MPD_TAG_NAME)
                MPD_OUTPUT_OPTION("genre", MPD_TAG_GENRE)
                MPD_OUTPUT_OPTION("date", MPD_TAG_DATE)
                MPD_OUTPUT_OPTION("composer", MPD_TAG_COMPOSER)
                MPD_OUTPUT_OPTION("performer", MPD_TAG_PERFORMER)
                MPD_OUTPUT_OPTION("comment", MPD_TAG_COMMENT)
                MPD_OUTPUT_OPTION("disc", MPD_TAG_DISC)
        }

}

void mpd_send_notification(
        struct mpd_song *song,
        const char *header_format,
        const char *body_format
) {
        char *outwalk;

        char header[4096];
        outwalk = header;
        mpd_format_string(song, header_format, header, &outwalk);

        char body[4096];
        outwalk = body;
        mpd_format_string(song, body_format, body, &outwalk);

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

        struct mpd_song *song;

        /* Use defaults */
        if (conn == NULL) {
                if ((conn = mpd_connection_new(NULL, 0, 0)) == NULL) {
                        outwalk += sprintf(outwalk, "%s", format_stopped);
                        OUTPUT_FULL_TEXT(buffer);
                        return;
                }
        }

        if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
                mpd_connection_free(conn);
                conn = NULL;
                outwalk += sprintf(outwalk, "%s", format_stopped);
                OUTPUT_FULL_TEXT(buffer);
                return;
        }

        /* Get current song */
        song = mpd_run_current_song(conn);
        if (song == NULL) {
                outwalk += sprintf(outwalk, "%s", format_stopped);
                OUTPUT_FULL_TEXT(buffer);
                return;
        }

        const char *walk;
        const char *value;

        for (walk = format; *walk != '\0'; walk++) {
                if (*walk != '%') {
                        *(outwalk++) = *walk;
                        continue;
                }

                MPD_OUTPUT_OPTION("artist", MPD_TAG_ARTIST)
                MPD_OUTPUT_OPTION("album_artist", MPD_TAG_ALBUM_ARTIST)
                else MPD_OUTPUT_OPTION("album", MPD_TAG_ALBUM)
                MPD_OUTPUT_OPTION("title", MPD_TAG_TITLE)
                MPD_OUTPUT_OPTION("track", MPD_TAG_TRACK)
                MPD_OUTPUT_OPTION("name", MPD_TAG_NAME)
                MPD_OUTPUT_OPTION("genre", MPD_TAG_GENRE)
                MPD_OUTPUT_OPTION("date", MPD_TAG_DATE)
                MPD_OUTPUT_OPTION("composer", MPD_TAG_COMPOSER)
                MPD_OUTPUT_OPTION("performer", MPD_TAG_PERFORMER)
                MPD_OUTPUT_OPTION("comment", MPD_TAG_COMMENT)
                MPD_OUTPUT_OPTION("disc", MPD_TAG_DISC)
        }


        const char *uri = mpd_song_get_uri(song);

        // Determine if this is a different song than the last time we checked
        if (prev_song != NULL && strcmp(prev_song, uri) == 0)
                goto out;

        // Otherwise, update the text and send a notification
        mpd_send_notification(song, notif_header_format, notif_body_format);

        // Copy the new URI into the static prev_song string
        free(prev_song);
        if ((prev_song = (char *)malloc(sizeof(char) * (strlen(uri) + 1))) == NULL)
                goto out;
        strcpy(prev_song, uri);

out:
        mpd_song_free(song);
        OUTPUT_FULL_TEXT(buffer);
        return;
}


void cleanup_mpd() {
        if (conn != NULL) {
                mpd_connection_free(conn);
        }
}

