#include "wire.h"

Wire Wires::yellow = { 'Y', 760,  780 };
Wire Wires::black  = { 'K', 840,  860};
Wire Wires::white  = { 'W', 960,  985};
Wire Wires::blue   = { 'B', 690, 710 };
Wire Wires::red    = { 'R', 870,  890};
Wire Wires::open   = { 'O', 999, 1024 };

Wire Wires::list[6] = {
  Wires::yellow,
  Wires::black,
  Wires::white,
  Wires::blue,
  Wires::red,
  Wires::open
};

