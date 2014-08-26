'use strict';
var NativeSet = require('./build/Release/native').NodeSet;
var makeIterator = require('./es6-iterator');

var ES6Set = function (iterator) {
    this._native_set = new NativeSet();
    if (iterator && iterator.hasOwnProperty('done') && iterator.hasOwnProperty('next') && iterator.hasOwnProperty('value')) {
        while (!iterator.done) {
            this._native_set.add(iterator.value);
            iterator.next();
        }
    }
};

ES6Set.prototype = {
    has: function (key) {
        return this._native_set.has(key);
    },
    add: function (key, value) {
        return this._native_set.add(key, value);
    },
    delete: function (key) {
        return this._native_set.delete(key);
    },
    forEach: function (cb, ctx) {
        return this._native_set.forEach(function (value) {
            return cb.call(ctx, value, value, this);
        }, this);
    },
    clear: function () {
        return this._native_set.clear();
    },
    keys: function () {
        return makeIterator(this._native_set, 'value');
    },
    values: function () {
        return makeIterator(this._native_set, 'value');
    },
    entries: function () {
        return makeIterator(this._native_set, 'value');
    }
};

Object.defineProperty(ES6Set.prototype, 'size', {
    get: function () {
        return this._native_set.size();
    },
    enumerable: true,
    configurable: true
});

module.exports = ES6Set;
