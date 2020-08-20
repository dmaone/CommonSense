#include "LayerCondition.h"

LayerCondition::LayerCondition() : fnKeys(0), layer_(0) {}

LayerCondition::LayerCondition(unsigned char bin) {
  fnKeys = (bin >> 4);
  if (fnKeys > 15)
    fnKeys = 0;

  layer_ = bin & 0x0f;
}

LayerCondition::LayerCondition(bool fn1, bool fn2, bool fn3, bool fn4,
                               int layer)
    : fnKeys(0), layer_(layer) {
  if (fn1)
    fnKeys |= fkmFn1;
  if (fn2)
    fnKeys |= fkmFn2;
  if (fn3)
    fnKeys |= fkmFn3;
  if (fn4)
    fnKeys |= fkmFn4;
}

bool LayerCondition::fn1Set() { return (fnKeys & fkmFn1) > 0; }

bool LayerCondition::fn2Set() { return (fnKeys & fkmFn2) > 0; }

bool LayerCondition::fn3Set() { return (fnKeys & fkmFn3) > 0; }

bool LayerCondition::fn4Set() { return (fnKeys & fkmFn4) > 0; }

int LayerCondition::layer() { return layer_; }

unsigned char LayerCondition::toBin() {
  return (fnKeys << 4) | (layer_ & 0xf);
}
