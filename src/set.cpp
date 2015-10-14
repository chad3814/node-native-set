#include "set.h"
#include "iterator.h"
#include <iostream>

using namespace v8;

void NodeSet::init(Local<Object> target) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> constructor = Nan::New<FunctionTemplate>(Constructor);

    constructor->SetClassName(Nan::New("NodeSet").ToLocalChecked());
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(constructor, "add", Add);
    Nan::SetPrototypeMethod(constructor, "has", Has);
    Nan::SetPrototypeMethod(constructor, "entries", Entries);
    Nan::SetPrototypeMethod(constructor, "keys", Keys);
    Nan::SetPrototypeMethod(constructor, "values", Values);
    Nan::SetPrototypeMethod(constructor, "delete", Delete);
    Nan::SetPrototypeMethod(constructor, "clear", Clear);
    Nan::SetPrototypeMethod(constructor, "forEach", ForEach);
    Nan::SetPrototypeMethod(constructor, "rehash", Rehash);
    Nan::SetPrototypeMethod(constructor, "reserve", Reserve);

    target->Set(Nan::New("NodeSet").ToLocalChecked(), constructor->GetFunction());

    SingleNodeIterator::init(target);
}

NodeSet::NodeSet() {}

NodeSet::~NodeSet() {
    for(SetType::const_iterator itr = this->set.begin(); itr != this->set.end(); ) {
        (*itr)->Reset();

        delete *itr;

        itr = this->set.erase(itr);
    }
}

NAN_METHOD(NodeSet::Constructor) {
    Nan::HandleScope scope;
    NodeSet *obj = new NodeSet();

    Local<Function> adder;
    Local<Array> arr;
    uint32_t i;
    Local<String> add = Nan::New("add").ToLocalChecked();
    Local<String> next = Nan::New("next").ToLocalChecked();
    Local<String> done = Nan::New("done").ToLocalChecked();
    Local<String> value = Nan::New("value").ToLocalChecked();
    Local<Object> iter;
    Local<Value> func_info[1];

    obj->Wrap(info.This());
    Nan::SetAccessor(info.This(), Nan::New("size").ToLocalChecked(), Size);
    info.GetReturnValue().Set(info.This());

    if(info.Length() > 0) {
        if (!info.This()->Has(add) || !Nan::Get(info.This(), add).ToLocalChecked()->IsFunction()) {
            Nan::ThrowTypeError("Invalid add method");
            return;
        }
        Nan::Callback adder(Nan::Get(info.This(), add).ToLocalChecked().As<Function>());
        if (info[0]->IsArray()) {
            arr = info[0].As<Array>();
            for (i = 0; i < arr->Length(); i += 1) {
                func_info[0] = Nan::Get(arr, i).ToLocalChecked();
                adder.Call(info.This(), 1, func_info);
            }
        } else if (info[0]->IsObject()) {
            iter = Nan::To<Object>(info[0]).ToLocalChecked();
            if (iter->Has(next) && iter->Get(next)->IsFunction() && iter->Has(value) && iter->Has(done)) {
                Nan::Callback next_func(Nan::Get(iter, next).ToLocalChecked().As<Function>());
                // a value iterator
                while(!Nan::Get(iter, done).ToLocalChecked()->BooleanValue()) {
                    func_info[0] = Nan::Get(iter, value).ToLocalChecked();
                    adder.Call(info.This(), 1, func_info);
                    next_func.Call(iter, 0, 0);
                }
            }
        }
    }

    return;
}

NAN_METHOD(NodeSet::Has) {
    Nan::HandleScope scope;

    if (info.Length() < 1 || info[0]->IsUndefined() || info[0]->IsNull()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeSet *obj = Nan::ObjectWrap::Unwrap<NodeSet>(info.This());
    CopyablePersistent *persistent = new Nan::Persistent<Value, Nan::CopyablePersistentTraits<v8::Value> >(info[0]);

    SetType::const_iterator itr = obj->set.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->set.end()) {
        info.GetReturnValue().Set(Nan::False());
        return;
    }

    info.GetReturnValue().Set(Nan::True());
    return;
}

