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
#include "v8_value_hasher.h"

typedef unordered_set<v8::Handle<v8::Value>, v8_value_hash, v8_value_equal_to> SetType;

class NodeSet : public node::ObjectWrap {
 public:
  static void init(v8::Handle<v8::Object> exports);

 private:
  NodeSet();
  NodeSet(size_t buckets);
  ~NodeSet();

  SetType set;

  // new NodeSet() or new NodeSet(buckets)
  static v8::Handle<v8::Value> Constructor(const v8::Arguments &args);

  // set.has(value) : boolean
  static v8::Handle<v8::Value> Has(const v8::Arguments &args);

  // set.add(key, value) : this
  static v8::Handle<v8::Value> Add(const v8::Arguments &args);

  // set.entries() : iterator
  static v8::Handle<v8::Value> Entries(const v8::Arguments &args);

  // set.keys() : iterator
  static v8::Handle<v8::Value> Keys(const v8::Arguments &args);

  // set.values() : iterator
  static v8::Handle<v8::Value> Values(const v8::Arguments &args);

  // set.delete(value) : boolean
  static v8::Handle<v8::Value> Delete(const v8::Arguments &args);

  // set.clear() : undefined
  static v8::Handle<v8::Value> Clear(const v8::Arguments &args);

  // set.size() : number of elements
  static v8::Handle<v8::Value> Size(v8::Local<v8::String> property, const v8::AccessorInfo &info);

  // set.rehash(buckets) : undefined
  static v8::Handle<v8::Value> Rehash(const v8::Arguments &args);

  //set.reserve(size) : undefined
  static v8::Handle<v8::Value> Reserve(const v8::Arguments &args);

  //set.forEach(function (key, value) {...}) : undefined
  static v8::Handle<v8::Value> ForEach(const v8::Arguments &args);
};

#endif
