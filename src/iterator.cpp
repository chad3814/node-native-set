#include "iterator.h"
#include <iostream>

using namespace v8;

Nan::Persistent<FunctionTemplate> SingleNodeIterator::_constructor;

void SingleNodeIterator::init(Local<Object> target) {
    Local<FunctionTemplate> tmplt = Nan::New<FunctionTemplate>();
    tmplt->SetClassName(Nan::New("NodeIterator").ToLocalChecked());
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    _constructor.Reset(tmplt);
    Nan::SetPrototypeMethod(tmplt, "next", Next);

    // got to do the Symbol.iterator function by hand, no Nan support
    Local<Symbol> symbol_iterator = Symbol::GetIterator(Isolate::GetCurrent());
    Local<FunctionTemplate> get_this_templt = Nan::New<FunctionTemplate>(
        GetThis
        , Local<Value>()
        , Nan::New<Signature>(tmplt));
    tmplt->PrototypeTemplate()->Set(symbol_iterator, get_this_templt);
    get_this_templt->SetClassName(Nan::New("Symbol(Symbol.iterator)").ToLocalChecked());

}

Local<Object> SingleNodeIterator::New(int type, NodeSet *set_obj) {
    Local<FunctionTemplate> constructor = Nan::New<FunctionTemplate>(_constructor);
    Local<Object> obj;
    SingleNodeIterator *iter = new SingleNodeIterator(type, set_obj);

    obj = constructor->InstanceTemplate()->NewInstance();

    iter->Wrap(obj);

    return obj;
}

SingleNodeIterator::SingleNodeIterator(int type, NodeSet *set_obj) {
    this->_set_obj = set_obj;
    this->_version = set_obj->StartIterator();
    this->_iter = set_obj->GetBegin();
    this->_end = set_obj->GetEnd();
    this->_type = type;
}

SingleNodeIterator::~SingleNodeIterator() {
    this->_set_obj->StopIterator();
}

// iterator[Symbol.iterator]() : this
NAN_METHOD(SingleNodeIterator::GetThis) {
    Nan::HandleScope scope;

    info.GetReturnValue().Set(info.This());
}

// iterator.next() : {value:, done:}
NAN_METHOD(SingleNodeIterator::Next) {
    Nan::HandleScope scope;

    SingleNodeIterator *iter = ObjectWrap::Unwrap<SingleNodeIterator >(info.This());
    Local<String> value = Nan::New("value").ToLocalChecked();
    Local<String> done = Nan::New("done").ToLocalChecked();
    Local<Object> obj = Nan::New<Object>();
    Local<Array> arr;


    if (iter->_iter == iter->_end) {
        Nan::Set(obj, value, Nan::Undefined());
        Nan::Set(obj, done, Nan::True());
        info.GetReturnValue().Set(obj);
        return;
    }

    if (iter->_type == KEY_TYPE) {
        obj->Set(value, iter->_iter->GetLocal());
    } else if (iter->_type == VALUE_TYPE) {
        obj->Set(value, iter->_iter->GetLocal());
    } else {
        arr = Nan::New<Array>(2);
        arr->Set(0, iter->_iter->GetLocal());
        arr->Set(1, iter->_iter->GetLocal());
        obj->Set(value, arr);
    }
    obj->Set(done, Nan::False());

    iter->_iter++;
    info.GetReturnValue().Set(obj);
    return;
}
