#pragma once

#include "d2d1_common.h"

#include <memory>
#include <list>

namespace base {
namespace dx {

// D2D 合成树节点
class D2d1LayerNode {
public:
  virtual ~D2d1LayerNode() {};

public:
  virtual void Draw(ID2D1RenderTarget* rt);
  void AddLayer(std::unique_ptr<D2d1LayerNode> node);

public:
  std::list<std::unique_ptr<D2d1LayerNode>>& children() { return _children; };

protected:
  std::list<std::unique_ptr<D2d1LayerNode>> _children;
};

// D2D 合成器
class D2d1Compositor {
public:
  D2d1Compositor();

  void Draw(ID2D1RenderTarget* rt);
  void AddLayer(std::unique_ptr<D2d1LayerNode> node) const;

private:
  std::unique_ptr<D2d1LayerNode> _root;
};

}
}