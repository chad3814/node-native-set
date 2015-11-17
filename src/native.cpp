#include <node.h>
#include <v8.h>
#include "set.h"

using namespace v8;

void init(Handle<Object> exports) {
  NodeSet::init(exports);
}

NODE_MODULE(native, init);
