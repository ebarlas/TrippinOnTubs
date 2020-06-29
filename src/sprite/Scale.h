#ifndef TRIPPINONTUBS_SCALE_H
#define TRIPPINONTUBS_SCALE_H

namespace trippin {
    enum class Scale {
        xsmall,
        small,
        medium,
        large,
        xlarge,
        xxlarge
    };

    constexpr const char *scaleName(trippin::Scale scale) {
        switch (scale) {
            case Scale::xsmall:
                return "xsmall";
            case Scale::small:
                return "small";
            case Scale::medium:
                return "medium";
            case Scale::large:
                return "large";
            case Scale::xlarge:
                return "xlarge";
            case Scale::xxlarge:
                return "xxlarge";
        }
    }

    constexpr double scaleMultiplier(trippin::Scale scale) {
        switch (scale) {
            case Scale::xsmall:
                return 0.25;
            case Scale::small:
                return 0.5;
            case Scale::medium:
                return 1.0;
            case Scale::large:
                return 1.5;
            case Scale::xlarge:
                return 2.0;
            case Scale::xxlarge:
                return 2.5;
        }
    }
}

#endif
