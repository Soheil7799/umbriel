#pragma once

#include <cstdint>
#include <unordered_map>
#include <wayland-server-core.h>
#include <xkbcommon/xkbcommon.h>

struct wlr_surface;

namespace umbriel {

  // Which XKB group each keyboard-focusable surface was last using, for
  // input.keyboard.track_layout = "window".
  //
  // Keyed on the SURFACE rather than on a View. That is the whole reason the
  // feature behaves correctly for layer-shell clients: a launcher or a lock
  // screen takes keyboard focus without ever being a toplevel, and keying on
  // windows would leave those inheriting whatever the last window used. A
  // surface nobody has focused yet has no entry, which is what lets a newly
  // opened surface start from the configured default instead of inheriting.
  //
  // Entries free themselves through the surface's own destroy signal, the same
  // arrangement wine_color_manager uses for its per-surface state.
  class SurfaceLayoutMemory {
  public:
    SurfaceLayoutMemory() = default;
    ~SurfaceLayoutMemory();

    SurfaceLayoutMemory(const SurfaceLayoutMemory&) = delete;
    SurfaceLayoutMemory& operator=(const SurfaceLayoutMemory&) = delete;

    void remember(wlr_surface* surface, xkb_layout_index_t layout);
    // The layout stored for `surface`, or `fallback` when it has never held
    // focus. A null surface also yields `fallback`.
    [[nodiscard]] xkb_layout_index_t recall(wlr_surface* surface, xkb_layout_index_t fallback) const;
    void forget(wlr_surface* surface);
    [[nodiscard]] size_t size() const { return m_entries.size(); }

  private:
    struct Entry {
      SurfaceLayoutMemory* owner = nullptr;
      wlr_surface* surface = nullptr;
      xkb_layout_index_t layout = 0;
      wl_listener destroy{};
    };

    static void onSurfaceDestroy(wl_listener* listener, void* data);
    static void destroyEntry(Entry* entry);

    std::unordered_map<wlr_surface*, Entry*> m_entries;
  };

} // namespace umbriel
