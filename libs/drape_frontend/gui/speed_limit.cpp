#include "drape_frontend/gui/speed_limit.hpp"

#include "drape_frontend/gui/drape_gui.hpp"

#include "drape/glsl_types.hpp"

#include <functional>
#include <utility>

using namespace std::placeholders;

namespace gui
{
namespace
{
class TextHandle : public MutableLabelHandle
{
  using TBase = MutableLabelHandle;

public:
  TextHandle(uint32_t id, m2::PointF const & pivot, ref_ptr<dp::TextureManager> textures) : TBase(id, dp::Center, pivot)
  {
    SetTextureManager(textures);
  }

private:
  bool Update(ScreenBase const & screen) override
  {
    SpeedLimitHelper const & helper = DrapeGui::GetSpeedLimitHelper();
    SetIsVisible(helper.IsSpeedLimitAvailable());
    if (IsVisible())
    {
      m_pivot = glsl::ToVec2(helper.GetPosition());
      SetContent(helper.GetSpeedLimit());
    }

    return TBase::Update(screen);
  }
};

class BackgroundHandle : public elements::CircleHandle
{
public:
  using CircleHandle::CircleHandle;

  bool Update(ScreenBase const & screen) override
  {
    SpeedLimitHelper const & helper = DrapeGui::GetSpeedLimitHelper();
    SetIsVisible(helper.IsSpeedLimitAvailable());

    if (IsVisible())
    {
      m_pivot = glsl::ToVec2(helper.GetPosition());
      SetRadius(helper.GetRadius());
    }

    return CircleHandle::Update(screen);
  }
};
}  // namespace

SpeedLimit::SpeedLimit(Position const & position) : Shape(position)
{
  m_background.SetHandleId(EGuiHandle::GuiHandleSpeedSign);
  m_background.SetPosition(position);
  m_background.SetRadius(Config::kBackgroundRadius);
  m_background.SetOutlineWidthRatio(Config::kBackgroundOutlineWidthRatio);
  m_background.SetColor(Config::kBackgroundColor);
  m_background.SetOutlineColor(Config::kBackgroundOutlineColor);
  m_background.SetHandleCreator([](uint32_t id, dp::Anchor anchor, m2::PointF const & pivot, m2::PointF const & size,
                                   elements::CircleVertexData data)
  { return make_unique_dp<BackgroundHandle>(id, anchor, pivot, size, data); });
}

drape_ptr<ShapeRenderer> SpeedLimit::Draw(ref_ptr<dp::GraphicsContext> context, ref_ptr<dp::TextureManager> tex) const
{
  LOG(LWARNING, ("Draw SpeedLimit"));
  ShapeControl control;
  m_background.Draw(context, control);
  DrawText(context, control, tex);

  drape_ptr<ShapeRenderer> renderer = make_unique_dp<ShapeRenderer>();
  renderer->AddShapeControl(std::move(control));
  return renderer;
}

void SpeedLimit::DrawText(ref_ptr<dp::GraphicsContext> context, ShapeControl & control,
                          ref_ptr<dp::TextureManager> tex) const
{
  ASSERT_EQUAL(m_position.m_anchor, dp::Center, ());

  MutableLabelDrawer::Params params;
  params.m_anchor = m_position.m_anchor;
  params.m_alphabet = "0123456789";
  params.m_maxLength = 3;
  params.m_font = DrapeGui::GetGuiTextFont();
  params.m_font.m_color = Config::kTextColor;
  params.m_font.m_outlineColor = Config::kTextColor;
  params.m_font.m_size = 24;
  params.m_pivot = m_position.m_pixelPivot;
  params.m_handleCreator = [tex](dp::Anchor anchor, m2::PointF const & pivot)
  { return make_unique_dp<TextHandle>(EGuiHandle::GuiHandleSpeedSignLabel, pivot, tex); };

  MutableLabelDrawer::Draw(context, params, tex, std::bind(&ShapeControl::AddShape, &control, _1, _2));
}
}  // namespace gui
