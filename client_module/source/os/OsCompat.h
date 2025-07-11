/*
 * Compatibility functions for older Linux versions
 */

#ifndef OSCOMPAT_H_
#define OSCOMPAT_H_

#include <common/Common.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/compat.h>
#include <linux/list.h>
#include <linux/mount.h>
#include <linux/posix_acl_xattr.h>
#include <linux/swap.h>
#include <linux/writeback.h>

#include <linux/task_io_accounting_ops.h>

#include <linux/semaphore.h>


#ifndef KERNEL_HAS_MEMDUP_USER
   extern void *memdup_user(const void __user *src, size_t len);
#endif

#ifndef KERNEL_HAS_D_MAKE_ROOT
   extern struct dentry *d_make_root(struct inode *root_inode);
#endif

#if defined(KERNEL_HAS_SB_BDI) && !defined(KERNEL_HAS_BDI_SETUP_AND_REGISTER)
   extern int bdi_setup_and_register(struct backing_dev_info *bdi, char *name, unsigned int cap);
#endif

#ifndef KERNEL_HAS_HAVE_SUBMOUNTS
extern int have_submounts(struct dentry *parent);
#endif

/*
 * PG_error and SetPageError() have been deprecated and removed in Linux 6.12.
 * We now use mapping_set_error() to record writeback errors at the address_space level.
 *
 * This ensures compatibility with kernels >= 4.19 and aligns with the new writeback
 * error tracking model using errseq_t (see LWN: https://lwn.net/Articles/724307/).
 *
 * BeeGFS compatibility:
 * - Buffered mode paths already use filemap_fdatawait(), which calls filemap_check_errors().
 * - Native mode uses file_write_and_wait_range(), which calls file_check_and_advance_wb_err().
 */

/**
 * fhgfs_set_wb_error - Record a writeback error at the mapping level
 *
 * Replaces SetPageError(); safe across all supported kernels.
 *
 * @page: the page associated with the mapping
 * @err:  the error code
 */
static inline void fhgfs_set_wb_error(struct page *page, int err)
{
   if (page && page->mapping && err)
      mapping_set_error(page->mapping, err);
}

/**
 * generic_permission() compatibility function
 *
 * NOTE: Only kernels > 2.6.32 do have inode->i_op->check_acl, but as we do not
 *       support it anyway for now, we do not need a complete kernel version check for it.
 *       Also, in order to skip useless pointer references we just pass NULL here.
 */
static inline int os_generic_permission(struct inode *inode, int mask)
{
   #ifdef KERNEL_HAS_GENERIC_PERMISSION_2
      return generic_permission(inode, mask);
   #elif defined(KERNEL_HAS_GENERIC_PERMISSION_4)
      return generic_permission(inode, mask, 0, NULL);
   #elif defined(KERNEL_HAS_IDMAPPED_MOUNTS)
      return generic_permission(&nop_mnt_idmap, inode, mask);
   #elif defined(KERNEL_HAS_USER_NS_MOUNTS)
      return generic_permission(&init_user_ns, inode, mask);
   #else
      return generic_permission(inode, mask, NULL);
   #endif
}

#if defined(KERNEL_HAS_GENERIC_FILLATTR_REQUEST_MASK)
static inline void os_generic_fillattr(struct inode *inode, struct kstat *kstat, u32 request_mask)
#else
static inline void os_generic_fillattr(struct inode *inode, struct kstat *kstat)
#endif
{
   #if defined(KERNEL_HAS_IDMAPPED_MOUNTS)
   #if defined(KERNEL_HAS_GENERIC_FILLATTR_REQUEST_MASK)
      generic_fillattr(&nop_mnt_idmap, request_mask, inode, kstat);
   #else
      generic_fillattr(&nop_mnt_idmap, inode, kstat);
   #endif // KERNEL_HAS_GENERIC_FILLATTR_REQUEST_MASK
   #elif defined(KERNEL_HAS_USER_NS_MOUNTS)
      generic_fillattr(&init_user_ns, inode, kstat);
   #else
      generic_fillattr(inode, kstat);
   #endif
}

#ifdef KERNEL_HAS_SETATTR_PREPARE
static inline int os_setattr_prepare(struct dentry *dentry, struct iattr *attr)
{
   #if defined(KERNEL_HAS_IDMAPPED_MOUNTS)
      return setattr_prepare(&nop_mnt_idmap, dentry, attr);
   #elif defined(KERNEL_HAS_USER_NS_MOUNTS)
      return setattr_prepare(&init_user_ns, dentry, attr);
   #else
      return setattr_prepare(dentry, attr);
   #endif
}
#endif // KERNEL_HAS_SETATTR_PREPARE

