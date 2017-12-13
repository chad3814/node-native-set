var test = require('tape');
var NodeSet = require('../index.js');

test('test set', function (t) {
    var set = new NodeSet();
    t.equal(set.size, 0, 'set size is 0');

    set.add('raz').add('dwa').add({});
    t.equal(set.size, 3, 'set size is 3');

    set.add('foo');

    t.ok(set.has('raz'), 'set has raz');

    set.delete('dwa')
    t.notOk(set.has('dwa'), 'dwa deleted')
    t.notOk(set.has('andrea'), 'andrea was not here')

    var iterator = set.values();
    t.ok(iterator.next());
    // t.ok(function() {iterator.next()}); // { done: false, value: 'raz' }
    // t.ok(function() {iterator.next()}); // { done: false, value: {} }
    // t.ok(function() {iterator.next()}); // { done: false, value: 'foo' }
    // t.ok(function() {iterator.next()}); // { done: true, value: undefined }
    // test array
    // test iterator

    t.end();
})

test('test iterator', function(t) {
  var set = new Set();
  var iterator = set.values();
  t.end();
})
