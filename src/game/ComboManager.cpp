#include "ComboManager.h"

trippin::ComboManager::ComboManager(NotificationManager &notificationManager, int minComboHits) :
        notificationManager(notificationManager), minComboHits(minComboHits) {

}

int trippin::ComboManager::reset() {
    int result = 0;
    if (hits >= minComboHits) {
        result = hits;
        notificationManager.add(hits);
    }
    hits = 0;
    return result;
}

void trippin::ComboManager::recordHit() {
    hits++;
}