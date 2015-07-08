#include "set.h"
#include "iterator.h"
#include <iostream>

using namespace v8;

#define PROTO(proto, js_name, cpp_name) \
    (proto)->Set(NanNew<String>(#js_name), \
                 NanNew<FunctionTemplate>(cpp_name)->GetFunction())


void NodeSet::init(Handle<Object> exports) {
    Local<FunctionTemplate> constructor = NanNew<FunctionTemplate>(Constructor);
    constructor->SetClassName(NanNew<String>("NodeSet"));
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    Local<ObjectTemplate> prototype = constructor->PrototypeTemplate();
    PROTO(prototype, add, Add);
    PROTO(prototype, has, Has);
    PROTO(prototype, entries, Entries);
    PROTO(prototype, keys, Keys);
    PROTO(prototype, values, Values);
    PROTO(prototype, delete, Delete);
    PROTO(prototype, clear, Clear);
    PROTO(prototype, forEach, ForEach);
    PROTO(prototype, rehash, Rehash);
    PROTO(prototype, reserve, Reserve);

    exports->Set(NanNew<String>("NodeSet"), constructor->GetFunction());

    SingleNodeIterator::init();
}

NodeSet::NodeSet() {}

NodeSet::NodeSet(size_t buckets) : set(buckets) {}

NodeSet::~NodeSet() {
    for(SetType::const_iterator itr = this->set.begin(); itr != this->set.end(); ) {
        (*itr)->Reset();
        delete *itr;

        itr = this->set.erase(itr);
    }
}

NAN_METHOD(NodeSet::Constructor) {
    NanScope();
    NodeSet *obj = new NodeSet();
    Local<Array> arr;
    Local<Function> adder;
    uint32_t i;
    Local<String> add = NanNew<String>("add");
    Local<String> next = NanNew<String>("next");
    Local<String> done = NanNew<String>("done");
    Local<String> value = NanNew<String>("value");
    Local<Object> iter;
    Local<Function> next_func;
    Local<Value> func_args[1];
    Local<Value> empty_args[0];

    obj->Wrap(args.This());
    args.This()->SetAccessor(NanNew<String>("size"), Size);

    if(args.Length() > 0) {
        if (!args.This()->Has(add) || !args.This()->Get(add)->IsFunction()) {
            NanThrowTypeError("Invalid add method");
            NanReturnThis();
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

    NanReturnValue(args.This());
}

NAN_METHOD(NodeSet::Has) {
    NanScope();

    if (args.Length() < 1 || args[0]->IsUndefined() || args[0]->IsNull()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnValue(NanFalse());
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());
    Isolate *isolate = Isolate::GetCurrent();
    UniquePersistent<Value> *persistent = new UniquePersistent<Value>(isolate, args[0]);

    SetType::const_iterator itr = obj->set.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->set.end()) {
        NanReturnValue(NanFalse());
    }

    NanReturnValue(NanTrue());
}

NAN_METHOD(NodeSet::Add) {
    NanScope();

    if (args.Length() < 1) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());
    Isolate *isolate = Isolate::GetCurrent();
    UniquePersistent<Value> *persistent;
    int i;

    for (i = 0; i < args.Length(); i += 1) {
        if (args[i]->IsUndefined() || args[i]->IsNull()) {
            NanThrowTypeError("Wrong arguments: can't add undefined or null");
            NanReturnUndefined();
        }

        persistent = new UniquePersistent<Value>(isolate, args[i]);
        SetType::const_iterator itr = obj->set.find(persistent);

        // value doesn't already exists
        if (itr == obj->set.end()) {
            obj->set.insert(persistent);
        } else {
            persistent->Reset();
            delete persistent;
        }
    }

    //Return this
    NanReturnThis();
}

NAN_METHOD(NodeSet::Entries) {
    NanScope();

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Local<Object> iter = SingleNodeIterator::New(SingleNodeIterator::KEY_TYPE | SingleNodeIterator::VALUE_TYPE, obj->set.begin(), obj->set.end());

    NanReturnValue(iter);
}

NAN_METHOD(NodeSet::Keys) {
    NanScope();

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Local<Object> iter = SingleNodeIterator::New(SingleNodeIterator::KEY_TYPE, obj->set.begin(), obj->set.end());

    NanReturnValue(iter);
}

NAN_METHOD(NodeSet::Values) {
    NanScope();

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    Local<Object> iter = SingleNodeIterator::New(SingleNodeIterator::VALUE_TYPE, obj->set.begin(), obj->set.end());

    NanReturnValue(iter);
}

NAN_METHOD(NodeSet::Delete) {
    NanScope();

    if (args.Length() < 1 || args[0]->IsUndefined() || args[0]->IsNull()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnValue(NanFalse());
    }

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());
    Isolate *isolate = Isolate::GetCurrent();
    UniquePersistent<Value> *persistent = new UniquePersistent<Value>(isolate, args[0]);

    SetType::const_iterator itr = obj->set.find(persistent);
    persistent->Reset();
    delete persistent;

    if(itr == obj->set.end()) {
        NanReturnValue(NanFalse());
    }

    (*itr)->Reset();
    delete *itr;
    obj->set.erase(itr);

    NanReturnValue(NanTrue());
}

NAN_METHOD(NodeSet::Clear) {
    NanScope();

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    for(SetType::const_iterator itr = obj->set.begin(); itr != obj->set.end(); ) {
        (*itr)->Reset();
        delete *itr;

        itr = obj->set.erase(itr);
    }

    NanReturnUndefined();
}

NAN_GETTER(NodeSet::Size) {
    NanScope();

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());
    uint32_t size = obj->set.size();

    NanReturnValue(NanNew<Integer>(size));
}

NAN_METHOD(NodeSet::Rehash) {
    NanScope();

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    size_t buckets = args[0]->Int32Value();

    obj->set.rehash(buckets);

    NanReturnUndefined();
}

NAN_METHOD(NodeSet::Reserve) {
    NanScope();

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());

    size_t elements = args[0]->Int32Value();

    obj->set.rehash(elements);

    NanReturnUndefined();
}

NAN_METHOD(NodeSet::ForEach) {
    NanScope();

    NodeSet *obj = ObjectWrap::Unwrap<NodeSet>(args.This());
    Isolate* isolate = Isolate::GetCurrent();

    if (args.Length() < 1 || !args[0]->IsFunction()) {
        NanThrowTypeError("Wrong arguments");
        NanReturnUndefined();
    }
    Local<Function> cb = Local<Function>::Cast(args[0]);

    Local<Object> ctx;
    if (args.Length() > 1 && args[1]->IsObject()) {
        ctx = args[1]->ToObject();
    } else {
        ctx = NanGetCurrentContext()->Global();
    }

    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[2] = args.This();

    SetType::const_iterator itr = obj->set.begin();

    while (itr != obj->set.end()) {
        argv[0] = Local<Value>::New(isolate, **itr);
        argv[1] = argv[0];
        cb->Call(ctx, argc, argv);
        itr++;
    }

    NanReturnUndefined();
}
