#pragma once

#include "drape_frontend/gui/shape.hpp"

namespace gui::elements
{
struct CircleVertex
{
  CircleVertex() = default;

  CircleVertex(glsl::vec2 position, glsl::vec3 color, glsl::vec3 outlineColor, float radius, float outlineWidthRatio)
    : m_position(position)
    , m_color(color)
    , m_outlineColor(outlineColor)
    , m_radius(radius)
    , m_outlineWidthRatio(outlineWidthRatio)
  {}

  static uint32_t GetDynamicStreamID() { return 0x228; }

  static dp::BindingInfo GetBindingInfo()
  {
    dp::BindingFiller<CircleVertex> filler(5, GetDynamicStreamID());
    filler.FillDecl<glsl::vec2>("a_position");
    filler.FillDecl<glsl::vec3>("a_color");
    filler.FillDecl<glsl::vec3>("a_outlineColor");
    filler.FillDecl<float>("a_radius");
    filler.FillDecl<float>("a_outlineWidthRatio");
    return filler.m_info;
  }

  glsl::vec2 m_position{};
  glsl::vec3 m_color{};
  glsl::vec3 m_outlineColor{};
  float m_radius{};
  float m_outlineWidthRatio{};
};

using CircleVertexData = buffer_vector<CircleVertex, dp::Batcher::VertexPerQuad>;

class CircleHandle : public Handle
{
public:
  CircleHandle(uint32_t id, dp::Anchor anchor, m2::PointF const & pivot, m2::PointF const & size,
               CircleVertexData data);

  void GetAttributeMutation(ref_ptr<dp::AttributeBufferMutator> mutator) const override;

  void SetRadius(float radius)
  {
    // todo
    for (auto & vertex : m_data)
      vertex.m_radius = radius;
    m_dirty = true;
  }

private:
  CircleVertexData m_data;
  mutable bool m_dirty = false;
};

class Circle
{
public:
  using HandleCreator = std::function<drape_ptr<CircleHandle>(uint32_t, dp::Anchor, m2::PointF const &,
                                                              m2::PointF const &, CircleVertexData)>;

  Circle() = default;

  void SetHandleId(uint32_t handleId);
  void SetPosition(Position const & position);
  void SetRadius(float radius);
  void SetOutlineWidthRatio(float widthRatio);
  void SetColor(dp::Color const & color);
  void SetOutlineColor(dp::Color const & color);
  void SetHandleCreator(HandleCreator handleCreator);

  void Draw(ref_ptr<dp::GraphicsContext> context, ShapeControl & control) const;

private:
  void Validate() const;

  uint32_t m_handleId = 0;
  Position m_position{};
  float m_radius = 0.0f;
  float m_outlineWidthRatio = 0.0f;
  dp::Color m_color = dp::Color::Transparent();
  dp::Color m_outlineColor = dp::Color::Transparent();
  HandleCreator m_handleCreator;
};
}  // namespace gui::elements
