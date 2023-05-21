#ifndef TRIPPIN_LEVELENDLISTENER_H
#define TRIPPIN_LEVELENDLISTENER_H

#include "engine/Listener.h"
#include "game/Goggin.h"
#include "game/ScoreTicker.h"

namespace trippin {
    // This listener runs last. It samples off-map level-end conditions after other engine listeners
    // have had an opportunity to respond. The main game loop uses this component.
    class LevelEndListener : public Listener {
    private:
        const Goggin &goggin;
        ScoreTicker &scoreTicker;
        bool ended{};
    public:
        LevelEndListener(const Goggin &goggin, ScoreTicker &scoreTicker);
        void afterTick(int engineTicks) override;
        bool hasEnded() const;
    };
}

#endif
