#include "output/identity.h"

namespace umbriel {

  namespace {
    constexpr std::string_view kUnknown = "Unknown";

    char lowerAscii(char value) { return value >= 'A' && value <= 'Z' ? static_cast<char>(value - 'A' + 'a') : value; }

    bool equalsIgnoreCase(std::string_view left, std::string_view right) {
      if (left.size() != right.size()) {
        return false;
      }
      for (size_t index = 0; index < left.size(); ++index) {
        if (lowerAscii(left[index]) != lowerAscii(right[index])) {
          return false;
        }
      }
      return true;
    }

    std::string_view orUnknown(std::string_view value) { return value.empty() ? kUnknown : value; }
  } // namespace

  std::string outputDescriptor(const OutputIdentity& identity) {
    std::string descriptor;
    descriptor.append(orUnknown(identity.make));
    descriptor.push_back(' ');
    descriptor.append(orUnknown(identity.model));
    descriptor.push_back(' ');
    descriptor.append(orUnknown(identity.serial));
    return descriptor;
  }

  bool outputNameMatches(const OutputIdentity& identity, std::string_view configured) {
    if (!identity.connector.empty() && equalsIgnoreCase(configured, identity.connector)) {
      return true;
    }
    // Nothing beyond the connector to match on. Without this guard every display
    // with no EDID strings would answer to "Unknown Unknown Unknown".
    if (identity.make.empty() && identity.model.empty() && identity.serial.empty()) {
      return false;
    }
    return equalsIgnoreCase(configured, outputDescriptor(identity));
  }

} // namespace umbriel
