#include "./Default.h"

namespace Components::Buttons {

    Default::Default() : Button() {}

    bool Default::onDown() { return true; }

    std::string Default::onUp() { return command; }

} // namespace Components::Buttons
