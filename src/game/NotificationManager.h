#ifndef TRIPPIN_NOTIFICATIONMANAGER_H
#define TRIPPIN_NOTIFICATIONMANAGER_H

#include "sprite/Sprite.h"
#include "NotificationDrawer.h"

namespace trippin {
    class NotificationManager {
    public:
        NotificationManager(
                const Sprite &text,
                const Sprite &digits,
                NotificationDrawer &notificationDrawer);
        void add(int value);

    private:
        const Sprite &text;
        const Sprite &digits;
        NotificationDrawer &notificationDrawer;
    };
}

#endif
