#ifndef V8_VALUE_HASHER_H
#define V8_VALUE_HASHER_H

#include <string>
#include <iostream>
#include <node.h>
#ifdef __APPLE__
#include <tr1/unordered_set>
#define hash std::tr1::hash
#else
#include <unordered_set>
#define hash std::hash
#endif


struct v8_value_hash
{
    size_t operator()(v8::UniquePersistent<v8::Value> *k) const {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Value> key = v8::Local<v8::Value>::New(isolate, *k);

        std::string s;
        if (key->IsString() || key->IsBoolean() || key->IsNumber()) {
            return hash<std::string>()(*v8::String::Utf8Value(key));
        }
        return hash<int>()(key->ToObject()->GetIdentityHash());
    }
};

struct v8_value_equal_to
{
    bool operator()(v8::UniquePersistent<v8::Value> *a, v8::UniquePersistent<v8::Value> *b) const {
        return *a == *b;
    }
};

#endif
