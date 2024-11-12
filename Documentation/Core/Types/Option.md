# Option<T>

Type representing optional types, **to use** instead of returning null pointers.

## Functionality

This class can contain a value of type `T` or nothing, this allow to avoid
using null pointers when is impossible to return a value.

Internally it store the optional value in the internal field `T* mem`.

## Interface

### Constructors and distructor

The following constructors are available:

 1. `Option()`: create an empty optional value
 2. `Option(const T &value)`: create a non empty optional value with the specified value

The distructor `~Option()` will take care of destroying the internal memory buffer.

### Information

To access to information about the state of the object there are the following methods:

 1. `bool empty()`: to check if the optional value is empty (`true`) or not (`false`)
 2. `operator bool()`: cast the object to a bool value that will be `true` if the the object is not empty
 3. `bool operator!()`: operator that return the empty state

### Value access

To access the value stored (maybe) inside the object there are the following methods:

 1. `T val()`: if a value is stored returne the value otherwise die
 2. `T operator~()`: shortcut for method `val()`

### Other operators

A cuple of additional operators are implemented:

 1. `Option &operator=(const T &value)`: to re-assign the value of an optional object
 2. `Option &operator=(const Option &other)`: to copy another optional

### Helpers

Simple helpers allow to transform functions that does not use optionals in ones
that use it:

 1. `Option<T> maybe(T (*fn)(T), const Option<T> &x)` that basically
 transfrom a `T fn(T)` into an `Option<T> fn(Option<T>)`
 2. `Option<T> maybe(T* (*fn)(T), const Option<T> &x)` that basically
 transfrom a `T* fn(T)` into an `Option<T> fn(Option<T>)`


