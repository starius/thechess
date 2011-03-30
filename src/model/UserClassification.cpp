
#include "model/UserClassification.hpp"

namespace thechess {
namespace model {

Wt::WString classification2str(Classification c)
{
    if (c == SUPER_GRANDMASTER) return Wt::WString::tr("SUPER_GRANDMASTER");
    if (c == GRANDMASTER) return Wt::WString::tr("GRANDMASTER");
    if (c == INTERNATIONAL_MASTER) return Wt::WString::tr("INTERNATIONAL_MASTER");
    if (c == NATIONAL_MASTER) return Wt::WString::tr("NATIONAL_MASTER");
    if (c == EXPERT) return Wt::WString::tr("EXPERT");
    if (c == CLASSIFICATION_A) return Wt::WString::tr("CLASSIFICATION_A");
    if (c == CLASSIFICATION_B) return Wt::WString::tr("CLASSIFICATION_B");
    if (c == CLASSIFICATION_C) return Wt::WString::tr("CLASSIFICATION_C");
    if (c == CLASSIFICATION_D) return Wt::WString::tr("CLASSIFICATION_D");
    if (c == CLASSIFICATION_E) return Wt::WString::tr("CLASSIFICATION_E");
    if (c == CLASSIFICATION_F) return Wt::WString::tr("CLASSIFICATION_F");
    if (c == CLASSIFICATION_G) return Wt::WString::tr("CLASSIFICATION_G");
    if (c == CLASSIFICATION_H) return Wt::WString::tr("CLASSIFICATION_H");
    if (c == CLASSIFICATION_I) return Wt::WString::tr("CLASSIFICATION_I");
    if (c == CLASSIFICATION_J) return Wt::WString::tr("CLASSIFICATION_J");
    return Wt::WString::tr("NO_CLASSIFICATION");
}

}
}


