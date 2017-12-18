'use strict';

const test = require('tape');

[['builtIn-js', Set], ['node-native-set', require('../index.js')]].forEach(([setType, Set]) => {
  test(`test ${setType} constructor`, (t) => {
    t.doesNotThrow(() => {new Set();}, 'can construct an empty new Set()');
    t.doesNotThrow(() => {new Set(null);}, 'can construct with null');
    t.doesNotThrow(() => {new Set([1,2,3]);}, 'can construct an iterable array of integers');

    let stringy;
    t.doesNotThrow(() => {
      stringy = new Set('stringy');
    }, 'can construct set with string');
    t.equal(stringy.size, 7, 'string set size is as expected');

    let arrayOfArrays;
    t.doesNotThrow(() => {
      arrayOfArrays = new Set([[1,2], [3,4]]);
    }, 'can construct an array of arrays');
    t.equal(arrayOfArrays.size, 2, 'arrayOfArrays size is as expected');

    t.throws(() => {new Set(1,2,3);}, TypeError,'throws error if not an array');
    t.throws(() => {new Set(9);}, TypeError, 'throws error with non-iterable obj');
    t.throws(() => {new Set({ 1:2, 3:4 });}, 'throws error on object constructor');

    t.end();
  });

  test(`test ${setType} add method`, (t) => {
    const mySet = new Set();
    const startSize = mySet.size;

    t.doesNotThrow(() => {mySet.add('a string');}, 'can add string to the set');
    t.equal(mySet.size, startSize + 1, 'adding value increases size by 1');
    t.doesNotThrow(() => {mySet.add({});}, 'can add object to a set');
    t.doesNotThrow(() => {mySet.add(()=>{});}, 'can add function to a set');
    t.skip('diverging from builtin; cannot add null to a set');
    t.equal(mySet.add(1), mySet, 'add returns the set');
    t.doesNotThrow(() => {mySet.add(1);}, 'can call add with only 1 argument');
    t.doesNotThrow(() => {mySet.add(2,3,4);}, 'can call add with more than 1 argument');
    t.ok(mySet.has(2) && !(mySet.has(3)), 'add ignores arguments after 1st arg');

    t.end();
  });

  test(`test ${setType} has method`, (assert) => {
    const mySet = new Set();
    mySet.add(0);
    mySet.add(1);
    assert.ok(mySet.has(1), 'has(value) returns true when set has value');
    assert.notOk(mySet.has(5), 'has(value) returns false for a nonexistent key');
    mySet.add({});
    assert.notOk(mySet.has({}), 'has(value) returns false for nonidentical keys');
    assert.end();
  });

  test(`test ${setType} delete method`, (t) => {
    const mySet = new Set();
    mySet.add(0);
    mySet.add(1);
    const startSize = mySet.size;

    t.ok(mySet.delete(1), 'deleting element in set returns true');
    t.notOk(mySet.has(1), 'deleted value no longer in set');
    t.equal(mySet.size, startSize - 1, 'deletion reduces size by 1');
    t.notOk(mySet.delete(5), 'deleting a nonexistent value returns false');
    t.equal(mySet.size, startSize - 1, 'deletion of a nonexistent value does not change size');

    t.end();
  });

  test(`test ${setType} clear method`, (t) => {
    const mySet = new Set();
    mySet.add(0);
    mySet.add(1);

    t.equal(mySet.size, 2, 'before clearing set has size > 0');
    t.doesNotThrow(() => {mySet.clear();}, 'nonempty set can be cleared');
    t.equal(mySet.size, 0, 'after clearing set has size == 0');
    const empty = new Set();
    t.doesNotThrow(() => {empty.clear();}, 'empty set can be cleared');
    t.equal(empty.size, 0, 'after clearing set has size == 0');
    t.end();
  });

  test(`test ${setType} iteration with for..of`, (t) => {
    const mySet = new Set();
    mySet.add(0);
    mySet.add(1);

    const elements = [];

    t.doesNotThrow(() => {
      for (let element of mySet) {
        elements.push(element);
      }
    }, 'can iterate over set with for...of');
    t.deepEquals(elements.sort(), [0,1].sort(), 'iteration with for..of includes all values (not preserving insertion order)');

    const keys = [];
    t.doesNotThrow(() => {
      for (let key of mySet.keys()) {
        keys.push(key);
      }
    }, 'can iterate over set.keys() with for..of');
    t.deepEquals(keys.sort(), [0,1].sort(), 'key iterator includes all keys (identical to values) (not preserving insertion order)');

    const values = [];
    t.doesNotThrow(() => {
      for (let value of mySet.values()) {
        values.push(value);
      }
    }, 'can iterate over set.values() with for..of');
    t.deepEquals(values.sort(), [0,1].sort(), 'value iterator includes all values (not preserving insertion order)');

    const entryKeys = [];
    const entryValues = [];
    t.doesNotThrow(() => {
      for (let [key, value] of mySet.entries()) {
        entryKeys.push(key);
        entryValues.push(value);
      }
    }, 'can iterate over set.entries() with for..of');
    t.deepEquals(entryKeys.sort(), [0,1].sort(), 'entries iterator includes all keys (not preserving insertion order)');
    t.deepEquals(entryValues.sort(), [0,1].sort(), 'entries iterator includes all values (not preserving insertion order)');
    t.deepEquals(entryKeys, entryValues, 'entry keys/values are identical');

    t.end();
  });

  test(`test ${setType} iteration with .forEach()`, (assert) => {
    const mySet = new Set();
    mySet.add(0);
    mySet.add(1);

    const values = [];
    assert.doesNotThrow(() => {
      mySet.forEach((value) => {
        values.push(value);
      });
    }, 'can iterate over set object with .forEach()');
    assert.deepEquals(values.sort(), [0,1].sort(), 'object iterator includes all values (not preserving insertion order)');

    assert.end();
  });

  test(`test ${setType} relation with Array objects`, (assert) => {
    const valArray = ['value1', 'value2'];
    let mySet;

    assert.doesNotThrow(() => {mySet = new Set(valArray);}, 'can create set from values');

    assert.doesNotThrow(() => {const spreadArray = [...mySet];}, 'can create Array from set using spread syntax');

    let setArray;
    assert.doesNotThrow(() => {setArray = Array.from(mySet);}, 'can create Array from set object');
    assert.deepEquals(setArray.sort(), valArray.sort(), 'array from set object has all values (not preserving insertion order)');

    let entryArray;
    assert.doesNotThrow(() => {entryArray = Array.from(mySet.entries());}, 'can create Array from set.entries()');
    assert.deepEquals(entryArray.sort(), [['value1','value1'], ['value2', 'value2']].sort(), 'array from set.entries() has all value-value pairs (not preserving insertion order)');

    let keyArray;
    assert.doesNotThrow(() => {keyArray = Array.from(mySet.keys());}, 'can create Array from set.keys()');
    assert.deepEquals(keyArray.sort(), ['value1', 'value2'].sort(), 'array from set.keys() has all values (not preserving insertion order)');

    let valueArray;
    assert.doesNotThrow(() => {valueArray = Array.from(mySet.values());}, 'can create Array from set.values()');
    assert.deepEquals(valueArray.sort(), ['value1', 'value2'].sort(), 'array from set.values() has all values (not preserving insertion order)');

    assert.end();
  });

  test(`test ${setType} readme code sample`, (assert) => {
      const Set = require('../index.js');
      let set;
      assert.doesNotThrow(() => {set = new Set();}, 'can construct new empty Set');

      assert.doesNotThrow(() => {set.add('raz').add('dwa').add({});}, 'can chain add calls to add strings and object');
      assert.equal(set.size, 3, 'after adding three elements set size is 3');
      assert.ok(set.has('raz'), 'set has added element');
      assert.notOk(set.has('foo'), 'set does not have nonexistent element');
      assert.equals(set.add('foo'), set, 'call to add returns the set object');
      assert.equals(set.size, 4, 'calling add increments set size by 1');
      assert.ok(set.has('foo'), 'set has existing element');
      assert.ok(set.has('dwa'), 'set has existing element');
      assert.ok(set.delete('dwa'), 'calling delete on existing element returns true');
      assert.equals(set.size, 3, 'calling delete on existing element decrements set size by 1');

      const elements = [];
      set.forEach(function (value) {
          elements.push(value);
      });
      assert.equals(elements.filter((v) => ['raz', '[object Object]', 'foo'].indexOf(v.toString()) === -1).length, 0, 'iteration with .forEach includes all values (not preserving insertion order)');

      const iterator = set.values();

      assert.doesNotThrow(() => {iterator.next();}, 'iterator returned by .values() has a next method');
      assert.equals(iterator.next().done, false, 'iterator returns object with done property set to false while not finished');
      assert.ok(set.has(iterator.next().value), 'iterator returns set element as the value property ');
      assert.ok(iterator.next().done && iterator.next().value === undefined, 'done is true and value is undefined when iteration has finished');

      assert.doesNotThrow(() => {set.clear();}, 'can clear set');
      assert.equal(set.size, 0, 'after clearing set size is 0');

      let setFromArray;
      assert.doesNotThrow(() => {setFromArray = new Set([1,2,3]);}, 'can create set from array');
      assert.ok(setFromArray.has(1), 'set from array has element of array');
      assert.deepEquals(Array.from(setFromArray).sort(), [1,2,3], 'can use Array.from() to create array from set (not preserving insertion order)');
      assert.deepEquals([...setFromArray].sort(), [1,2,3], 'can use spread syntax to create array from set (not preserving insertion order)');

      assert.end();
  });
});
