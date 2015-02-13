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
    size_t operator()(v8::Handle<v8::Value> key) const {
        std::string s;
        if (key->IsString() || key->IsBoolean() || key->IsNumber()) {
            return hash<std::string>()(*v8::String::AsciiValue(key->ToString()));
        }
        return hash<int>()(v8::Handle<v8::Object>::Cast(key)->GetIdentityHash());
    }
};

struct v8_value_equal_to
{
    bool operator()(v8::Handle<v8::Value> a, v8::Handle<v8::Value> b) const {
        if (a->Equals(b)) {          /* same as JS == */
            return true;
        }

        /* try basic types, if it is one, then can't be equal */
        if (a->IsString() || b->IsString()) {
            return false;
        }
        if (a->IsNumber() || b->IsNumber()) {
            return false;
        }
        if (a->IsBoolean() || b->IsBoolean()) {
            return false;
        }

        // if the identity hashes are equal, then it's equal
        return v8::Handle<v8::Object>::Cast(a)->GetIdentityHash() == v8::Handle<v8::Object>::Cast(b)->GetIdentityHash();
    }
};

#endif
