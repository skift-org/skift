export module Karm.App:form_factor;

namespace Karm::App {

export enum struct FormFactor {
    DESKTOP,
    MOBILE,
};

export FormFactor formFactor = FormFactor::DESKTOP;

} // namespace Karm::App
