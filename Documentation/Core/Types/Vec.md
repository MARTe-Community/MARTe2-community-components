# Vec<T>

The templated class `Vec<T>` defined in the [`array.h`](../include/array.h) header file is a dynamically sized
array.

## Functionality

The `Vec<T>` class has the following UML representation:

![uml of Vec<T>](//www.plantuml.com/plantuml/png/bP7TIWCn48Nl-nJZJOdj5lm1hxgbuWC8ahjIIhgPnK1-ocHIgEAxsuIIhbX5htRF-SxaCCaoiY9EpewUh8eH7edKOIuNy3w26KYOW2BQvFzqVmVHl64LszJt9sDQvyIGjBSFrrzW5tpaAc5yYKfGsLYj6oX6BIxXVJ0QbDP2dcL4_jFiDWGBX2xiKPWZCCLzth49NjdvCRk0tdWj9A15XvxhM3wEbV7np9foU4_1GGit9Vc2CLdEcoaUD4WOXnsIua2mMlyCKWMbigAs_NPnWL-HBaIlVejMhxR-Bprjo_QPbjqjUb65GxV9DotExqAeKpSqEekAifv1PCMcEzbXTn1o0eInWUCtKjSCNkvZj4IlaxEV)

Internally it works with a buffer, `T* arr_`, that is dinammically sized to contains the
number of elements.

The field `uint32 size_` store the current number of elements, while `uint32 buffsize_` store the actual size of `arr_`.

The buffer `arr_` once full is incremented of a constant `step` of `16` elements.

## Interface

### Constructors

The class has 3 constructors:

1. The empty constructor, `Vec<T>(uint32 init_size = step)`, that initialize an empty buffer of size `init_size`
2. The copy constructor, `Vec<T>(const Vec<T> &other)`, that copy the content of another array
3. The `C` array constructor, `Vec<T>(const T array[], const uint32 size)`, that convert a `C` array into an `Vec<T>`

In the third case the internal buffer will be initialised to a size of `size + step` in order to be
ready to host new values.

### Destructor

The destructro simply clean the internal buffer.

### Information methods

To access basic information about the array there are the following methods:

- `uint32 len()` to get the number of elements inside the array
- `uint32 mem_size()` to get the actual size of the internal buffer

### Append methods

To add new elements in the array there are the following methods:

- `void add(T item)`
- `Vec<T> &operator +=(T item)`

Both methods add an element to the end of the array.

### Set methods

To complitly set an array there are the following methods:

- `void set(const T* arr, const uint32 size)` to set the array using a `C` array
- `Vec<T> &operator=(const Vec<T> &other)` to set the array using another `Vec<T>`

### Remove and modify methods

To remove items inside the array there are the following methods:

- `bool remove(int32 i)` to remove the element at the index `i`, it returns false if out of bound
- `void clear()` to remove all elements (by resetting the `size_` field)

When index `i` is negative the index is counted from the end.

### Access methods

To access items stored in the array there are the following methods:

- `Result<T> at(int32 i)` return the value at the index `i` or an error message if index is out of bound
- `T &operator [](int32 i)` return the value at the index `i` or die
- `T &operator [](uint32 i)` return the value at the index `i` or die

The first two methods that have a signed integer index allows negative index, with negative indexes converted
as `len() + index` (so `-1` being the last element).

### Research methods

To find an item or to check if it exists inside an array there are the follwoing methods:

- `Option<uint32> find(T item, uint32 from=0)` to find the index of an `item`, with the possibility of specify the start search index
- `bool contains(T item, uint32 from=0)` to check if array contain an `item`, with the possibility of specify the start search index

### Other methods

Additional methods and operators:

- `bool operator ==(const Vec<T> &other)` equality operator
- `bool operator !=(const Vec<T> &other)` disequality operator
- `void reduce()` to reduce the size of the buffer to the currently needed size
