// $Id: xless.h,v 1.1 2016-02-17 02:16:28-08 - - $
// James Garbagnati
// jgarbagn
// 1354722

#ifndef __XLESS_H__
#define __XLESS_H__

//
// We assume that the type type_t has an operator< function.
//

template <typename Type>
struct xless {
   bool operator() (const Type& left, const Type& right) const {
      return left < right;
   }
};

#endif

