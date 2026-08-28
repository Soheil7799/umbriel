#pragma once

#include <string>
#include <string_view>

namespace umbriel {

  // How a physical output identifies itself, as wlroots reports it on
  // wlr_output. Every field except the connector may be absent: make, model and
  // serial come from EDID, which not all displays fill in.
  struct OutputIdentity {
    std::string_view connector;
    std::string_view make;
    std::string_view model;
    std::string_view serial;
  };

  // "<make> <model> <serial>", with the literal "Unknown" standing in for any
  // field the display does not report. This is the string a user writes to name
  // an output by the panel rather than by the port it happens to be plugged
  // into, and it is what `umbriel outputs` should show them.
  [[nodiscard]] std::string outputDescriptor(const OutputIdentity& identity);

  // Does a configured output name refer to this output?
  //
  // Two forms are accepted:
  //   * the connector, e.g. "HDMI-A-1";
  //   * the descriptor above, e.g. "Microstep MSI G2712F CD6T084401192".
  //
  // Both are compared case-insensitively over ASCII. A display reporting no
  // make, model or serial can only be named by its connector, so the descriptor
  // form is not attempted for it -- otherwise every such output would answer to
  // "Unknown Unknown Unknown" and they would all collide.
  //
  // The point of the descriptor form is that connectors are a property of the
  // machine, not of the monitor: two different displays used at two different
  // desks are both HDMI-A-1, so a connector-keyed rule written for one silently
  // misapplies to the other.
  [[nodiscard]] bool outputNameMatches(const OutputIdentity& identity, std::string_view configured);

} // namespace umbriel
