#include "settings.h"

namespace leavesfx {
void ResetSettings() {
    bool enabled=Cfg().enabled; int lang=Cfg().lang;
    Cfg()=Settings(); Cfg().enabled=enabled; Cfg().lang=lang;
}
} // namespace leavesfx
