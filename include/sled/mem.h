/*
 * Copyright (c) 2013-2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#pragma once

#ifndef WIN32
#include <sys/mman.h>
#endif

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

namespace sled {

/*
 * Custom allocator supporting allocation of read-write-exec pages. Special care
 * should be taken when using this allocator.  The primary purpose is for just
 * in time compliation support.
 */
template <typename T>
class ExecPolicy {
 public:
  //    typedefs
  using value_type = T;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

 public:
  template <typename U>
  struct rebind {
    using other = ExecPolicy<U>;
  };

 public:
  //    memory allocation
  inline pointer allocate(
      size_type cnt, typename std::allocator<void>::const_pointer = nullptr) {
#ifdef WIN32
    return reinterpret_cast<T *>(VirtualAllocEx(GetCurrentProcess(), NULL,
                                                sizeof(T) * cnt, MEM_COMMIT,
                                                PAGE_EXECUTE_READWRITE));
#else
    return reinterpret_cast<T *>(mmap(nullptr, sizeof(T) * cnt,
                                      PROT_READ | PROT_WRITE | PROT_EXEC,
                                      MAP_ANON | MAP_PRIVATE, -1, 0));
#endif
  }
  inline void deallocate(pointer p, size_type cnt) {
#ifdef WIN32
    VirtualFreeEx(GetCurrentProcess(), p, sizeof(T) * cnt, MEM_DECOMMIT);
#else
    munmap(p, sizeof(T) * cnt);
#endif
  }

  //    size
  inline size_type max_size() const {
    return (std::numeric_limits<size_type>::max)() / sizeof(T);
  }

};  //    end of class ExecPolicy

//    determines if memory from another allocator
//    can be deallocated from this one
template <typename T>
inline bool operator==(ExecPolicy<T> const &, ExecPolicy<T> const &) {
  return true;
}
template <typename T, typename T2>
inline bool operator==(ExecPolicy<T> const &, ExecPolicy<T2> const &) {
  return false;
}
template <typename T, typename OtherAllocator>
inline bool operator==(ExecPolicy<T> const &, OtherAllocator const &) {
  return false;
}

#ifdef WIN32
using exec_buf_t = std::vector<uint8_t>;
#else
using exec_buf_t = std::vector<uint8_t, ExecPolicy<uint8_t> >;
#endif

}  // namespace sled
