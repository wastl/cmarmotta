//
// Created by wastl on 10.12.15.
//

#ifndef MARMOTTA_ITERATOR_H
#define MARMOTTA_ITERATOR_H

namespace marmotta {
namespace util {

/**
 * A common iterator class for iterators binding resources.
 */
template<typename T>
class CloseableIterator {
 public:

    /**
     * Close the iterator, freeing any wrapped resources
     */
    virtual ~CloseableIterator() {}

    /**
     * Increment iterator to next element.
    */
    virtual CloseableIterator<T>& operator++() = 0;

    /**
     * Dereference iterator, returning a reference to the current element.
     */
    virtual T& operator*() = 0;

    /**
     * Dereference iterator, returning a pointer to the current element.
     */
    virtual T* operator->() = 0;

    /**
     * Return true in case the iterator has more elements.
     */
    virtual bool hasNext() = 0;

};

}
}


#endif //MARMOTTA_ITERATOR_H
