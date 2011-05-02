#ifndef THECHESS_MODEL_USER_CLASSIFICATION_HPP_
#define THECHESS_MODEL_USER_CLASSIFICATION_HPP_

#include <Wt/WString>

namespace thechess {
namespace model {

enum Classification {
    SUPER_GRANDMASTER = 110,
    GRANDMASTER = 100,
    INTERNATIONAL_MASTER = 90,
    NATIONAL_MASTER = 80,
    EXPERT = 70,
    CLASSIFICATION_A = 50,
    CLASSIFICATION_B = 45,
    CLASSIFICATION_C = 40,
    CLASSIFICATION_D = 35,
    CLASSIFICATION_E = 30,
    CLASSIFICATION_F = 25,
    CLASSIFICATION_G = 20,
    CLASSIFICATION_H = 15,
    CLASSIFICATION_I = 10,
    CLASSIFICATION_J = 5,
    NO_CLASSIFICATION = 0
};

Wt::WString classification2str(Classification c);

const Classification classifications[] = {
    SUPER_GRANDMASTER, GRANDMASTER, INTERNATIONAL_MASTER,
    NATIONAL_MASTER, EXPERT, CLASSIFICATION_A, CLASSIFICATION_B,
    CLASSIFICATION_C, CLASSIFICATION_D, CLASSIFICATION_E, CLASSIFICATION_F,
    CLASSIFICATION_G, CLASSIFICATION_H, CLASSIFICATION_I, CLASSIFICATION_J,
    NO_CLASSIFICATION
};
const int classifications_size = 16;

}
}

#endif
