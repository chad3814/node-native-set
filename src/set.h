#ifndef NODE_SET_H
#define NODE_SET_H

#include <string>
#include <iostream>
#ifdef __APPLE__
#include <tr1/unordered_set>
#define unordered_set std::tr1::unordered_set
#define hash std::tr1::hash
#else
#include <unordered_set>
#define unordered_set std::unordered_set
#define hash std::hash
#endif
#include <node.h>


template<>
struct hash<v8::Persistent<v8::Value> >
{
    size_t operator()(v8::Persistent<v8::Value> key) const {
        std::string s;
        if (key->IsString() || key->IsBoolean() || key->IsDate() || key->IsRegExp() || key->IsStringObject() || key->IsNumberObject() || key->IsBooleanObject()) {
            s = *v8::String::AsciiValue(key->ToString());
        } else {
            v8::Handle<v8::Context> context = v8::Context::GetCurrent();
            v8::Handle<v8::Object> global = context->Global();
            v8::Handle<v8::Object> JSON = global->Get(v8::String::New("JSON"))->ToObject();
            v8::Handle<v8::Function> stringify = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::New("stringify")));
            s = *v8::String::AsciiValue(stringify->Call(JSON, 1, new v8::Handle<v8::Value>(key)));
        }
        //std::cout << "hasher key " << s << '\n';

        return hash<std::string>()(s);
    }
};

template<>
struct std::equal_to<v8::Persistent<v8::Value> >
{
    bool operator()(v8::Handle<v8::Value> a, v8::Handle<v8::Value> b) const {
        if (a->Equals(b)) {          /* same as JS == */
            return true;
        }

        uint32_t length;
        uint32_t i;

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

        /* try complex types that need a deeper equality */
        if (a->IsDate()) {
            if (!b->IsDate()) {
                return false;
            }
            return a->NumberValue() == b->NumberValue();
        }
        if (a->IsRegExp()) {
            if (!b->IsRegExp()) {
                return false;
            }
            v8::RegExp *a_reg = v8::RegExp::Cast(*a);
            v8::RegExp *b_reg = v8::RegExp::Cast(*b);
            return (a_reg->GetFlags() == b_reg->GetFlags()) && a_reg->GetSource()->Equals(b_reg->GetSource());
        }
        if (a->IsBooleanObject()) {
            if (!b->IsBooleanObject()) {
                return false;
            }
            return a->BooleanValue() == b->BooleanValue();
        }
        if (a->IsNumberObject()) {
            if (!b->IsNumberObject()) {
                return false;
            }
            return a->NumberValue() == b->NumberValue();
        }
        if (a->IsStringObject()) {
            if (!b->IsStringObject()) {
                return false;
            }
            return v8::String::Cast(*a)->Equals(*(new v8::Handle<v8::String>(v8::String::Cast(*b))));
        }
        if (a->IsArray()) {
            if (!b->IsArray()) {
                return false;
            }
            v8::Array *a_arr = v8::Array::Cast(*a);
            v8::Array *b_arr = v8::Array::Cast(*b);
            length = a_arr->Length();
            if (length != b_arr->Length()) {
                return false;
            }
            for (i = 0; i < length; ++i) {
                if (!std::equal_to<v8::Persistent<v8::Value> >()(*(new v8::Handle<v8::Value>(*a_arr->CloneElementAt(i))), *(new v8::Handle<v8::Value>(*b_arr->CloneElementAt(i))))) {
                    return false;
                }
            }
            return true;
        }

        /* generic Object deep equals, but only own properties, not prototype properties */
        v8::Object *a_obj = v8::Object::Cast(*a);
        v8::Object *b_obj = v8::Object::Cast(*b);
        v8::Local<v8::Array> properties = a_obj->GetOwnPropertyNames();
        v8::Local<v8::String> *property_name;
        length = properties->Length();
        if (length != b_obj->GetOwnPropertyNames()->Length()) {
            return false;
        }
        for (i = 0; i < length; ++i) {
            property_name = new v8::Local<v8::String>(v8::String::Cast(*properties->Get(i)));
            if (!b_obj->Has(*property_name)) {
                return false;
            }
            if (!std::equal_to<v8::Persistent<v8::Value> >()(*(new v8::Handle<v8::Value>(*a_obj->Get(*property_name))), *(new v8::Handle<v8::Value>(*b_obj->Get(*property_name))))) {
                return false;
            }
        }
        return true;
    }
};

typedef unordered_set<v8::Persistent<v8::Value> > SetType;

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

  //set.max_load_factor() or set.max_load_factor(factor) : current or old max_load_factor
  static v8::Handle<v8::Value> MaxLoadFactor(const v8::Arguments &args);

  //set.forEach(function (key, value) {...}) : undefined
  static v8::Handle<v8::Value> ForEach(const v8::Arguments &args);
};

#endif
