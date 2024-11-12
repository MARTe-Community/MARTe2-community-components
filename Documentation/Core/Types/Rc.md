# Rc<T>

Smart pointer that uses reference counting to safely manage memory, similar to `ReferenceT<T>` but with support to
weak references to break reference cycles.

## Functionality

This smart pointer uses [reference counting](https://en.wikipedia.org/wiki/Reference_counting) to
safely manage the memory.

To handle [reference cylces](https://en.wikipedia.org/wiki/Reference_counting) it uses the **weak pointer**
concept.

The value is stored in a shared pointer: `T *pointer_`.

Additionally a `bool weak_` flag is used to know if the current is a weak or hard pointer as well as
a shared bool to inform weak references of a nullification of the pointer: `bool *isNull_`.

To handle the reference counting two `u32` shared variables are used:

- `u32 * refCounter_`: used to store the number of hard pointers referencing to the same address
- `u32 * weakCounter_`: used to store the number of weak pointers referencing to the same address

Below a simplified example of what a double linked list would look like using smart pointers:

![double linked list](https//www.plantuml.com/plantuml/png/hP71IiGm48RlynHp5dRP14yBoe8FKcexrUWqaNIo2kAxcvZG4BOemfxQcVc_BzFcj4Zl9SmCJ9EWEFJcwLbmD9u6CSu2kCUN-8fi5kcySuAVW3YuaT3JH9xiGF05Qyx6INVu2RqvOuKyO4e-k90Al4CL6-_KlxOTitGsCCTgwdc9ojQehY5iZFfVniElAnBJJ5RATvzx3fGi2RBq8UaWJsVS85H3QFqteHlYTek0UbcxtNsv4iYFsBcf-nxNFx2sAgAEsPoB1bINeAgWBc0TxVTbb4wCQqBEDKvzwTGLL0KyaHt3pDy0)

Note that dashed arrow represent _weak pointers_, solid arrow _hard pointers_, while diamond arrows composition.

In this simplified case once the code reference `list_root` is out of scope all the list will be
automatically deleted (without using the weak pointer instead would be impossible, as `node_0` will still have
a reference from `node_1`).

The basic idea is to use _hard pointers_ in parent-child relationship and _weak pointers_ in
child-parent relationship. However in complex graph this rule could not be enough, and you should be
careful when chosing wich type to use.

## Interface

### Constructors ad destructor

The following constructors are available:

1. `Rc()`: null pointer constructor
2. `Rc(const T &value)`: internally create a pointer and copy the value to the new memory
3. `Rc(T *pointer)`: take ownership of the pointer (**note** if you delete `pointer` you will brake everything)
4. `Rc(const Rc &other)`: copy constructor

The destructor `~Rc()` will call the private method `derefenciate()` that decrease the correct counter (`refCounter_` or `weakCounter_`)
and clean the memory when the counters reach 0.

In particular if `refCounter_` is `0` the internal memory (`pointer_`) and the reference counter (`refCounter_`) are both freed and the flag `isNull_` set to `true`.
If and when the `weakCounter_` is `0` the rest of the memory is freed (`isNull_` and `weakCounter_`).

### Information

Information about the state of the pointer can be accesses using these methods:

- `bool isNull()` return `true` if pointer is null
- `bool isWeak()` return `true` if pointer is a weak pointer
- `operator bool()` return `true` if pointer is not null
- `bool operator!()` return `true` if pointer is null
- `u32 referencesCount()` return number of hard refernces to the pointer
- `u32 weakReferencesCount()` return number of weak references to the pointer

### Accessing the value

To access to the value stored inside the pointer there are the following methods:

- `T &val()` return the value stored or die
- `T &operator*()` return the value stored or die
- `T *operator->()` access internal fields and methods of the value stored or die

### Changing the value

To change the value stored there are the following options:

- `Rc &operator=(const T val)` set the pointer to the new value (if null init it)
- `Rc &operator=(const Rc &other)` referneciate the other pointer
- `Rc &operator%=(const Rc &other)` referenciate the other pointer weakly

### Comparation

To compare the smart pointer to other values there are the following options:

- `bool same(const T*other)` check if internal pointer is equal to `other`
- `bool same(const Rc &other)` check if internal pointer is equal to `other` internal pointer
- `bool operator==(const T* other)` check if internal pointer is equal to `other`
- `bool operator!=(const T* other)` check if interanl pointer is different to `other`
- `bool operator==(const Rc &other)` check if internal pointer is equal to `other` internal pointer
- `bool operator!=(const Rc &other)` check if internal pointer is different to `other` internal pointer
- `bool operator==(const T &other)` check if stored value is eqaul to other
- `bool operator!=(const T &other)` check if stored value is different to other

### Other methods

- `Rc<T> operator+()` return an hard link to the pointer (or a null pointer if it is null)
- `Rc<T> operator~()` return a soft link to the pointer (or a null pointer if it is null)
- `void del()` force to deferenciate current pointer
