#ifndef PARSER_POINTER_H
#define PARSER_POINTER_H

#include <cassert>
#include <ostream>
#include <string>

/**
 * @class Pointer
 * 
 * A SmartPointer with reference counting.
 * 
 * @author Felipe Borges Alves
 */
template<class T>
class Pointer {
	public:
		/**
		 * Construct a Pointer pointing to NULL.
		 */
		inline Pointer() : ptr(NULL), references(NULL) {}
		
		/**
		 * Copy constructor.
		 * @param p Pointer that will be copied.
		 */
		inline Pointer(const Pointer<T> & p) {
			increment(p.getPtr(), p._getReferences());
		}
		
		/**
		 * Constructor for coping a pointer to a derived class of T.
		 * @param p Pointer that will be copied.
		 */
		template<class C>
		inline Pointer(const Pointer<C> & p) {
			increment(p.getPtr(), p._getReferences());
		}
		
		/**
		 * Construct a Pointer pointing to p.
		 * @param p Pointer that references will be controlled.
		 */
		inline Pointer(T *p) {
			increment(p);
		}
		
		/**
		 * This constructor is for internal use. Do not use it unless you know what you are doing.
		 * Construct a Pointer pointing to p.
		 * @param p Pointer that references will be controlled.
		 * @param ref The reference counter.
		 */
		inline Pointer(T *p, int *ref) {
			increment(p, ref);
		}
		
		inline ~Pointer() {
			decrement();
		}
		
		/**
		 * @return True if this object is derived of C.
		 */
		template<class C>
		inline bool instanceOf() const {
			C *c = dynamic_cast<C *>(this->ptr);
			return c;
		}
		
		/**
		 * Cast this Pointer to C.
		 * @return A Pointer to C.
		 */
		template<class C>
		inline Pointer<C> staticCast() {
			assert(instanceOf<C>());
			return Pointer<C>(static_cast<C *>(this->ptr), this->references);
		}
		
		/**
		 * Cast this Pointer to C.
		 * @return A Pointer to C.
		 */
		template<class C>
		inline const Pointer<C> staticCast() const {
			assert(instanceOf<C>());
			return Pointer<C>(static_cast<C *>(this->ptr), this->references);
		}
		
		/**
		 * Cast this Pointer to C, checking if this pointer is derived from C.
		 * If this pointer is not derived from C, this method return a NULL pointer.
		 * @return A Pointer to C.
		 */
		template<class C>
		inline Pointer<C> dynamicCast() {
			C *p = dynamic_cast<C *>(this->ptr);
			if (!p) return Pointer<C>();
			return Pointer<C>(p, this->references);
		}
		
		/**
		 * Cast this Pointer to C, checking if this pointer is derived from C.
		 * If this pointer is not derived from C, this method return a NULL pointer.
		 * @return A Pointer to C.
		 */
		template<class C>
		inline const Pointer<C> dynamicCast() const {
			C *p = dynamic_cast<C *>(this->ptr);
			if (!p) return Pointer<C>();
			return Pointer<C>(p, this->references);
		}
		
		/**
		 * Mark this pointer to not be deleted.
		 */
		inline void doNotDelete() {
			if (this->references) ++*this->references;
		}
		
		/**
		 * @return The pointer.
		 */
		inline T *getPtr() const {
			return this->ptr;
		}
		
		/**
		 * @return The number of references of this pointer.
		 */
		inline int getReferences() const {
			if (!this->references) return 0;
			return *this->references;
		}
		
		/**
		 * This method is for internal use. Do not use it unless you know what you are doing.
		 * @return A pointer to the the reference counter.
		 */
		inline int *_getReferences() const {
			return this->references;
		}
		
		inline Pointer<T> & operator=(const Pointer<T> & p) {
			if (this != &p) {
				decrement();
				increment(p.getPtr(), p._getReferences());
			}
			
			return *this;
		}
		
		template<class C>
		inline Pointer<T> & operator=(const Pointer<C> & p) {
			decrement();
			increment(p.getPtr(), p._getReferences());
			
			return *this;
		}
		
		template<class C>
		inline bool operator==(const Pointer<C> & p) const {
			return this->ptr == p.getPtr();
		}
		
		template<class C>
		inline bool operator!=(const Pointer<C> & p) const {
			return this->ptr != p.getPtr();
		}
		
		// useful for using Pointer with std::map
		template<class C>
		inline bool operator<(const Pointer<C> & p) const {
			return this->ptr < p.getPtr();
		}
		
		// useful for using Pointer with std::map
		template<class C>
		inline bool operator<=(const Pointer<C> & p) const {
			return this->ptr <= p.getPtr();
		}
		
		// useful for using Pointer with std::map
		template<class C>
		inline bool operator>(const Pointer<C> & p) const {
			return this->ptr > p.getPtr();
		}
		
		// useful for using Pointer with std::map
		template<class C>
		inline bool operator>=(const Pointer<C> & p) const {
			return this->ptr >= p.getPtr();
		}
		
		inline T *operator->() const {
			assert(this->ptr);
			return this->ptr;
		}
		
		inline T & operator*() {
			assert(this->ptr);
			return *this->ptr;
		}
		
		inline const T & operator*() const {
			assert(this->ptr);
			return *this->ptr;
		}
		
		inline operator T *() const {
			return this->ptr;
		}
		
		inline operator bool() const {
			return this->ptr;
		}
		
		friend std::ostream & operator<<(std::ostream & stream, const Pointer<T> & pointer) {
			return stream << pointer.ptr;
		}
		
	private:
		inline void increment(T *p, int *ref) {
			this->ptr = p;
			if (this->ptr) {
				this->references = ref;
				++*this->references;
			}
			else this->references = NULL;
		}
		
		inline void increment(T *p) {
			this->ptr = p;
			if (this->ptr) this->references = new int(1);
			else this->references = NULL;
		}
		
		inline void decrement() {
			if (this->references) {
				if (!--*this->references) {
					delete(this->ptr);
					delete(this->references);
				}
			}
		}
		
		T *ptr;
		int *references;
};

#endif
