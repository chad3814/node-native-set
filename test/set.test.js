var test = require('tape');
var request = require('request');
var queue = require('queue-async');
var zlib = require('zlib');
var forOf = require('es6-iterator/for-of');
var Set = require('es6-native-set');

test('test set', function (t) {
  var set = new Set();
  t.throws(function() {set.add('raz').add('dwa').add({})});
  t.throws(function() {set.size}); // 3
  t.throws(function() {set.has('raz')}); // true
  t.throws(function() {set.has('foo')}); // false
  t.throws(function() {set.add('foo')}); // set
  t.throws(function() {set.size}); // 4
  t.throws(function() {set.has('dwa')}); // true
  t.throws(function() {set.delete('dwa')}); //true
  t.throws(function() {set.size}); // 3
  set.forEach(function (value) {
    // 'raz', {}, 'foo' iterated
  });
}

test('test set', function (t) {
  var iterator = set.values()
  t.throws(function() {iterator.next()}); // { done: false, value: 'raz' }
  t.throws(function() {iterator.next()}); // { done: false, value: {} }
  t.throws(function() {iterator.next()}); // { done: false, value: 'foo' }
  t.throws(function() {iterator.next()}); // { done: true, value: undefined }
}
