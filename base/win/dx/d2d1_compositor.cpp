#include "d2d1_compositor.h"

namespace base {
namespace win {
namespace dx {

void D2d1LayerNode::Draw(ID2D1RenderTarget* rt) {
  for (auto& node : _children) {
    node->Draw(rt);
  }
}

void D2d1LayerNode::AddLayer(std::unique_ptr<D2d1LayerNode> node) {
  _children.emplace_back(std::move(node));
}

D2d1Compositor::D2d1Compositor() {
  _root = std::make_unique<D2d1LayerNode>();
}

void D2d1Compositor::Draw(ID2D1RenderTarget* rt) {
  _root->Draw(rt);
}

void D2d1Compositor::AddLayer(std::unique_ptr<D2d1LayerNode> node) const {
  _root->children().emplace_back(std::move(node));
}

}
}
}