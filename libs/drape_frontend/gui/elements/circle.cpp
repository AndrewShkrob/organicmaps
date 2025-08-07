#include "circle.hpp"

namespace gui::elements
{
CircleVertexData createCircleVertexData(glsl::vec3 color, glsl::vec3 outlineColor, float radius,
                                        float outlineWidthRatio)
{
  CircleVertexData data;
  data.emplace_back(glsl::vec2(-1.0, 1.0), color, outlineColor, radius, outlineWidthRatio);
  data.emplace_back(glsl::vec2(-1.0, -1.0), color, outlineColor, radius, outlineWidthRatio);
  data.emplace_back(glsl::vec2(1.0, 1.0), color, outlineColor, radius, outlineWidthRatio);
  data.emplace_back(glsl::vec2(1.0, -1.0), color, outlineColor, radius, outlineWidthRatio);
  return data;
}

CircleHandle::CircleHandle(uint32_t id, dp::Anchor anchor, m2::PointF const & pivot, m2::PointF const & size,
                           CircleVertexData data)
  : Handle(id, anchor, pivot)
  , m_data(data)
{
  SetIsVisible(true);
}

void CircleHandle::GetAttributeMutation(ref_ptr<dp::AttributeBufferMutator> mutator) const
{
  if (!m_dirty)
    return;

  TOffsetNode const & node = GetOffsetNode(CircleVertex::GetDynamicStreamID());
  ASSERT_EQUAL(node.first.GetElementSize(), sizeof(CircleVertex), ());
  ASSERT_EQUAL(node.second.m_count, m_data.size(), ());

  std::uint32_t const byteCount = static_cast<std::uint32_t>(m_data.size()) * sizeof(CircleVertex);
  void * buffer = mutator->AllocateMutationBuffer(byteCount);
  memcpy(buffer, m_data.data(), byteCount);

  dp::MutateNode mutateNode;
  mutateNode.m_region = node.second;
  mutateNode.m_data = make_ref(buffer);
  mutator->AddMutation(node.first, mutateNode);

  m_dirty = false;
}

void Circle::SetHandleId(uint32_t handleId)
{
  m_handleId = handleId;
}

void Circle::SetPosition(Position const & position)
{
  m_position = position;
}

void Circle::SetRadius(float radius)
{
  m_radius = radius;
}

void Circle::SetOutlineWidthRatio(float widthRatio)
{
  m_outlineWidthRatio = widthRatio;
}

void Circle::SetColor(dp::Color const & color)
{
  m_color = color;
}

void Circle::SetOutlineColor(dp::Color const & color)
{
  m_outlineColor = color;
}

void Circle::SetHandleCreator(HandleCreator handleCreator)
{
  m_handleCreator = std::move(handleCreator);
}

void Circle::Draw(ref_ptr<dp::GraphicsContext> context, ShapeControl & control) const
{
  Validate();

  float const radiusInPixels = m_radius;
  CircleVertexData data =
      createCircleVertexData(glsl::ToVec3(m_color), glsl::ToVec3(m_outlineColor), radiusInPixels, m_outlineWidthRatio);

  auto state = df::CreateRenderState(gpu::Program::GuiCircle, df::DepthLayer::GuiLayer);
  state.SetDepthTestEnabled(false);

  dp::AttributeProvider provider(1, 4);
  provider.InitStream(0, CircleVertex::GetBindingInfo(), make_ref(data.data()));
  drape_ptr<dp::OverlayHandle> handle = m_handleCreator(m_handleId, m_position.m_anchor, m_position.m_pixelPivot,
                                                        m2::PointF{radiusInPixels * 2, radiusInPixels * 2}, data);

  dp::Batcher batcher(dp::Batcher::IndexPerQuad, dp::Batcher::VertexPerQuad);
  batcher.SetBatcherHash(static_cast<uint64_t>(df::BatcherBucket::Default));
  dp::SessionGuard guard(context, batcher, std::bind(&ShapeControl::AddShape, &control, _1, _2));
  batcher.InsertTriangleStrip(context, state, make_ref(&provider), std::move(handle));
}

void Circle::Validate() const
{
  ASSERT_NOT_EQUAL(m_handleId, 0, ("Handle id must be set."));
  ASSERT_EQUAL(m_position.m_anchor, dp::Center, ("Only dp::Center is supported for Circle."));
  ASSERT_NOT_EQUAL(m_radius, 0.0f, ("Radius must be set."));
  ASSERT_GREATER_OR_EQUAL(m_outlineWidthRatio, 0.0f, ("Outline width ratio must be in the range [0.0, 1.0]."));
  ASSERT_LESS_OR_EQUAL(m_outlineWidthRatio, 1.0f, ("Outline width ratio must be in the range [0.0, 1.0]."));
  ASSERT_NOT_EQUAL(m_color, dp::Color::Transparent(), ("Color must be set."));
  ASSERT(m_handleCreator, ("HandleCreator must be set."));
}
}  // namespace gui::elements
