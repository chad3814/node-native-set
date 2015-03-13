#include "iterator.h"
#include <iostream>

using namespace v8;

#define PROTO(proto, js_name, cpp_name)    \
    (proto)->Set(NanNew<String>(#js_name), \
                 NanNew<FunctionTemplate>(cpp_name)->GetFunction())

static Persistent<FunctionTemplate> key_value_tmplt;
static Persistent<FunctionTemplate> key_tmplt;
static Persistent<FunctionTemplate> value_tmplt;

void SingleNodeIterator::init() {
    Local<FunctionTemplate> tmplt = NanNew<FunctionTemplate>();
    tmplt->SetClassName(NanNew<String>("NodeIterator"));
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    Local<ObjectTemplate> key_value_prototype = tmplt->PrototypeTemplate();
    NanAssignPersistent(key_value_tmplt, tmplt);

    tmplt = NanNew<FunctionTemplate>();
    tmplt->SetClassName(NanNew<String>("NodeIterator"));
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    Local<ObjectTemplate> key_prototype = tmplt->PrototypeTemplate();
    NanAssignPersistent(key_tmplt, tmplt);

    tmplt = NanNew<FunctionTemplate>();
    tmplt->SetClassName(NanNew<String>("NodeIterator"));
    tmplt->InstanceTemplate()->SetInternalFieldCount(1);
    Local<ObjectTemplate> value_prototype = tmplt->PrototypeTemplate();
    NanAssignPersistent(value_tmplt, tmplt);

    PROTO(key_value_prototype, next, Next);
    PROTO(key_prototype, next, Next);
    PROTO(value_prototype, next, Next);
}

Local<Object> SingleNodeIterator::New(int type, SetType::const_iterator new_iter, SetType::const_iterator new_end) {
    Handle<FunctionTemplate> tmplt;
    if ((SingleNodeIterator::KEY_TYPE & type) && (SingleNodeIterator::VALUE_TYPE & type)) {
        tmplt = NanNew<FunctionTemplate>(key_value_tmplt);
    } else if (KEY_TYPE & type) {
        tmplt = NanNew<FunctionTemplate>(key_tmplt);
    } else {
        tmplt = NanNew<FunctionTemplate>(value_tmplt);
    }

    Local<Object> obj = tmplt->InstanceTemplate()->NewInstance();
    SingleNodeIterator *iter = new SingleNodeIterator(new_iter, new_end);

    iter->Wrap(obj);
    Local<String> key = NanNew<String>("key");
    Local<String> value = NanNew<String>("value");
    Local<String> done = NanNew<String>("done");

    if (SingleNodeIterator::KEY_TYPE & type) {
        obj->SetAccessor(key, GetValue);
    }
    if (SingleNodeIterator::VALUE_TYPE & type) {
        obj->SetAccessor(value, GetValue);
    }

    obj->SetAccessor(done, GetDone);

    return obj;
}

SingleNodeIterator::SingleNodeIterator(SetType::const_iterator new_iter, SetType::const_iterator new_end) : iter(new_iter), end(new_end) {}


// iterator.done : boolean
NAN_GETTER(SingleNodeIterator::GetDone) {
    NanScope();

    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator>(args.This());

    if (obj->iter == obj->end) {
        NanReturnValue(NanTrue());
    }
    NanReturnValue(NanFalse());
}


// iterator.value : boolean
NAN_GETTER(SingleNodeIterator::GetValue) {
    NanScope();

    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator>(args.This());

    if (obj->iter == obj->end) {
        NanReturnUndefined();
    }

    NanReturnValue((*(obj->iter))->Extract());
}

// iterator.next() : undefined
NAN_METHOD(SingleNodeIterator::Next) {
    NanScope();

    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator >(args.This());

    if (obj->iter == obj->end) {
        NanReturnThis();
    }

    obj->iter++;

    NanReturnThis();
}
