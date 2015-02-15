#include "set.h"
#include "iterator.h"
#include <iostream>

using namespace v8;

void NodeSet::init(Handle<Object> exports) {
    Local<FunctionTemplate> constructor = FunctionTemplate::New(Constructor);
    constructor->SetClassName(String::NewSymbol("NodeSet"));
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Local<ObjectTemplate> prototype = constructor->PrototypeTemplate();
    prototype->Set("add", FunctionTemplate::New(Add)->GetFunction());
    prototype->Set("has", FunctionTemplate::New(Has)->GetFunction());
    prototype->Set("entries", FunctionTemplate::New(Entries)->GetFunction());
    prototype->Set("keys", FunctionTemplate::New(Keys)->GetFunction());
    prototype->Set("values", FunctionTemplate::New(Values)->GetFunction());
    prototype->Set("delete", FunctionTemplate::New(Delete)->GetFunction());
    prototype->Set("clear", FunctionTemplate::New(Clear)->GetFunction());
    prototype->Set("forEach", FunctionTemplate::New(ForEach)->GetFunction());
    prototype->Set("rehash", FunctionTemplate::New(Rehash)->GetFunction());
    prototype->Set("reserve", FunctionTemplate::New(Reserve)->GetFunction());

    exports->Set(String::NewSymbol("NodeSet"), Persistent<Function>::New(constructor->GetFunction()));

    SingleNodeIterator::init();
}

NodeSet::NodeSet() {}

NodeSet::NodeSet(size_t buckets) : set(buckets) {}

NodeSet::~NodeSet() {
    for(SetType::const_iterator itr = this->set.begin(); itr != this->set.end(); ) {
        Persistent<Value> value = Persistent<Value>::Persistent(*itr);
        value.Dispose();
        value.Clear();

        itr = this->set.erase(itr);
    }
}

Handle<Value> NodeSet::Constructor(const Arguments& args) {
    HandleScope scope;
    NodeSet *obj = new NodeSet();
    Local<Array> arr;
    SetType::const_iterator itr;
    Local<Function> adder;
    uint32_t i;
    Local<String> add = String::New("add");
    Local<String> next = String::New("next");
    Local<String> done = String::New("done");
    Local<String> value = String::New("value");
    Local<Object> iter;
    Local<Function> next_func;
    Local<Value> func_args[1];
    Local<Value> empty_args[0];

    obj->Wrap(args.This());
    args.This()->SetAccessor(String::New("size"), Size);

    if(args.Length() > 0) {
        if (!args.This()->Has(add) || !args.This()->Get(add)->IsFunction()) {
            ThrowException(Exception::TypeError(String::New("Invalid add method")));
            return scope.Close(args.This());
        }
        adder = Local<Function>::Cast(args.This()->Get(add));
        if (args[0]->IsArray()) {
            arr = Local<Array>::Cast(args[0]);
            for (i = 0; i < arr->Length(); i += 1) {
                func_args[0] = arr->Get(i);
                adder->Call(args.This(), 1, func_args);
            }
        } else if (args[0]->IsObject()) {
            iter = Local<Object>::Cast(args[0]);
            if (iter->Has(next) && iter->Get(next)->IsFunction() && iter->Has(value) && iter->Has(done)) {
                next_func = Local<Function>::Cast(iter->Get(next));
                // a value iterator
                while(!iter->Get(done)->BooleanValue()) {
                    func_args[0] = iter->Get(value);
                    adder->Call(args.This(), 1, func_args);
                    next_func->Call(iter, 0, empty_args);
                }
            }
        }
    }

    return scope.Close(args.This());
}

Handle<Value> NodeSet::Has(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Boolean::New(false));
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    SetType::const_iterator itr = obj->set.find(args[0]);

    if(itr == obj->set.end()) {
        return scope.Close(False());
    }

    return scope.Close(True());
}

Handle<Value> NodeSet::Add(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());
    int i;
    for (i = 0; i < args.Length(); i += 1) {
        SetType::const_iterator itr = obj->set.find(args[i]);

        // value already exists
        if (itr == obj->set.end()) {
            Persistent<Value> persistent = Persistent<Value>::New(args[i]);
            obj->set.insert(persistent);
        }
    }

    //Return this
    return scope.Close(args.This());
}

Handle<Value> NodeSet::Entries(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Local<Object> iter = SingleNodeIterator::New(SingleNodeIterator::KEY_TYPE | SingleNodeIterator::VALUE_TYPE, obj->set.begin(), obj->set.end());

    return scope.Close(iter);
}

Handle<Value> NodeSet::Keys(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Local<Object> iter = SingleNodeIterator::New(SingleNodeIterator::KEY_TYPE, obj->set.begin(), obj->set.end());

    return scope.Close(iter);
}

Handle<Value> NodeSet::Values(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Local<Object> iter = SingleNodeIterator::New(SingleNodeIterator::VALUE_TYPE, obj->set.begin(), obj->set.end());

    return scope.Close(iter);
}

Handle<Value> NodeSet::Delete(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Boolean::New(false));
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Persistent<Value> value = Persistent<Value>::New(args[0]);

    SetType::const_iterator itr = obj->set.find(value);

    if(itr == obj->set.end()) {
        return scope.Close(False()); //do nothing and return false
    }

    value.Dispose();
    value.Clear();

    obj->set.erase(itr);

    return scope.Close(True());
}

Handle<Value> NodeSet::Clear(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    for(SetType::const_iterator itr = obj->set.begin(); itr != obj->set.end(); ) {
        Persistent<Value> value = Persistent<Value>::Persistent(*itr);
        value.Dispose();
        value.Clear();

        itr = obj->set.erase(itr);
    }

    return scope.Close(Undefined());
}

Handle<Value> NodeSet::Size(Local<String> property, const AccessorInfo &info) {
    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(info.Holder());

    return Integer::New(obj->set.size());
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

Handle<Value> NodeSet::ForEach(const Arguments& args) {
    HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    if (!args[0]->IsFunction()) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    Local<Function> cb = Local<Function>::Cast(args[0]);
    const unsigned argc = 2;
    Handle<Value> argv[argc];

    SetType::const_iterator itr = obj->set.begin();

    while (itr != obj->set.end()) {
        argv[0] = *itr;
        argv[1] = *itr;
        cb->Call(Context::GetCurrent()->Global(), argc, argv);
        itr++;
    }

    return scope.Close(Undefined());
}
