var test = require('tape');
var Set = require('../index.js');

test('test set', function (t) {
    var set = new Set();
    var iterator = set.values()
    t.ok(function() {set.add('raz').add('dwa').add({})});
    t.ok(function() {set.size}); // 3
    t.ok(function() {set.has('raz')}); // true
    t.ok(function() {set.has('foo')}); // false
    t.ok(function() {set.add('foo')}); // set
    t.ok(function() {set.size}); // 4
    t.ok(function() {set.has('dwa')}); // true
    t.ok(function() {set.delete('dwa')}); //true
    t.ok(function() {set.size}); // 3
    t.ok(function() {iterator.next()}); // { done: false, value: 'raz' }
    t.ok(function() {iterator.next()}); // { done: false, value: {} }
    t.ok(function() {iterator.next()}); // { done: false, value: 'foo' }
    t.ok(function() {iterator.next()}); // { done: true, value: undefined }
    // set.forEach(function (value) {
    //   // 'raz', {}, 'foo' iterated
    // });
    t.end();
})
