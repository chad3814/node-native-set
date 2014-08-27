#ifndef NODE_ITERATOR_H
#define NODE_ITERATOR_H

#include <string>
#include <iostream>
#include <node.h>
#include "set.h"

class SingleNodeIterator : public node::ObjectWrap {
public:
    static v8::Local<v8::Object> init(int type, SetType::iterator new_iter, SetType::iterator new_end);

    const static int KEY_TYPE = 1;
    const static int VALUE_TYPE = 1 << 1;

private:
    SingleNodeIterator(SetType::iterator new_iter, SetType::iterator new_end);
    //~SingleNodeIterator();

    SetType::iterator iter;
    SetType::iterator end;

    // iterator.done : boolean
    static v8::Handle<v8::Value> GetDone(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    // iterator.value : boolean
    static v8::Handle<v8::Value> GetValue(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    // iterator.next() : undefined
    static v8::Handle<v8::Value> Next(const v8::Arguments &args);
};

#endif