static inline bool os_inode_owner_or_capable(const struct inode *inode)
{
   #if defined(KERNEL_HAS_IDMAPPED_MOUNTS)
      return inode_owner_or_capable(&nop_mnt_idmap, inode);
   #elif defined(KERNEL_HAS_USER_NS_MOUNTS)
      return inode_owner_or_capable(&init_user_ns, inode);
   #else
      return inode_owner_or_capable(inode);
   #endif
}

#ifndef KERNEL_HAS_D_MATERIALISE_UNIQUE
extern struct dentry* d_materialise_unique(struct dentry *dentry, struct inode *inode);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
/**
 * Taken from ext3 dir.c. is_compat_task() does work for all kernels, although it was already there.
 * So we are conservativ and only allow it for recent kernels.
 */
static inline int is_32bit_api(void)
{
#ifdef CONFIG_COMPAT
# ifdef in_compat_syscall
   return in_compat_syscall();
# else
   return is_compat_task();
# endif
#else
   return (BITS_PER_LONG == 32);
#endif
}
#else
static inline int is_32bit_api(void)
{
   return (BITS_PER_LONG == 32);
}
#endif // LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)

#ifndef KERNEL_HAS_I_UID_READ
static inline uid_t i_uid_read(const struct inode *inode)
{
   return inode->i_uid;
}

static inline gid_t i_gid_read(const struct inode *inode)
{
   return inode->i_gid;
}

static inline void i_uid_write(struct inode *inode, uid_t uid)
{
   inode->i_uid = uid;
}

static inline void i_gid_write(struct inode *inode, gid_t gid)
{
   inode->i_gid = gid;
}

#endif // KERNEL_HAS_I_UID_READ


#if defined(KERNEL_HAS_KMEMCACHE_CACHE_FLAGS_CTOR)
struct kmem_cache* OsCompat_initKmemCache(const char* cacheName, size_t cacheSize,
   void initFuncPtr(void* initObj, struct kmem_cache* cache, unsigned long flags) );
#elif defined(KERNEL_HAS_KMEMCACHE_CACHE_CTOR)
struct kmem_cache* OsCompat_initKmemCache(const char* cacheName, size_t cacheSize,
   void initFuncPtr(struct kmem_cache* cache, void* initObj) );
#else
struct kmem_cache* OsCompat_initKmemCache(const char* cacheName, size_t cacheSize,
   void initFuncPtr(void* initObj) );
#endif // LINUX_VERSION_CODE


// added to 3.13, backported to -stable
#ifndef list_next_entry
/**
 * list_next_entry - get the next element in list
 * @pos: the type * to cursor
 * @member: the name of the list_struct within the struct.
 */
#define list_next_entry(pos, member) \
   list_entry((pos)->member.next, typeof(*(pos)), member)
#endif


#ifndef list_first_entry
/**
 * list_first_entry - get the first element from a list
 * @ptr: the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
   list_entry((ptr)->next, type, member)
#endif // list_first_entry


static inline struct posix_acl* os_posix_acl_from_xattr(const void* value, size_t size)
{
#ifndef KERNEL_HAS_POSIX_ACL_XATTR_USERNS_ARG
   return posix_acl_from_xattr(value, size);
#else
   return posix_acl_from_xattr(&init_user_ns, value, size);
#endif
}

static inline int os_posix_acl_to_xattr(const struct posix_acl* acl, void* buffer, size_t size)
{
#ifndef KERNEL_HAS_POSIX_ACL_XATTR_USERNS_ARG
   return posix_acl_to_xattr(acl, buffer, size);
#else
   return posix_acl_to_xattr(&init_user_ns, acl, buffer, size);
#endif
}

#if defined(KERNEL_HAS_SET_ACL) || defined(KERNEL_HAS_SET_ACL_DENTRY)
static inline int os_posix_acl_chmod(struct dentry *dentry, umode_t mode)
{

#if defined(KERNEL_HAS_IDMAPPED_MOUNTS)
   return posix_acl_chmod(&nop_mnt_idmap, dentry, mode);

#elif defined(KERNEL_HAS_POSIX_ACL_CHMOD_NS_DENTRY)
   return posix_acl_chmod(&init_user_ns, dentry, mode);

#elif defined(KERNEL_HAS_USER_NS_MOUNTS)
   return posix_acl_chmod(&init_user_ns, dentry->d_inode, mode);

#else
   return posix_acl_chmod(dentry->d_inode, mode);
#endif
}
#endif // KERNEL_HAS_SET_ACL || KERNEL_HAS_SET_ACL_DENTRY

#ifndef KERNEL_HAS_PAGE_ENDIO
static inline void page_endio(struct page *page, int rw, int err)
{
   if (rw == READ)
   {
      if (!err)
      {
         SetPageUptodate(page);
      }
      else
      {
         ClearPageUptodate(page);
         fhgfs_set_wb_error(page, err);
      }

      unlock_page(page);
   }
   else
   { /* rw == WRITE */
      if (err)
      {
         fhgfs_set_wb_error(page, err);
      }

      end_page_writeback(page);
   }
}
#endif

