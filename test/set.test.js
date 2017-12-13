var test = require('tape');

[['builtIn-javascript', Set], ['node-native-set', require('../index.js')]].forEach(([setType, Set]) => {
  test(`test ${setType} constructor`, (t) => {
    t.doesNotThrow(() => {new Set()}, 'can consturct an empty new Set()');
    t.doesNotThrow(() => {new Set(null)}, 'can construct with null'); //
    t.doesNotThrow(() => {new Set([1,2,3]), 'can construct an iterable array of integers'});
    t.throws(() => {new Set(1,2,3)}, TypeError,'throws error if not an array'); // note to throw type error
    t.throws(() => {new Set({1:2, 3:4})}, 'throws error on object constructor'); //
    // let stringy;
    // t.doesNotThrow(() => {
    //   stringy = new Set('stringy');
    // }, 'can construct set with string');
    // t.equal(stringy.size, 7, 'string set size is as expected'); // note to self about possibly adding logging fxn
    // let arrayOfArrays;
    // t.doesNotThrow(() => {
    //   arrayOfArrays = new Set([[1,2], [3,4]]);
    // }, 'can construct an array of arrays');
    // t.equal(arrayOfArrays.size, 2, 'arrayOfArrays size is as expected');
    t.end();
  })

  // test(`test ${setType} add method`, (assert) => {
  //   let mySet = new Set();
  //   const startSize = mySet.size;
  //
  //   assert.doesNotThrow(() => {mySet.add('a string')}, 'can add string to the set');
  //   assert.equal(mySet.size, startSize+1, 'adding value increases size by 1');
  //   assert.doesNotThrow(() => {mySet.add({}, 'value')}, 'can add object to a set');
  //   assert.doesNotThrow(() => {mySet.add(()=>{}, 'value')}, 'can add function to a set');
  //   assert.doesNotThrow(() => {mySet.add(null, 'value')}, 'can add null to a set'); //
  //   assert.doesNotThrow(() => {mySet.add(1)}, 'can call add with only 1 argument');
  //   assert.doesNotThrow(() => {mySet.add(2,3,4)}, 'can call add with more than 1 argument');
  //   assert.ok(mySet.has(2) && !(mySet.has(3)), 'add ignores arguments after 1st arg');
  //
  //   assert.end();
  //  });

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
