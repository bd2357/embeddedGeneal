#ifndef _GENPOOLGENERATORS_H_
#define _GENPOOLGENERATORS_H_

/**
 * @brief define a pool object pointer and the associated pool object.
 * 
 * @details so if we use name fooPool of type MyChunk with 10 elements
 *  we generate the following objects and functions
 * 
 * typedef struct fooPool_objtype;
 * typedef struct fooPool_pool_t;
 * foolPool_t fooPool;
 * fooPool_t * const fooPool;  <-- Main handle
 * 
 * Status_t fooPool_return(fooPool_t *self);    // returns a pool item to the pool
 * MyChunk *fooPool_allocate(fooPool_t *self);  // allocates item from pool and returns its pointer
 * MyChunk *fooPool_allocate_with_callback(fooPool_t *self);  // allocates item from pool attaches action pointer, returns pointer to pool item
 * int fooPool_status(fooPool_t *self); // returns the number of items available from the pool
 * 
 */
#define DefineGenPool(name, type, len, onRel) \
enum {name##_len = (len)}; \
typedef struct name##_objtype { \
    PreBuf_t genBuf; \
    type poolObject[1]; /* the MyChunk object */ \
} name##_objtype; /* creates type fooPool_objtype */ \
typedef struct name##_pool_t { \
    GenPool_t poolHeader; /* the base class object */ \
    name##_objtype pool[name##_len]; /* the pool storage */ \
} name##_pool_t; /* fooPool_pool_t */ \
/* actually instantiate and initialize */   \
name##_pool_t name##_pool = (name##_pool_t){ \
    .pool = {[0].genBuf.guard=1}, /* zero the storage */ \
    .poolHeader.next=(PreBuf_t *)name##_pool.pool, /* set first */ \
    .poolHeader.end=(PreBuf_t *const)&name##_pool.pool[name##_len-1], /* assign const */ \
    .poolHeader.objectSize=sizeof(type), /* fill in derived info in base class */ \
    .poolHeader.cellSize = sizeof(name##_objtype), /* needed to find base from instance */ \
    .poolHeader.onRelease=onRel, \
}; \
Status_t name##_return(type *obj) {return GenPool_return(obj);} \
type * name##_allocate(name##_pool_t *self){return GenPool_allocate((GenPool_t *)self);} \
type * name##_allocate_with_callback(name##_pool_t *self, GenCallback_t cb, Context_t context) \
{return GenPool_allocate_with_callback((GenPool_t *)self, cb, context);} \
int name##_status(name##_pool_t const *self) \
{int t,a; Status_t s=GenPool_status((GenPool_t const *)self, &a, &t);  \
    if(Status_OK==s) return a; return 0; } \
name##_pool_t * const name = &name##_pool

/**
 * @brief Declares the type specific functions related to the
 *   Defined pool type.  Includes the necessary forward reference
 *   to the pool type.
 */
#define DeclareGenPool(name, type) \
struct name##_pool_t; \
Status_t name##_return(type *obj); \
type * name##_allocate(struct name##_pool_t *self); \
type * name##_allocate_with_callback(struct name##_pool_t *self, GenCallback_t cb, Context_t context); \
int name##_status(struct name##_pool_t const *self); \
extern struct name##_pool_t * const name;


/**
 * @brief Same as DefineGenPool except pool scope is static to allocation
 *   place (file or block)
 */
#define DefineStaticGenPool(name, type, len, onRel) \
enum {name##_len = (len)}; \
typedef struct name##_objtype { \
    PreBuf_t genBuf; \
    type poolObject[1]; /* the MyChunk object */ \
} name##_objtype; /* creates type fooPool_objtype */ \
typedef struct name##_pool_t { \
    GenPool_t poolHeader; /* the base class object */ \
    name##_objtype pool[name##_len]; /* the pool storage */ \
} name##_pool_t; /* fooPool_pool_t */ \
/* actually instantiate and initialize */   \
STATIC name##_pool_t name##_pool = (name##_pool_t){ \
    .pool = {[0].genBuf.guard=1}, /* zero the storage */ \
    .poolHeader.next=(PreBuf_t *)name##_pool.pool, /* set first */ \
    .poolHeader.end=(PreBuf_t *const)&name##_pool.pool[name##_len-1], /* assign const */ \
    .poolHeader.objectSize=sizeof(type), /* fill in derived info in base class */ \
    .poolHeader.cellSize = sizeof(name##_objtype), /* needed to find base from instance */ \
    .poolHeader.onRelease=onRel, \
}; \
STATIC Status_t name##_return(type *obj) {return GenPool_return(obj);} \
STATIC type * name##_allocate(name##_pool_t *self){return GenPool_allocate((GenPool_t *)self);} \
STATIC type * name##_allocate_with_callback(name##_pool_t *self, GenCallback_t cb, Context_t context) \
{return GenPool_allocate_with_callback((GenPool_t *)self, cb, context);} \
STATIC int name##_status(name##_pool_t const *self) \
{int t,a; Status_t s=GenPool_status((GenPool_t const *)self, &a, &t);  \
    if(Status_OK==s) return a; return 0; }  \
STATIC name##_pool_t * const name = &name##_pool


/**
 * @brief Further defines a set of wrapper type specific wrapper functions to
 *   further isolate the user from the implimntation. Or to isolate two pools of 
 *   the same type from getting mixed up.
 */
#define DefineGenPoolWrappers(name, type) \
Status_t name##Return(type *obj) {return GenPool_return(obj);} \
type * name##Allocate(void){return name##_allocate(name);} \
type * name##AllocateWithCallback(GenCallback_t cb, Context_t context) \
{return name##_allocate_with_callback(name, cb, context);} \
int name##Status(void) {return name##_status(name);}

/**
 * @brief Finally defines a set of declarations for the wrappers
 *  these are nice since they break dependency from implimtation
 *  and only depend on the object type served by the pool.

 */
#define DeclareGenPoolWrappers(name, type) \
Status_t name##Return(type *obj); \
type * name##Allocate(void); \
type * name##AllocateWithCallback(GenCallback_t cb, Context_t context); \
int name##Status(void);


#endif