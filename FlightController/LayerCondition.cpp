#include "LayerCondition.h"

LayerCondition::LayerCondition(unsigned char bin) {
  fnKeys_ = (bin >> 4);
  if (fnKeys_ > 15) {
    fnKeys_ = 0; // invalid, clear
  }
  layer_ = bin & 0x0f;
}

LayerCondition::LayerCondition(
    bool fn1, bool fn2, bool fn3, bool fn4, int layer) : layer_(layer) {
  fnKeys_ = (fn1 ? fkmFn1 : 0)
          + (fn2 ? fkmFn2 : 0)
          + (fn3 ? fkmFn3 : 0)
          + (fn4 ? fkmFn4 : 0);
}

bool LayerCondition::fn1Set() { return (fnKeys_ & fkmFn1) > 0; }

bool LayerCondition::fn2Set() { return (fnKeys_ & fkmFn2) > 0; }

bool LayerCondition::fn3Set() { return (fnKeys_ & fkmFn3) > 0; }

bool LayerCondition::fn4Set() { return (fnKeys_ & fkmFn4) > 0; }

int LayerCondition::layer() { return layer_; }

unsigned char LayerCondition::toBin() {
  return (fnKeys_ << 4) | (layer_ & 0xf);
}