#ifndef KERNEL_HAS_GENERIC_WRITE_CHECKS_ITER
# define os_generic_write_checks generic_write_checks
#else
extern int os_generic_write_checks(struct file* filp, loff_t* offset, size_t* size, int isblk);
#endif

#ifndef rb_entry_safe
#define rb_entry_safe(ptr, type, member) \
   ({ typeof(ptr) ____ptr = (ptr); \
      ____ptr ? rb_entry(____ptr, type, member) : NULL; \
   })
#endif

#ifndef rbtree_postorder_for_each_entry_safe
#define rbtree_postorder_for_each_entry_safe(pos, n, root, field) \
   for (pos = rb_entry_safe(rb_first_postorder(root), typeof(*pos), field); \
         pos && ({ n = rb_entry_safe(rb_next_postorder(&pos->field), \
         typeof(*pos), field); 1; }); \
      pos = n)

extern struct rb_node *rb_first_postorder(const struct rb_root *);
extern struct rb_node *rb_next_postorder(const struct rb_node *);
#endif

#ifndef KERNEL_HAS_CURRENT_UMASK
#define current_umask() (current->fs->umask)
#endif

#ifndef XATTR_NAME_POSIX_ACL_ACCESS
# define XATTR_POSIX_ACL_ACCESS  "posix_acl_access"
# define XATTR_NAME_POSIX_ACL_ACCESS XATTR_SYSTEM_PREFIX XATTR_POSIX_ACL_ACCESS
# define XATTR_POSIX_ACL_DEFAULT  "posix_acl_default"
# define XATTR_NAME_POSIX_ACL_DEFAULT XATTR_SYSTEM_PREFIX XATTR_POSIX_ACL_DEFAULT
#endif

#ifndef KERNEL_HAS_I_MMAP_LOCK
static inline void i_mmap_lock_read(struct address_space* mapping)
{
#if defined(KERNEL_HAS_I_MMAP_RWSEM)
   down_read(&mapping->i_mmap_rwsem);
#elif defined(KERNEL_HAS_I_MMAP_MUTEX)
   mutex_lock(&mapping->i_mmap_mutex);
#else
   spin_lock(&mapping->i_mmap_lock);
#endif
}

static inline void i_mmap_unlock_read(struct address_space* mapping)
{
#if defined(KERNEL_HAS_I_MMAP_RWSEM)
   up_read(&mapping->i_mmap_rwsem);
#elif defined(KERNEL_HAS_I_MMAP_MUTEX)
   mutex_unlock(&mapping->i_mmap_mutex);
#else
   spin_unlock(&mapping->i_mmap_lock);
#endif
}
#endif

static inline bool beegfs_hasMappings(struct inode* inode)
{
#if defined(KERNEL_HAS_I_MMAP_RBTREE)
   if (!RB_EMPTY_ROOT(&inode->i_mapping->i_mmap))
      return true;
#elif defined(KERNEL_HAS_I_MMAP_CACHED_RBTREE)
   if (!RB_EMPTY_ROOT(&inode->i_mapping->i_mmap.rb_root))
      return true;
#else
   if (!prio_tree_empty(&inode->i_mapping->i_mmap))
      return true;
#endif

#ifdef KERNEL_HAS_I_MMAP_NONLINEAR
   if (!list_empty(&inode->i_mapping->i_mmap_nonlinear))
      return true;
#endif

   return false;
}

#ifndef KERNEL_HAS_INODE_LOCK
static inline void os_inode_lock(struct inode* inode)
{
   mutex_lock(&inode->i_mutex);
}

static inline void os_inode_unlock(struct inode* inode)
{
   mutex_unlock(&inode->i_mutex);
}
#else
static inline void os_inode_lock(struct inode* inode)
{
   inode_lock(inode);
}

static inline void os_inode_unlock(struct inode* inode)
{
   inode_unlock(inode);
}
#endif


#if defined(KERNEL_ACCESS_OK_WANTS_TYPE)
#  define os_access_ok(type, addr, size) access_ok(type, addr, size)
#else
#  define os_access_ok(type, addr, size) access_ok(addr, size)
#endif


#endif /* OSCOMPAT_H_ */
