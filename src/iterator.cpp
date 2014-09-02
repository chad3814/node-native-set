#include "iterator.h"
#include <iostream>

using namespace v8;

Local<Object> SingleNodeIterator::init(int type, SetType::iterator new_iter, SetType::iterator new_end) {
    Local<ObjectTemplate> tmplt = ObjectTemplate::New();
    tmplt->SetInternalFieldCount(1);

    SingleNodeIterator *iter = new SingleNodeIterator(new_iter, new_end);

    if (SingleNodeIterator::KEY_TYPE & type) {
        tmplt->SetAccessor(String::New("key"), GetValue);
    }

    if (SingleNodeIterator::VALUE_TYPE & type) {
        tmplt->SetAccessor(String::New("value"), GetValue);
    }

    tmplt->SetAccessor(String::New("done"), GetDone);
    tmplt->Set(String::New("next"), FunctionTemplate::New(Next)->GetFunction());
    Local<Object> obj = tmplt->NewInstance();

    iter->Wrap(obj);
    return obj;
}

SingleNodeIterator::SingleNodeIterator(SetType::iterator new_iter, SetType::iterator new_end) : iter(new_iter), end(new_end) {}


// iterator.done : boolean
Handle<Value> SingleNodeIterator::GetDone(Local<String> property, const AccessorInfo &info) {
    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator>(info.Holder());

    return Boolean::New(obj->iter == obj->end);
}


// iterator.value : boolean
Handle<Value> SingleNodeIterator::GetValue(Local<String> property, const AccessorInfo &info) {
    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator>(info.Holder());

    if (obj->iter == obj->end) {
        return Undefined();
    }

    return *(obj->iter);
}

// iterator.next() : undefined
v8::Handle<v8::Value> SingleNodeIterator::Next(const v8::Arguments &args) {
    HandleScope scope;

    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator >(args.This());

    if (obj->iter == obj->end) {
        return scope.Close(args.This());
    }

    obj->iter++;

    return scope.Close(args.This());
}
