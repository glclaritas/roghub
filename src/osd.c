#include <fcntl.h>
#include <glib-object.h>
#include <libnotify/notification.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include <libnotify/notify.h>

void osd_show(char *msg) {
    if(!notify_init("rogctl")) {
        return;
    }

    NotifyNotification *noti = notify_notification_new(msg, NULL, NULL);

    notify_notification_set_urgency(noti, NOTIFY_URGENCY_NORMAL);
    notify_notification_set_timeout(noti, 1000);
    notify_notification_set_hint_string(noti, "x-canonical-private-synchronous", "rogctl");

    notify_notification_show(noti, NULL);
    g_object_unref(G_OBJECT(noti));
    notify_uninit();
}
