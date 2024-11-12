# Result<T, E>

A class to handle functions that can fail.

## Functionality

This class has similar goal to [[Option]] but allows to return additional
information when the command fail, in particular it is possible to return an
error object instead of a void type.

By default the type `E` is an `ErrorManagement::ErrorType` but the programmer can choose
another error type if needed.

To avoid to many memory operations internally it uses a simple reference counting
to propagate the result information.

## Interface

### Constructors and distructor

1. `static Result Result::Succ(const T &val)` create a successful result with value `val`
2. `static Result Result::Fail(const E &e)` create a fail result with error payload `e`
3. `Result(const T &value)` successfull constructor
4. `Result(const Result &other)` copy constructor
5. `~Result()` dereferenciate the result and clean the memory if the counter reach 0

### Getters

1. `T val() const` retrieve the resulting value, or die if the result was a fail
2. `E err() const` retrieve the resulting error, or die if teh result was a succes 
3. `bool failed() const` check if the result was a fail
4. `bool succeded() const` check if the result was a success


### Operators

 1. `bool operator!() const` return true if failed, false otherwise
 2. `T operator+() const` return the value if succeded, die otherwise
 3. `E operator-() const` return the error if failed, die otherwise
 4. `Result & operator=(const Result &other)` assignement operator
