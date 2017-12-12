var test = require('tape');
var Set = require('../index.js');

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
})

test('test iterator', function (t) {
  var iterator = set.values()
  t.throws(function() {iterator.next()}); // { done: false, value: 'raz' }
  t.throws(function() {iterator.next()}); // { done: false, value: {} }
  t.throws(function() {iterator.next()}); // { done: false, value: 'foo' }
  t.throws(function() {iterator.next()}); // { done: true, value: undefined }
})