NAN_METHOD(NodeSet::Add) {
    Nan::HandleScope scope;

    if (info.Length() < 1 || info[0]->IsUndefined() || info[0]->IsNull()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeSet *obj = Nan::ObjectWrap::Unwrap<NodeSet>(info.This());
    CopyablePersistent *pvalue = new CopyablePersistent(info[0]);

    SetType::const_iterator itr = obj->set.find(pvalue);

    //overwriting an existing value
    if(itr != obj->set.end()) {
        (*itr)->Reset();

        delete *itr;

        obj->set.erase(itr);
    }

    obj->set.insert(pvalue);

    //Return this
    info.GetReturnValue().Set(info.This());
    return;
}

NAN_METHOD(NodeSet::Entries) {
    Nan::HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(info.This());

    Local<Object> iter = SingleNodeIterator::New(SingleNodeIterator::KEY_TYPE | SingleNodeIterator::VALUE_TYPE, obj->set.begin(), obj->set.end());

    info.GetReturnValue().Set(iter);
    return;
}

NAN_METHOD(NodeSet::Keys) {
    Nan::HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(info.This());

    Local<Object> iter = SingleNodeIterator::New(SingleNodeIterator::KEY_TYPE, obj->set.begin(), obj->set.end());

    info.GetReturnValue().Set(iter);
    return;
}

NAN_METHOD(NodeSet::Values) {
    Nan::HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(info.This());

    Local<Object> iter = SingleNodeIterator::New(SingleNodeIterator::VALUE_TYPE, obj->set.begin(), obj->set.end());

    info.GetReturnValue().Set(iter);
    return;
}

NAN_METHOD(NodeSet::Delete) {
    Nan::HandleScope scope;

    if (info.Length() < 1 || info[0]->IsUndefined() || info[0]->IsNull()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeSet *obj = Nan::ObjectWrap::Unwrap<NodeSet>(info.This());
    CopyablePersistent *persistent = new CopyablePersistent(info[0]);

    SetType::const_iterator itr = obj->set.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->set.end()) {
        //do nothing and return false
        info.GetReturnValue().Set(Nan::False());
        return;
    }

    (*itr)->Reset();

    delete *itr;

    obj->set.erase(itr);

    info.GetReturnValue().Set(Nan::True());
    return;
}

NAN_METHOD(NodeSet::Clear) {
    Nan::HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(info.This());

    for(SetType::const_iterator itr = obj->set.begin(); itr != obj->set.end(); ) {
        (*itr)->Reset();
        delete *itr;

        itr = obj->set.erase(itr);
    }

    info.GetReturnValue().Set(Nan::Undefined());
    return;
}

NAN_GETTER(NodeSet::Size) {
    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(info.This());
    uint32_t size = obj->set.size();

    info.GetReturnValue().Set(Nan::New<Integer>(size));
    return;
}

NAN_METHOD(NodeSet::Rehash) {
    Nan::HandleScope scope;

    if (info.Length() < 1 || !info[0]->IsInt32()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(info.This());

    size_t buckets = info[0]->Int32Value();

    obj->set.rehash(buckets);

    info.GetReturnValue().Set(Nan::Undefined());
    return;
}

NAN_METHOD(NodeSet::Reserve) {
    Nan::HandleScope scope;

    if (info.Length() < 1 || !info[0]->IsInt32()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(info.This());

    size_t elements = info[0]->Int32Value();

    obj->set.rehash(elements);

    info.GetReturnValue().Set(Nan::Undefined());
    return;
}

NAN_METHOD(NodeSet::ForEach) {
    Nan::HandleScope scope;

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(info.This());

    if (info.Length() < 1 || !info[0]->IsFunction()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }
    Local<Function> cb = info[0].As<v8::Function>();

    Local<Object> ctx;
    if (info.Length() > 1 && info[1]->IsObject()) {
        ctx = info[1]->ToObject();
    } else {
        ctx = Nan::GetCurrentContext()->Global();
    }

    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[2] = info.This();

    SetType::const_iterator itr = obj->set.begin();

    while (itr != obj->set.end()) {
        argv[0] = Local<Value>::New(Isolate::GetCurrent(), *(*itr));
        argv[1] = argv[0];
        cb->Call(ctx, argc, argv);
        itr++;
    }

    info.GetReturnValue().Set(Nan::Undefined());
    return;
}


extern "C" void
init (Local<Object> target) {
    Nan::HandleScope scope;

    NodeSet::init(target);
}

NODE_MODULE(set, init);
