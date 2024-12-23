#include "drape_frontend/tile_renderer.hpp"

#include "drape_frontend/color_constants.hpp"
#include "drape_frontend/tile_utils.hpp"
#include "drape_frontend/visual_params.hpp"

#include "shaders/programs.hpp"

#include "drape/glsl_func.hpp"
#include "drape/support_manager.hpp"
#include "drape/vertex_array_buffer.hpp"

#include "indexer/map_style_reader.hpp"
#include "indexer/scales.hpp"

#include "base/logging.hpp"

#include <algorithm>
#include <array>
#include <utility>

namespace df
{
struct TileVertex
{
  using TPosition = glsl::vec2;
  using TNormal = glsl::vec4;
  using TTexCoord = glsl::vec4;

  TileVertex() = default;
  TileVertex(TPosition const & position
             //             , TNormal const & normal, TTexCoord const & colorTexCoord
             )
    : m_position(position)
  //    , m_normal(normal), m_colorTexCoord(colorTexCoord)
  {
  }

  TPosition m_position;
  //  TNormal m_normal;
  //  TTexCoord m_colorTexCoord;
};

using TileVertexData = buffer_vector<TileVertex, dp::Batcher::VertexPerQuad>;

dp::BindingInfo const & GetTileStaticBindingInfo()
{
  static std::unique_ptr<dp::BindingInfo> s_info;
  if (s_info == nullptr)
  {
    dp::BindingFiller<TileVertex> filler(1);
    filler.FillDecl<TileVertex::TPosition>("a_position");
    //    filler.FillDecl<TileStaticVertex::TNormal>("a_normal");
    //    filler.FillDecl<TileStaticVertex::TTexCoord>("a_colorTexCoord");
    s_info = std::make_unique<dp::BindingInfo>(filler.m_info);
  }
  return *s_info;
}

TileVertexData createTileVertexData()
{
  TileVertexData data;
  data.emplace_back(glsl::vec2(-1.0, 1.0));
  data.emplace_back(glsl::vec2(-1.0, -1.0));
  data.emplace_back(glsl::vec2(1.0, 1.0));
  data.emplace_back(glsl::vec2(1.0, -1.0));
  return data;
}

void TileRenderer::Render(ref_ptr<dp::GraphicsContext> context, ref_ptr<gpu::ProgramManager> mng,
                          ScreenBase const & screen, int zoomLevel, float opacity, FrameValues const & frameValues)
{
  auto tileVertexData = createTileVertexData();

  auto state = CreateRenderState(gpu::Program::Tile, DepthLayer::GeometryLayer);

  auto program = mng->GetProgram(state.GetProgram<gpu::Program>());
  program->Bind();
  dp::ApplyState(context, program, state);
  gpu::TileProgramParams params;
  frameValues.SetTo(params);
  dp::AttributeProvider provider(1 /* stream count */, 0 /* vertices count*/);
  provider.InitStream(0 /* stream index */, GetTileStaticBindingInfo(), tileVertexData.data());

  dp::Batcher tileBatcher(dp::Batcher::IndexPerQuad, dp::Batcher::VertexPerQuad);
  tileBatcher.InsertTriangleStrip(context, state, make_ref(&provider));
  tileBatcher.StartSession([&](auto && a, auto && b) {
    mng->GetParamsSetter()->Apply(context, program, params);
    b->Render(context, false /* draw as line */);
  });
  tileBatcher.EndSession(context);

  //  CalcTilesCoverage(screen.ClipRect(), zoomLevel,
  //                    [&](int tileX, int tileY)
  //                    {
  //                      TileKey const tileKey{tileX, tileY, (uint8_t)zoomLevel};
  //                      math::Matrix<float, 4, 4> const mv = tileKey.GetTileBasedModelView(screen);
  //                      params.m_modelView = glsl::make_mat4(mv.m_data);
  //                      mng->GetParamsSetter()->Apply(context, program, params);
  //                      LOG(LDEBUG, ("TileRenderer::Render: Rendering tile ", tileKey));
  //                    });
}

bool TileRenderer::HasRenderData() const { return true; }
void TileRenderer::OnUpdateViewport(CoverageResult const & coverage, int currentZoomLevel,
                                    buffer_vector<TileKey, 8> const & tilesToDelete)
{
}
}  // namespace df
