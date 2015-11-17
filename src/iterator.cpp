#include "iterator.h"
#include <iostream>

using namespace v8;

Nan::Persistent<FunctionTemplate> SingleNodeIterator::k_constructor;
Nan::Persistent<FunctionTemplate> SingleNodeIterator::v_constructor;
Nan::Persistent<FunctionTemplate> SingleNodeIterator::kv_constructor;

void SingleNodeIterator::init(Local<Object> target) {
    Local<String> key = Nan::New("key").ToLocalChecked();
    Local<String> value = Nan::New("value").ToLocalChecked();
    Local<String> done = Nan::New("done").ToLocalChecked();
    Local<FunctionTemplate> k_tmplt = Nan::New<FunctionTemplate>();
    Local<FunctionTemplate> v_tmplt = Nan::New<FunctionTemplate>();
    Local<FunctionTemplate> kv_tmplt = Nan::New<FunctionTemplate>();

    k_tmplt->SetClassName(Nan::New("NodeIterator").ToLocalChecked());
    k_tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    k_constructor.Reset(k_tmplt);
    Nan::SetAccessor(k_tmplt->InstanceTemplate(), key, GetValue);
    Nan::SetAccessor(k_tmplt->InstanceTemplate(), done, GetDone);
    Nan::SetPrototypeMethod(k_tmplt, "next", Next);

    v_tmplt->SetClassName(Nan::New("NodeIterator").ToLocalChecked());
    v_tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    v_constructor.Reset(v_tmplt);
    Nan::SetAccessor(v_tmplt->InstanceTemplate(), value, GetValue);
    Nan::SetAccessor(v_tmplt->InstanceTemplate(), done, GetDone);
    Nan::SetPrototypeMethod(v_tmplt, "next", Next);

    kv_tmplt->SetClassName(Nan::New("NodeIterator").ToLocalChecked());
    kv_tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    kv_constructor.Reset(kv_tmplt);
    Nan::SetAccessor(kv_tmplt->InstanceTemplate(), key, GetValue);
    Nan::SetAccessor(kv_tmplt->InstanceTemplate(), value, GetValue);
    Nan::SetAccessor(kv_tmplt->InstanceTemplate(), done, GetDone);
    Nan::SetPrototypeMethod(kv_tmplt, "next", Next);
}

Local<Object> SingleNodeIterator::New(int type, SetType::const_iterator new_iter, SetType::const_iterator new_end) {
    Local<FunctionTemplate> constructor;
    Local<Object> obj;
    SingleNodeIterator *iter = new SingleNodeIterator(new_iter, new_end);

    if (SingleNodeIterator::KEY_TYPE & type) {
        if (SingleNodeIterator::VALUE_TYPE & type) {
            constructor = Nan::New<FunctionTemplate>(kv_constructor);
        } else {
            constructor = Nan::New<FunctionTemplate>(k_constructor);
        }
    } else {
        constructor = Nan::New<FunctionTemplate>(v_constructor);
    }

    obj = constructor->InstanceTemplate()->NewInstance();

    iter->Wrap(obj);

    return obj;
}

SingleNodeIterator::SingleNodeIterator(SetType::const_iterator new_iter, SetType::const_iterator new_end) : iter(new_iter), end(new_end) {}


// iterator.done : boolean
NAN_GETTER(SingleNodeIterator::GetDone) {
    Nan::HandleScope scope;

    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator>(info.This());

    if (obj->iter == obj->end) {
        info.GetReturnValue().Set(Nan::True());
        return;
    }
    info.GetReturnValue().Set(Nan::False());
    return;
}


// iterator.value : boolean
NAN_GETTER(SingleNodeIterator::GetValue) {
    Nan::HandleScope scope;

    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator>(info.This());

    if (obj->iter == obj->end) {
        info.GetReturnValue().Set(Nan::Undefined());
        return;
    }

    info.GetReturnValue().Set(Local<Value>::New(Isolate::GetCurrent(), *(*obj->iter)));
}

// iterator.next() : undefined
NAN_METHOD(SingleNodeIterator::Next) {
    Nan::HandleScope scope;

    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator >(info.This());

    if (obj->iter == obj->end) {
        info.GetReturnValue().Set(info.This());
        return;
    }

    obj->iter++;
    info.GetReturnValue().Set(info.This());
    return;
}
