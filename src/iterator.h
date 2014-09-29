#ifndef NODE_ITERATOR_H
#define NODE_ITERATOR_H

#include <string>
#include <iostream>
#include <node.h>
#include "set.h"

class SingleNodeIterator : public node::ObjectWrap {
public:
    static void init();
    static v8::Local<v8::Object> New(int type, SetType::const_iterator new_iter, SetType::const_iterator new_end);

    const static int KEY_TYPE = 1;
    const static int VALUE_TYPE = 1 << 1;

private:
    SingleNodeIterator(SetType::const_iterator new_iter, SetType::const_iterator new_end);
    //~SingleNodeIterator();

    SetType::const_iterator iter;
    SetType::const_iterator end;

    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);

    // iterator.done : boolean
    static v8::Handle<v8::Value> GetDone(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    // iterator.value : boolean
    static v8::Handle<v8::Value> GetValue(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    // iterator.next() : undefined
    static v8::Handle<v8::Value> Next(const v8::Arguments &args);
};

#endif
