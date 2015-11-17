#ifndef NODE_SET_H
#define NODE_SET_H

#include <string>
#include <iostream>
#ifdef __APPLE__
#include <tr1/unordered_set>
#define unordered_set std::tr1::unordered_set
#else
#include <unordered_set>
#define unordered_set std::unordered_set
#endif
#include <node.h>
#include <nan.h>
#include "v8_value_hasher.h"

typedef unordered_set<CopyablePersistent *, v8_value_hash, v8_value_equal_to> SetType;

class NodeSet : public Nan::ObjectWrap {
public:
    static void init(v8::Local<v8::Object> target);

 private:
  NodeSet();
  ~NodeSet();

  SetType set;

  // new NodeSet() or new NodeSet(buckets)
  static NAN_METHOD(Constructor);

  // set.has(value) : boolean
  static NAN_METHOD(Has);

  // set.add(key, value) : this
  static NAN_METHOD(Add);

  // set.entries() : iterator
  static NAN_METHOD(Entries);

  // set.keys() : iterator
  static NAN_METHOD(Keys);

  // set.values() : iterator
  static NAN_METHOD(Values);

  // set.delete(value) : boolean
  static NAN_METHOD(Delete);

  // set.clear() : undefined
  static NAN_METHOD(Clear);

  // set.size() : number of elements
  static NAN_GETTER(Size);

  // set.rehash(buckets) : undefined
  static NAN_METHOD(Rehash);

  //set.reserve(size) : undefined
  static NAN_METHOD(Reserve);

  //set.forEach(function (key, value) {...}) : undefined
  static NAN_METHOD(ForEach);
};

#endif
