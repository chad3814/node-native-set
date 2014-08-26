#include "set.h"
#include <iostream>
#ifndef __APPLE__
#include <unordered_set>
#endif

using namespace v8;

void NodeSet::init(Handle<Object> exports) {
    Local<FunctionTemplate> constructor = FunctionTemplate::New(Constructor);
    constructor->SetClassName(String::NewSymbol("NodeSet"));
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    auto prototype = constructor->PrototypeTemplate();
    prototype->Set("add", FunctionTemplate::New(Add)->GetFunction());
    prototype->Set("has", FunctionTemplate::New(Has)->GetFunction());
    prototype->Set("entries", FunctionTemplate::New(Entries)->GetFunction()); // should return an iterator for key+value (where both are the set value entry)
    prototype->Set("keys", FunctionTemplate::New(Entries)->GetFunction()); // should return an iterator for key (where it is the as the set value entry), same as entries for now
    prototype->Set("values", FunctionTemplate::New(Entries)->GetFunction()); // should return an iterator for value, same as entries for now
    prototype->Set("delete", FunctionTemplate::New(Delete)->GetFunction());
    prototype->Set("clear", FunctionTemplate::New(Clear)->GetFunction());
    prototype->Set("size", FunctionTemplate::New(Size)->GetFunction()); // should be an accessor
    prototype->Set("forEach", FunctionTemplate::New(ForEach)->GetFunction());
    prototype->Set("rehash", FunctionTemplate::New(Rehash)->GetFunction());
    prototype->Set("reserve", FunctionTemplate::New(Reserve)->GetFunction());
    prototype->Set("max_load_factor", FunctionTemplate::New(MaxLoadFactor)->GetFunction());

    exports->Set(String::NewSymbol("NodeSet"), Persistent<Function>::New(constructor->GetFunction()));
}

NodeSet::NodeSet() {}

NodeSet::NodeSet(size_t buckets) : set(buckets) {}

NodeSet::~NodeSet() {
    for(auto itr = this->set.begin(); itr != this->set.end(); ) {
        Persistent<Value> value = *itr;
        value.Dispose();

        itr = this->set.erase(itr);
    }
}

Handle<Value> NodeSet::Constructor(const Arguments& args) {
    NodeSet *obj;

    if(args.Length() > 0 && args[0]->IsInt32()) {
        int buckets = args[0]->Int32Value();
        obj = new NodeSet(buckets);
    } else {
        obj = new NodeSet();
    }

    obj->Wrap(args.This());

    return args.This();
}

Handle<Value> NodeSet::Has(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Boolean::New(false));
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Persistent<Value> value = Persistent<Value>(args[0]);

    SetType::const_iterator itr = obj->set.find(value);

    if(itr == obj->set.end()) {
        return scope.Close(Boolean::New(false));
    }

    return scope.Close(Boolean::New(true));
}

Handle<Value> NodeSet::Add(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Persistent<Value> persistent = Persistent<Value>::New(args[0]);

    SetType::const_iterator itr = obj->set.find(persistent);

    // value already exists
    if (itr != obj->set.end()) {
        return scope.Close(args.This()); // return this
    }

    obj->set.insert(persistent);

    //Return this
    return scope.Close(args.This());
}

Handle<Value> NodeSet::Entries(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Local<Array> array = Array::New();

    int i = 0;
    for(auto itr = obj->set.begin(); itr != obj->set.end(); ++itr, ++i) {
        array->Set(Integer::New(i), *itr);
    }

    return scope.Close(array);
}

Handle<Value> NodeSet::Delete(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Boolean::New(false));
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Persistent<Value> value = Persistent<Value>::New(args[0]);

    auto itr = obj->set.find(value);

    if(itr == obj->set.end()) {
        return scope.Close(Boolean::New(false)); //do nothing and return false
    }

    value.Dispose();

    obj->set.erase(itr);

    return scope.Close(Boolean::New(true));
}

Handle<Value> NodeSet::Clear(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    for(auto itr = obj->set.begin(); itr != obj->set.end(); ) {
        Persistent<Value> value = *itr;
        value.Dispose();

        itr = obj->set.erase(itr);
    }

    return scope.Close(Undefined());
}

Handle<Value> NodeSet::Size(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    return scope.Close(Integer::New(obj->set.size()));
}

Handle<Value> NodeSet::Rehash(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    size_t buckets = args[0]->Int32Value();

    obj->set.rehash(buckets);

    return scope.Close(Undefined());
}

Handle<Value> NodeSet::Reserve(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    size_t elements = args[0]->Int32Value();

    obj->set.rehash(elements);

    return scope.Close(Undefined());
}

Handle<Value> NodeSet::MaxLoadFactor(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    if(args.Length() > 0) {
        Number *num = static_cast<Number*>(*args[0]);
        float factor = (float)num->Value();
        float old_factor = obj->set.max_load_factor();
        if(factor > 0)
            obj->set.max_load_factor(factor);

        return scope.Close(Number::New((double)old_factor));
    } else {
        float old_factor = obj->set.max_load_factor();

        return scope.Close(Number::New((double)old_factor));
    }
}

Handle<Value> NodeSet::ForEach(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    if (!args[0]->IsFunction()) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    Local<Function> cb = Local<Function>::Cast(args[0]);
    const unsigned argc = 2;
    Persistent<Value> argv[argc];

    SetType::const_iterator itr = obj->set.begin();

    while (itr != obj->set.end()) {
        argv[0] = Persistent<Value>::New(*itr);
        argv[1] = Persistent<Value>::New(*itr);
        cb->Call(Context::GetCurrent()->Global(), argc, argv);
        itr++;
    }

    return scope.Close(Undefined());
}
