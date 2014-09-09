#include "iterator.h"
#include <iostream>

using namespace v8;

int iterator_count = 0;

Local<Object> SingleNodeIterator::init(int type, SetType::iterator new_iter, SetType::iterator new_end) {
    Local<ObjectTemplate> tmplt = ObjectTemplate::New();
    tmplt->SetInternalFieldCount(1);

    SingleNodeIterator *iter = new SingleNodeIterator(new_iter, new_end, iterator_count);
    iterator_count++;

    if (SingleNodeIterator::KEY_TYPE & type) {
        tmplt->SetAccessor(String::New("key"), GetValue);
    }

    if (SingleNodeIterator::VALUE_TYPE & type) {
        tmplt->SetAccessor(String::New("value"), GetValue);
    }

    tmplt->SetAccessor(String::New("done"), GetDone);
    tmplt->Set(String::New("next"), FunctionTemplate::New(Next)->GetFunction());
    Local<Object> obj = tmplt->NewInstance();

    std::cout << "New SingleNodeIterator: " << iter->my_number << "\n";
    iter->Wrap(obj);
    return obj;
}

SingleNodeIterator::SingleNodeIterator(SetType::iterator new_iter, SetType::iterator new_end, int number) : iter(new_iter), end(new_end), my_number(number) {}


// iterator.done : boolean
Handle<Value> SingleNodeIterator::GetDone(Local<String> property, const AccessorInfo &info) {
    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator>(info.Holder());

    std::cout << "check if SignleNodeIterator " << obj->my_number << " is done: " << (obj->iter == obj->end) << "\n";
    return Boolean::New(obj->iter == obj->end);
}


// iterator.value : boolean
Handle<Value> SingleNodeIterator::GetValue(Local<String> property, const AccessorInfo &info) {
    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator>(info.Holder());

    if (obj->iter == obj->end) {
        std::cout << "tried to get value past end for SingleNodeIterator " << obj->my_number << "\n";
        return Undefined();
    }

    std::cout << "got a value for SingleNodeIterator " << obj->my_number << "\n";
    return *(obj->iter);
}

// iterator.next() : undefined
v8::Handle<v8::Value> SingleNodeIterator::Next(const v8::Arguments &args) {
    HandleScope scope;

    SingleNodeIterator *obj = ObjectWrap::Unwrap<SingleNodeIterator >(args.This());

    if (obj->iter == obj->end) {
        std::cout << "called next() past end of SingleNodeIterator " << obj->my_number << "\n";
        return scope.Close(args.This());
    }

    obj->iter++;

    std::cout << "called next() on SingleNodeIterator " << obj->my_number << "\n";
    return scope.Close(args.This());
}
