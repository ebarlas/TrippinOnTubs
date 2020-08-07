#ifndef TRIPPINONTUBS_SCALE_H
#define TRIPPINONTUBS_SCALE_H

namespace trippin {
    enum class Scale {
        xxxsmall,
        xxsmall,
        xsmall,
        small,
        medium,
        large
    };

    constexpr const char *scaleName(trippin::Scale scale) {
        switch (scale) {
            case Scale::xxxsmall:
                return "xxxsmall";
            case Scale::xxsmall:
                return "xxsmall";
            case Scale::xsmall:
                return "xsmall";
            case Scale::small:
                return "small";
            case Scale::medium:
                return "medium";
            case Scale::large:
                return "large";
        }
    }

    constexpr double scaleMultiplier(trippin::Scale scale) {
        switch (scale) {
            case Scale::xxxsmall:
                return 0.25;
            case Scale::xxsmall:
                return 0.375;
            case Scale::xsmall:
                return 0.5;
            case Scale::small:
                return 0.75;
            case Scale::medium:
                return 1.0;
            case Scale::large:
                return 1.25;
        }
    }
}

#endif
