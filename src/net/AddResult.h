#ifndef TRIPPIN_ADDRESULT_H
#define TRIPPIN_ADDRESULT_H

namespace trippin {
    enum class AddResult {
        success,
        clientError,
        other
    };

    inline const char *toString(AddResult r) {
        switch (r) {
            case AddResult::success:
                return "success";
            case AddResult::clientError:
                return "clientError";
            default:
                return "other";
        }
    }
}

#endif
