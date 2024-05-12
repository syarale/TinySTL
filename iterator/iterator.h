#ifndef ITERATOR_ITERATOR_H_
#define ITERATOR_ITERATOR_H_

#include <cstddef>

namespace sgi {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename IteratorCategory, typename T,
          typename Distance = std::ptrdiff_t, typename Pointer = T*,
          typename Reference = T&>
struct iterator {
  typedef IteratorCategory iterator_category;
  typedef T value_type;
  typedef Distance difference_type;
  typedef Pointer pointer;
  typedef Reference reference;
};

template <typename Iterator>
struct iterator_traits {
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::value_type value_type;
  typedef typename Iterator::difference_type difference_type;
  typedef typename Iterator::pointer pointer;
  typedef typename Iterator::reference reference;
};

template <typename T>
struct iterator_traits<T*> {
  typedef random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef std::ptrdiff_t difference_type;
  typedef T* pointer;
  typedef T& reference;
};

template <typename T>
struct iterator_traits<const T*> {
  typedef random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef std::ptrdiff_t difference_type;
  typedef const T* pointer;
  typedef const T& reference;
};

template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(
    const Iterator&) {
  typedef typename iterator_traits<Iterator>::value_type value_type;
  return static_cast<value_type*>(0);
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type* difference_type(
    const Iterator&) {
  typedef typename iterator_traits<Iterator>::difference_type difference_type;
  return static_cast<difference_type*>(0);
}

template <typename Iterator>
inline iterator_traits<Iterator> iterator_category(const Iterator&) {
  typedef typename iterator_traits<Iterator>::iterator_category category;
  return category();
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::differece_type distance_aux(
    const InputIterator& first, const InputIterator& last, input_iterator_tag) {
  typename iterator_traits<InputIterator>::difference_type count = 0;
  for (; first != last; first++) {
    ++count;
  }
  return count;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::differece_type
distance_aux(const RandomAccessIterator& first,
             const RandomAccessIterator& last, random_access_iterator_tag) {
  return last - first;
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type distance(
    const InputIterator& first, const InputIterator& last) {
  typedef typename iterator_traits<InputIterator>::iterator_category category;
  return distance_aux(first, last, category());
}

template <typename InputIterator, typename Distance>
inline void advance_aux(InputIterator& begin, Distance n, input_iterator_tag) {
  while (n-- > 0) {
    ++begin;
  }
}

template <typename BidirectionalIterator, typename Distance>
inline void advance_aux(BidirectionalIterator& begin, Distance n,
                        bidirectional_iterator_tag) {
  if (n > 0) {
    while (n-- > 0) {
      ++begin;
    }
  } else if (n < 0) {
    while (n++ < 0) {
      --begin;
    }
  }
}

template <typename RandomAccessIterator, typename Distance>
inline void advance_aux(RandomAccessIterator& begin, Distance n,
                        random_access_iterator_tag) {
  begin += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator& begin, Distance n) {
  typedef typename iterator_traits<InputIterator>::iterator_category category;
  advance_aux(begin, n, category());
}

}  // namespace sgi

#endif  // ITERATOR_ITERATOR_H_