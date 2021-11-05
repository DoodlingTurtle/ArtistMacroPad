#include "../Button.h"

namespace Components::Buttons {

    class Default : public Components::Button {

      public:
        Default();

        bool onDown() override;
        std::string onUp() override;
    };

} // namespace Components::Buttons