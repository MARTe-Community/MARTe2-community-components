#ifndef STR_H__
#define STR_H__

#include "CompilerTypes.h"
#include "Option.h"

namespace MARTe {

/**
  @brief Variable size simple string.

        This class create a variable size string capable of automatically resize
when needed.
**/
class Str {
public:
  /**
    @brief Create an empty string
  **/
  Str();
  /**
    @brief Create a string with a value.
    @param str value used to the initalization
    **/
  Str(const char *str);
  /**
    @brief Copy constructor.
  **/
  Str(const Str &other);

  /**
    @brief Destructor
  **/
  ~Str();
  /**
    @brief Gets the string length.
    @return the string length contained in the first byte.
  **/
  uint32 len() const;
  /**
    @brief Converts the string in a standard c string
    @return pointer to the first byte.
  **/
  const char *cstr() const;
  /**
    @brief Clears the string by resetting the counter and the first byte.
  **/
  void clear();

  /**
    @brief Add operator with another Str
    @return the sum of the two strings
  **/
  Str operator+(const Str &other) const;
  /**
    @brief Add operator with a c style string
    @return the sum of the two strings
  **/
  Str operator+(const char *other) const;
  /**
    @brief Add operator with a char
    @return the sum of the string and char.
  **/
  Str operator+(const char &ch) const;
  /**
    @brief Assign operator
  **/
  Str &operator=(const Str &other);
  /**
    @brief equality operator with another Str
    @return true if same length and content.
  **/
  bool operator==(const Str &other) const;
  /**
    @brief equality operator with another Str
    @return true if same length and content.
  **/
  bool operator<(const Str &other) const;
  /**
    @brief equality operator with another Str
    @return true if same length and content.
  **/
  bool operator>(const Str &other) const;
  /**
    @brief equality operator with a c string
    @return true if same length and content.
  **/
  bool operator==(const char *other) const;
  /**
    @brief disequality operator with another Str
    @return true if different length or content.
  **/
  bool operator!=(const Str &other) const;
  /**
    @brief disequality operator with a c string
    @return true if different length or content.
  **/
  bool operator!=(const char *other) const;

  /**
    @brief append char to the end of the string.
    @return it self
  **/
  Str &operator+=(const char ch);

  /**
    @brief operator to access the i-th char
    @param i index of the char to get (if negative counted from the end)
    @return char at i-th position
  **/
  char operator[](int i) const;

  /**
    @brief set i-th char
    @param i index of the char to get (if negative counted from the end)
    @param character to set
    @return string itself
  **/
  Str &set(const int &i, const char &c);

  /**
    @brief shorten string at the specified index.
    @param end index of the string (if negative will be deduced from len)
    @return the shortened string if end index is valid (< len)
    @return empty string if end > len or if end is negative and > -len
  **/
  Str substr(const int32 end) const;
  /**
    @brief subdivide string with the given start and end indexes.
    @param start index
    @end index (if negative will be deduced from len)
    @return the sub-string if indexes are valid
    @return empty string if end > len or if end is negative and > -len
  **/
  Str substr(const int32 start, const int32 end) const;

  /**
    @brief find position of a sub-string
    @param str is the sub string to find
    @param start is the initial index where to start searching
    @return index if found
  **/
  Option<uint32> find(const Str &str, const uint32 &start = 0) const;
  Option<uint32> find(const char &ch, const uint32 &start = 0) const;

  uint32 hash() const;

private:
  void extend(uint32 lenght); /// extend
  char *mem_;   // + 1 for byte 0 containing size and +1 for 0 padding
  uint32 size_; // size of memory
  uint32 len_;  // length of string
};

}; // namespace MARTe

#endif
