#pragma once

class LayerCondition {
public:
  enum FnKeyMask {
    fkmFn1 = (1 << 0),
    fkmFn2 = (1 << 1),
    fkmFn3 = (1 << 2),
    fkmFn4 = (1 << 3)
  };

  LayerCondition();
  LayerCondition(unsigned char bin);
  LayerCondition(bool fn1, bool fn2, bool fn3, bool fn4, int layer);
  bool fn1Set();
  bool fn2Set();
  bool fn3Set();
  bool fn4Set();
  int layer();
  unsigned char toBin();

private:
  unsigned char fnKeys;
  unsigned char layer_;
};
