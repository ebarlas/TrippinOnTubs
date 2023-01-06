#ifndef TRIPPIN_COMBOMANAGER_H
#define TRIPPIN_COMBOMANAGER_H

#include "NotificationManager.h"

namespace trippin {
    class ComboManager {
    public:
        ComboManager(NotificationManager &notificationManager, int minComboHits);
        int reset();
        void recordHit();
    private:
        NotificationManager &notificationManager;
        const int minComboHits;

        int hits{};
    };
}

#endif