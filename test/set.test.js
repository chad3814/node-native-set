var test = require('tape');

[['builtIn-javascript', Set], ['node-native-set', require('../index.js')]].forEach(([setType, Set]) => {
  test(`test ${setType} constructor`, (t) => {
    t.doesNotThrow(() => {new Set()}, 'can consturct an empty new Set()');
    t.doesNotThrow(() => {new Set(null)}, 'can construct with null'); //
    t.doesNotThrow(() => {new Set([1,2,3]), 'can construct an iterable array of integers'});

    let stringy;
    t.doesNotThrow(() => {
      stringy = new Set('stringy');
    }, 'can construct set with string');
    t.equal(stringy.size, 7, 'string set size is as expected'); // note to self about possibly adding logging fxn

    let arrayOfArrays;
    t.doesNotThrow(() => {
      arrayOfArrays = new Set([[1,2], [3,4]]);
    }, 'can construct an array of arrays');
    t.equal(arrayOfArrays.size, 2, 'arrayOfArrays size is as expected');

    t.throws(() => {new Set(1,2,3)}, TypeError,'throws error if not an array'); // note to throw type error
    t.throws(() => {new Set(9)}, TypeError, 'throws error with non-iterable obj')
    t.throws(() => {new Set({1:2, 3:4})}, 'throws error on object constructor'); //

    t.end();
  })

  test(`test ${setType} add method`, (t) => {
    let mySet = new Set();
    const startSize = mySet.size;

    t.doesNotThrow(() => {mySet.add('a string')}, 'can add string to the set');
    t.equal(mySet.size, startSize+1, 'adding value increases size by 1');
    t.doesNotThrow(() => {mySet.add({})}, 'can add object to a set');
    t.doesNotThrow(() => {mySet.add(()=>{})}, 'can add function to a set');
    t.skip('diverging from builtin; cannot add null to a set');
    t.equal(mySet.add(1), mySet, "add returns the set");
    t.doesNotThrow(() => {mySet.add(1)}, 'can call add with only 1 argument');
    t.doesNotThrow(() => {mySet.add(2,3,4)}, 'can call add with more than 1 argument');
    t.ok(mySet.has(2) && !(mySet.has(3)), 'add ignores arguments after 1st arg');

    t.end();
   });

   test(`test ${setType} iteration with for..of`, (t) => {
     let mySet = new Set();
     mySet.add(0);
     mySet.add(1);

     let elements = [];

     t.doesNotThrow(() => {
       for (var element of mySet) {
         elements.push(element);
       }
     }, 'can iterate over set with for...of');
     t.deepEquals(elements.sort(), [0,1].sort(), 'iteration with for..of includes all values (not preserving insertion order)');

     let keys = [];
     t.doesNotThrow(() => {
       for (var key of mySet.keys()) {
         keys.push(key);
       }
     }, 'can iterate over set.keys() with for..of');
     t.deepEquals(keys.sort(), [0,1].sort(), 'key iterator includes all keys (identical to values) (not preserving insertion order)');

     let values = [];
     t.doesNotThrow(() => {
       for (var value of mySet.values()) {
         values.push(value);
       }
     }, 'can iterate over set.values() with for..of');
     t.deepEquals(values.sort(), [0,1].sort(), 'value iterator includes all values (not preserving insertion order)');

     let entryKeys = [];
     let entryValues = [];
     t.doesNotThrow(() => {
       for (var [key, value] of mySet.entries()) {
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
     var mySet = new Set();
     mySet.add(0);
     mySet.add(1);  

     let values = [];
     assert.doesNotThrow(() => {
       mySet.forEach(function(value) {
         values.push(value);
       });
     }, 'can iterate over set object with .forEach()');
     assert.deepEquals(values.sort(), [0,1].sort(), 'object iterator includes all values (not preserving insertion order)');

     assert.end();
   });

})
// ```
// > var s;
// undefined
// > s = new Set([1,2,3]);
// Set { 1, 2, 3 }
// > s = new Set({1:2, 3:4})
// TypeError: (var)[Symbol.iterator] is not a function
//     at new Set (native)
//     at repl:1:5
//     at sigintHandlersWrap (vm.js:22:35)
//     at sigintHandlersWrap (vm.js:73:12)
//     at ContextifyScript.Script.runInThisContext (vm.js:21:12)
//     at REPLServer.defaultEval (repl.js:340:29)
//     at bound (domain.js:280:14)
//     at REPLServer.runBound [as eval] (domain.js:293:12)
//     at REPLServer.<anonymous> (repl.js:538:10)
//     at emitOne (events.js:101:20)
// > s = new Set(null)
// Set {}
// > s = new Set('')
// Set {}
// > s = new Set('thing')
// Set { 't', 'h', 'i', 'n', 'g' }
// ```
//
//
// [11:00]
// ```
//
//
// [11:00]
//  ```> s = new Set([[1,2],[3,4]])
// Set { [ 1, 2 ], [ 3, 4 ] }```



// test('test set', function (t) {
//     var set = new NodeSet();
//     t.equal(set.size, 0, 'set size is 0');
//     set.add('raz').add('dwa').add({});
//     t.equal(set.size, 3, 'set size is 3');
//     set.add('foo');
//     t.ok(set.has('raz'), 'set has raz');
//     set.delete('dwa')
//     t.notOk(set.has('dwa'), 'dwa deleted')
//     t.notOk(set.has('andrea'), 'andrea was not here')
//     var iterator = set.values();
//     t.ok(iterator.next());
//     t.end();
// })
//
// test('test iterator', function(t) {
//   var set = new Set();
//   var iterator = set.values();
//   t.end();
// })
