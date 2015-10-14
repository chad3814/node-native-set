#include "iterator.h"
#include <iostream>

using namespace v8;

Nan::Persistent<FunctionTemplate> SingleNodeIterator::constructor;

void SingleNodeIterator::init(Local<Object> target) {
    Local<FunctionTemplate> tmplt = Nan::New<FunctionTemplate>();
    constructor.Reset(tmplt);
    tmplt->SetClassName(Nan::New("NodeIterator").ToLocalChecked());
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetPrototypeMethod(tmplt, "next", Next);
}

Local<Object> SingleNodeIterator::New(int type, SetType::const_iterator new_iter, SetType::const_iterator new_end) {
    Local<FunctionTemplate> constr = Nan::New<FunctionTemplate>(constructor);
    Local<Object> obj = constr->InstanceTemplate()->NewInstance();
    SingleNodeIterator *iter = new SingleNodeIterator(new_iter, new_end);

    iter->Wrap(obj);

    Local<String> key = Nan::New("key").ToLocalChecked();
    Local<String> value = Nan::New("value").ToLocalChecked();
    Local<String> done = Nan::New("done").ToLocalChecked();

    if (SingleNodeIterator::KEY_TYPE & type) {
        Nan::SetAccessor(obj, key, GetValue);
    }
    if (SingleNodeIterator::VALUE_TYPE & type) {
        Nan::SetAccessor(obj, value, GetValue);
    }

    Nan::SetAccessor(obj, done, GetDone);

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
