#include "check.h"
#include "output/identity.h"

#include <string>

using umbriel::outputDescriptor;
using umbriel::OutputIdentity;
using umbriel::outputNameMatches;

namespace {
  // A display that fills in every EDID string.
  constexpr OutputIdentity kMonitor{
      .connector = "HDMI-A-1", .make = "Microstep", .model = "MSI G2712F", .serial = "CD6T084401192"
  };
  // A laptop panel: make and model, no serial. Common.
  constexpr OutputIdentity kPanel{
      .connector = "eDP-1", .make = "Najing CEC Panda FPD Technology CO. ltd", .model = "0x0036", .serial = ""
  };
  // Nothing but a connector: headless and nested outputs look like this.
  constexpr OutputIdentity kBare{.connector = "HEADLESS-1", .make = "", .model = "", .serial = ""};
} // namespace

UMBRIEL_TEST(connectorStillMatches) {
  CHECK(outputNameMatches(kMonitor, "HDMI-A-1"));
  CHECK(outputNameMatches(kPanel, "eDP-1"));
  CHECK(outputNameMatches(kBare, "HEADLESS-1"));
  CHECK(!outputNameMatches(kMonitor, "HDMI-A-2"));
}

UMBRIEL_TEST(descriptorMatches) {
  CHECK_EQ(outputDescriptor(kMonitor), std::string("Microstep MSI G2712F CD6T084401192"));
  CHECK(outputNameMatches(kMonitor, "Microstep MSI G2712F CD6T084401192"));
}

// A missing field becomes "Unknown" rather than collapsing the separators, so
// the descriptor stays a stable three-part string.
UMBRIEL_TEST(missingFieldsBecomeUnknown) {
  CHECK_EQ(outputDescriptor(kPanel), std::string("Najing CEC Panda FPD Technology CO. ltd 0x0036 Unknown"));
  CHECK(outputNameMatches(kPanel, "Najing CEC Panda FPD Technology CO. ltd 0x0036 Unknown"));
}

UMBRIEL_TEST(matchingIsCaseInsensitive) {
  CHECK(outputNameMatches(kMonitor, "hdmi-a-1"));
  CHECK(outputNameMatches(kMonitor, "microstep msi g2712f cd6t084401192"));
  CHECK(outputNameMatches(kMonitor, "MICROSTEP MSI G2712F CD6T084401192"));
}

// The guard that stops every EDID-less output answering to the same string.
UMBRIEL_TEST(outputWithoutEdidMatchesOnlyItsConnector) {
  CHECK(!outputNameMatches(kBare, "Unknown Unknown Unknown"));
  CHECK(outputNameMatches(kBare, "HEADLESS-1"));
}

// Two panels on the same connector are the whole point: a rule written for one
// desk must not apply at the other.
UMBRIEL_TEST(sameConnectorDifferentPanelsAreDistinguishable) {
  constexpr OutputIdentity home{
      .connector = "HDMI-A-1", .make = "Microstep", .model = "MSI G2712F", .serial = "CD6T084401192"
  };
  constexpr OutputIdentity office{
      .connector = "HDMI-A-1", .make = "Microstep", .model = "MSI MP275Q", .serial = "PC3M805701148"
  };
  CHECK(outputNameMatches(home, "Microstep MSI G2712F CD6T084401192"));
  CHECK(!outputNameMatches(office, "Microstep MSI G2712F CD6T084401192"));
  CHECK(outputNameMatches(office, "Microstep MSI MP275Q PC3M805701148"));
  CHECK(!outputNameMatches(home, "Microstep MSI MP275Q PC3M805701148"));
  // Both still answer to the connector, which is why connector-keyed rules
  // cannot tell them apart.
  CHECK(outputNameMatches(home, "HDMI-A-1"));
  CHECK(outputNameMatches(office, "HDMI-A-1"));
}

UMBRIEL_TEST(partialDescriptorsDoNotMatch) {
  CHECK(!outputNameMatches(kMonitor, "Microstep"));
  CHECK(!outputNameMatches(kMonitor, "MSI G2712F"));
  CHECK(!outputNameMatches(kMonitor, "Microstep MSI G2712F"));
  CHECK(!outputNameMatches(kMonitor, ""));
}

int main() { return RUN_TESTS(); }
