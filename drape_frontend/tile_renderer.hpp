#pragma once

#include "drape_frontend/frame_values.hpp"
#include "drape_frontend/traffic_generator.hpp"
#include "drape_frontend/tile_utils.hpp"

#include "shaders/program_manager.hpp"

#include "drape/pointers.hpp"

#include "geometry/screenbase.hpp"
#include "geometry/spline.hpp"

#include <vector>

namespace df
{
class TileRenderer final
{
public:
  void Render(ref_ptr<dp::GraphicsContext> context, ref_ptr<gpu::ProgramManager> mng, ScreenBase const & screen,
              int zoomLevel, float opacity, FrameValues const & frameValues);

  bool HasRenderData() const;
  void OnUpdateViewport(CoverageResult const & coverage, int currentZoomLevel,
                        buffer_vector<TileKey, 8> const & tilesToDelete);
};
}  // namespace df
