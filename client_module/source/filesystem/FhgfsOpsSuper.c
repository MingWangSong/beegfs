#include <app/log/Logger.h>
#include <app/App.h>
#include <app/config/Config.h>
#include <filesystem/ProcFs.h>
#include <os/OsCompat.h>
#include <common/storage/StorageDefinitions.h>
#include <common/toolkit/MetadataTk.h>
#include <common/Common.h>
#include <components/worker/RWPagesWork.h>
#include <net/filesystem/FhgfsOpsRemoting.h>
#include "FhgfsOps_versions.h"
#include "FhgfsOpsSuper.h"
#include "FhgfsOpsInode.h"
#include "FhgfsOpsFile.h"
#include "FhgfsOpsDir.h"
#include "FhgfsOpsPages.h"
#include "FhgfsOpsExport.h"
#include "FhgfsXAttrHandlers.h"


static int  __FhgfsOps_initApp(struct super_block* sb, char* rawMountOptions);
static void __FhgfsOps_uninitApp(App* app);

static int  __FhgfsOps_constructFsInfo(struct super_block* sb, void* rawMountOptions);
static void __FhgfsOps_destructFsInfo(struct super_block* sb);


/* read-ahead size is limited by BEEGFS_DEFAULT_READAHEAD_PAGES, so this is the maximum already going
 * to to the server. 32MiB read-head also seems to be a good number. It still may be reduced by
 * setting /sys/class/bdi/fhgfs-${number}/read_ahead_kb */
#define BEEGFS_DEFAULT_READAHEAD_PAGES BEEGFS_MAX_PAGE_LIST_SIZE

/**
 * 文件系统类型结构体，用于注册BeeGFS文件系统到Linux内核。
 * 这是连接BeeGFS与Linux VFS层的主要入口点。
 */
static struct file_system_type fhgfs_fs_type =
{
   .name       = BEEGFS_MODULE_NAME_STR,  // 文件系统名称
   .owner      = THIS_MODULE,            // 所有者为当前模块
   .kill_sb    = FhgfsOps_killSB,        // 卸载文件系统时的清理函数
   //.fs_flags   = FS_BINARY_MOUNTDATA, // not required currently

#ifdef KERNEL_HAS_GET_SB_NODEV
   .get_sb     = FhgfsOps_getSB,         // 旧内核版本的挂载函数
#else
   .mount      = FhgfsOps_mount,         // 新内核版本的挂载函数
#endif
};

/**
 * 超级块操作结构体，定义了VFS与BeeGFS文件系统交互的关键回调函数。
 * 这些函数处理文件系统级别的操作，如获取文件系统统计信息、
 * 分配/销毁inode、卸载超级块等。
 */
static struct super_operations fhgfs_super_ops =
{
   .statfs        = FhgfsOps_statfs,        // 获取文件系统统计信息
   .alloc_inode   = FhgfsOps_alloc_inode,   // 分配新的inode
   .destroy_inode = FhgfsOps_destroy_inode, // 销毁inode
   .drop_inode    = generic_drop_inode,     // 减少inode引用计数
   .put_super     = FhgfsOps_putSuper,      // 卸载文件系统时清理超级块
   .show_options  = FhgfsOps_showOptions,   // 显示挂载选项
};

/**
 * 创建并初始化每个挂载点的应用程序对象。
 * 
 * 这个函数解析挂载选项，创建并初始化BeeGFS客户端应用程序实例，
 * 设置与服务器的连接，启动各种组件（如数据报监听器、同步器等）。
 *
 * @param sb 超级块结构体指针
 * @param rawMountOptions 原始挂载选项字符串
 * @return 0表示成功，非零表示错误代码
 */
int __FhgfsOps_initApp(struct super_block* sb, char* rawMountOptions)
{
   MountConfig* mountConfig;
   bool parseRes;
   App* app;
   int appRes;

   // 创建挂载配置对象并从挂载选项解析配置
   mountConfig = MountConfig_construct();

   parseRes = MountConfig_parseFromRawOptions(mountConfig, rawMountOptions);
   if(!parseRes)
   {
      MountConfig_destruct(mountConfig);
      return APPCODE_INVALID_CONFIG;
   }

   //printk_fhgfs(KERN_INFO, "Initializing App...\n"); // debug in

   // 获取应用程序对象并初始化
   app = FhgfsOps_getApp(sb);
   App_init(app, mountConfig);

   // 运行应用程序，建立与服务器的连接
   appRes = App_run(app);

   if(appRes != APPCODE_NO_ERROR)
   { // 发生错误时进行清理
      printk_fhgfs_debug(KERN_INFO, "Stopping App...\n");

      App_stop(app);

      printk_fhgfs_debug(KERN_INFO, "Cleaning up...\n");

      App_uninit(app);

      printk_fhgfs_debug(KERN_INFO, "App unitialized.\n");

      return appRes;
   }

   // 创建procfs条目，提供状态信息和配置接口
   ProcFs_createEntries(app);

   return appRes;
}

/**
 * 停止并销毁每个挂载点的应用程序对象。
 * 
 * 在文件系统卸载时调用，负责关闭与服务器的连接，
 * 停止所有组件，并释放资源。
 *
 * @param app 要停止和销毁的应用程序对象
 */
void __FhgfsOps_uninitApp(App* app)
{
   App_stop(app);

   /* note: some of the procfs entries (e.g. remove_node) won't work anymore after app components
      have been stopped, but others are still useful for finding reasons why app stop is delayed
      in some cases (so we remove procfs after App_stop() ). */

   ProcFs_removeEntries(app);

   App_uninit(app);
}

/**
 * 向Linux内核注册BeeGFS文件系统类型。
 * 
 * 在模块加载时调用，使Linux能够识别并挂载BeeGFS文件系统。
 *
 * @return 0表示成功，非零表示错误代码
 */
int FhgfsOps_registerFilesystem(void)
{
   return register_filesystem(&fhgfs_fs_type);
}

/**
 * 从Linux内核注销BeeGFS文件系统类型。
 *
 * 在模块卸载时调用，清理文件系统注册。
 *
 * @return 0表示成功，非零表示错误代码
 */
int FhgfsOps_unregisterFilesystem(void)
{
   return unregister_filesystem(&fhgfs_fs_type);
}

/**
 * 初始化超级块的文件系统信息。
 * 
 * 分配并填充BeeGFS特定的超级块信息结构体，
 * 初始化应用程序对象，设置BDI（backing device info）。
 *
 * @param sb 超级块结构体指针
 * @param rawMountOptions 原始挂载选项
 * @return 0表示成功，负值表示Linux错误代码
 */
int __FhgfsOps_constructFsInfo(struct super_block* sb, void* rawMountOptions)
{
   int res;
   int appRes;
   App* app;
   Logger* log;

#if defined(KERNEL_HAS_SB_BDI) && !defined(KERNEL_HAS_SUPER_SETUP_BDI_NAME)
   struct backing_dev_info* bdi;
#endif

   // 使用kzalloc分配并初始化超级块信息结构
   FhgfsSuperBlockInfo* sbInfo = kzalloc(sizeof(FhgfsSuperBlockInfo), GFP_KERNEL);
   if (!sbInfo)
   {
      printk_fhgfs_debug(KERN_INFO, "Failed to allocate memory for FhgfsSuperBlockInfo");
      sb->s_fs_info = NULL;
      return -ENOMEM;
   }

   sb->s_fs_info = sbInfo;

   // 初始化应用程序对象
   appRes = __FhgfsOps_initApp(sb, rawMountOptions);
   if(appRes)
   {
      printk_fhgfs_debug(KERN_INFO, "Failed to initialize App object");
      res = -EINVAL;
      goto outFreeSB;
   }

   app = FhgfsOps_getApp(sb);
   log = App_getLogger(app);
   IGNORE_UNUSED_VARIABLE(log);

#if defined(KERNEL_HAS_SB_BDI)
   // 设置backing device info (BDI)，处理页缓存和回写操作
   #if defined(KERNEL_HAS_SUPER_SETUP_BDI_NAME) && !defined(KERNEL_HAS_BDI_SETUP_AND_REGISTER)
   {
      static atomic_long_t bdi_seq = ATOMIC_LONG_INIT(0);

      res = super_setup_bdi_name(sb, BEEGFS_MODULE_NAME_STR "-%ld",
            atomic_long_inc_return(&bdi_seq));
   }
   #else
      bdi = &sbInfo->bdi;

      /* NOTE: The kernel expects a fully initialized bdi structure, so at a minimum it has to be
       *       allocated by kzalloc() or memset(bdi, 0, sizeof(*bdi)).
       *       we don't set the congest_* callbacks (like every other filesystem) because those are
       *       intended for dm and md.
       */
      bdi->ra_pages = BEEGFS_DEFAULT_READAHEAD_PAGES;

      #if defined(KERNEL_HAS_BDI_CAP_MAP_COPY) 
         res = bdi_setup_and_register(bdi, BEEGFS_MODULE_NAME_STR, BDI_CAP_MAP_COPY);
      #else
         res = bdi_setup_and_register(bdi, BEEGFS_MODULE_NAME_STR);
      #endif
   #endif

   if (res)
   {
      Logger_logFormatted(log, 2, __func__, "Failed to init super-block (bdi) information: %d",
         res);
      __FhgfsOps_uninitApp(app);
      goto outFreeSB;
   }
#endif

   // 初始化根目录inode信息的状态
   FhgfsOps_setHasRootEntryInfo(sb, false);
   FhgfsOps_setIsRootInited(sb, false);

   printk_fhgfs(KERN_INFO, "BeeGFS mount ready.\n");

   return 0; // 成功，res应该为0

outFreeSB:
   kfree(sbInfo);
   sb->s_fs_info = NULL;

   return res;
}

/**
 * 清理超级块的文件系统信息。
 * 
 * 在卸载文件系统时调用，销毁应用程序对象，
 * 释放BDI和其他资源。
 * 
 * @param sb 超级块结构体指针
 */
void __FhgfsOps_destructFsInfo(struct super_block* sb)
{
   /* sb->s_fs_info在__FhgfsOps_constructFsInfo()失败时可能为NULL */
   if (sb->s_fs_info)
   {
      App* app = FhgfsOps_getApp(sb);

//call destroy iff not initialised/registered by super_setup_bdi_name
#if defined(KERNEL_HAS_SB_BDI)

#if !defined(KERNEL_HAS_SUPER_SETUP_BDI_NAME) || defined(KERNEL_HAS_BDI_SETUP_AND_REGISTER)
      struct backing_dev_info* bdi = FhgfsOps_getBdi(sb);

      bdi_destroy(bdi);
#endif

#endif

      __FhgfsOps_uninitApp(app);

      kfree(sb->s_fs_info);
      sb->s_fs_info = NULL;

      printk_fhgfs(KERN_INFO, "BeeGFS unmounted.\n");
   }
}

/**
 * 填充文件系统超级块。
 * 
 * 当用户执行挂载命令时，VFS调用此函数初始化超级块，
 * 设置各种文件系统参数，创建根inode和dentry。
 * 这是文件系统挂载过程的核心。
 *
 * @param sb 超级块结构体指针
 * @param rawMountOptions 原始挂载选项
 * @param silent 是否静默操作（不打印错误信息）
 * @return 0表示成功，负值表示Linux错误代码
 */
int FhgfsOps_fillSuper(struct super_block* sb, void* rawMountOptions, int silent)
{
   App* app = NULL;
   Config* cfg = NULL;

   struct inode* rootInode;
   struct dentry* rootDentry;
   struct kstat kstat;
   EntryInfo entryInfo;

   FhgfsIsizeHints iSizeHints;

   // 初始化每个挂载点的文件系统信息和应用程序对象
   if(__FhgfsOps_constructFsInfo(sb, rawMountOptions) )
      return -ECANCELED;

   app = FhgfsOps_getApp(sb);
   cfg = App_getConfig(app);

   // 设置超级块参数
   sb->s_maxbytes = MAX_LFS_FILESIZE;   // 最大文件大小
   sb->s_blocksize = PAGE_SIZE;         // 块大小
   sb->s_blocksize_bits = PAGE_SHIFT;   // 块大小位数
   sb->s_magic = BEEGFS_MAGIC;          // 文件系统魔数
   sb->s_op = &fhgfs_super_ops;         // 超级块操作函数
   sb->s_time_gran = 1000000000;        // 时间戳粒度（纳秒）
#ifdef KERNEL_HAS_SB_NODIRATIME
   sb->s_flags |= SB_NODIRATIME;        // 不更新目录访问时间
#else
   sb->s_flags |= MS_NODIRATIME;
#endif

   // 根据配置设置扩展属性和ACL支持
   if (Config_getSysXAttrsEnabled(cfg) )
      sb->s_xattr = fhgfs_xattr_handlers_noacl; // 仅处理用户扩展属性

#ifdef KERNEL_HAS_GET_ACL
   if (Config_getSysACLsEnabled(cfg) )
   {
      sb->s_xattr = fhgfs_xattr_handlers; // 替换为支持ACL的扩展属性处理函数
#ifdef SB_POSIXACL
      sb->s_flags |= SB_POSIXACL;       // 启用POSIX ACL支持
#else
      sb->s_flags |= MS_POSIXACL;
#endif
   }
#endif // KERNEL_HAS_GET_ACL
   if (Config_getSysXAttrsCheckCapabilities(cfg) != CHECKCAPABILITIES_Always)
   #if defined(SB_NOSEC)
      sb->s_flags |= SB_NOSEC;          // 禁用写操作时的能力检查
   #else
      sb->s_flags |= MS_NOSEC;
   #endif

   /* MS_ACTIVE is rather important as it marks the super block being successfully initialized and
    * allows the vfs to keep important inodes in the cache. However, it seems it is already
    * initialized in vfs generic mount functions.
      sb->s_flags |= MS_ACTIVE; // used in iput_final()  */

   // NFS内核导出支持（2.6.29及更高版本）
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
   sb->s_export_op = &fhgfs_export_ops;
#endif

#if defined(KERNEL_HAS_SB_BDI)
   sb->s_bdi = FhgfsOps_getBdi(sb);
#endif

   // 初始化根inode
   memset(&kstat, 0, sizeof(struct kstat) );

   kstat.ino = BEEGFS_INODE_ROOT_INO;           // 根inode特殊编号
   kstat.mode = S_IFDIR | 0777;                 // 目录类型，所有人可访问
   kstat.atime = kstat.mtime = kstat.ctime = current_fs_time(sb);  // 设置时间戳
   kstat.uid = current_fsuid();                 // 当前用户ID
   kstat.gid = current_fsgid();                 // 当前组ID
   kstat.blksize = Config_getTuneInodeBlockSize(cfg);  // 块大小
   kstat.nlink = 1;                             // 链接数

   // 初始化根目录entryInfo（总是在需要时更新，这里只设置虚拟值）
   EntryInfo_init(&entryInfo, NodeOrGroup_fromGroup(0), StringTk_strDup(""), StringTk_strDup(""),
      StringTk_strDup(""), DirEntryType_DIRECTORY, 0);

   // 创建根inode
   rootInode = __FhgfsOps_newInode(sb, &kstat, 0, &entryInfo, &iSizeHints);
   if(!rootInode || IS_ERR(rootInode) )
   {
      __FhgfsOps_destructFsInfo(sb);
      return IS_ERR(rootInode) ? PTR_ERR(rootInode) : -ENOMEM;
   }

   // 创建根目录dentry
   rootDentry = d_make_root(rootInode);
   if(!rootDentry)
   {
      __FhgfsOps_destructFsInfo(sb);
      return -ENOMEM;
   }

#ifdef KERNEL_HAS_S_D_OP
   // Linux 2.6.38之后转向使用默认的超级块dentry操作
   /* note: Only set default dentry operations here, as we don't want those OPs set for the root
    * dentry. In fact, setting as before would only slow down everything a bit, due to
    * useless revalidation of our root dentry. */
   sb->s_d_op = &fhgfs_dentry_ops;
#endif // KERNEL_HAS_S_D_OP

   rootDentry->d_time = jiffies;   // 设置dentry时间戳
   sb->s_root = rootDentry;        // 设置超级块的根dentry

   return 0;
}

/**
 * 当执行FhgfsOps_killSB()->kill_anon_super()->generic_shutdown_super()时调用。
 * 
 * 卸载文件系统时的清理函数，销毁超级块信息。
 * 
 * @param sb 超级块结构体指针
 */
void FhgfsOps_putSuper(struct super_block* sb)
{
   if (sb->s_fs_info)
   {
      App* app = FhgfsOps_getApp(sb);

      if(app)
         __FhgfsOps_destructFsInfo(sb);
   }
}

/**
 * 文件系统卸载时的处理函数。
 * 
 * 禁用连接重试（加速卸载），刷新工作队列，
 * 注销BDI，然后调用通用的超级块销毁函数。
 * 
 * @param sb 超级块结构体指针
 */
void FhgfsOps_killSB(struct super_block* sb)
{
   App* app = FhgfsOps_getApp(sb);

   if (app) // 在挂载尝试失败时可能为NULL
      App_setConnRetriesEnabled(app, false); // 禁用连接重试，加速卸载

   RWPagesWork_flushWorkQueue();  // 刷新所有待处理的页面工作

#if defined(KERNEL_HAS_SB_BDI) && LINUX_VERSION_CODE < KERNEL_VERSION(4,1,0)
   /**
    * s_fs_info可能为NULL
    */
   if (likely(sb->s_fs_info) )
   {
      struct backing_dev_info* bdi = FhgfsOps_getBdi(sb);

      bdi_unregister(bdi);
   }
#endif

   kill_anon_super(sb);  // 调用通用的超级块销毁函数
}


/**
 * 显示挂载选项。
 * 
 * 当读取/proc/mounts时调用，向用户显示当前挂载选项。
 * 
 * @param sf seq_file结构体，用于输出
 * @param dentry 或 vfs 包含超级块的结构
 * @return 0表示成功
 */
#ifdef KERNEL_HAS_SHOW_OPTIONS_DENTRY
extern int FhgfsOps_showOptions(struct seq_file* sf, struct dentry* dentry)
{
   struct super_block* super = dentry->d_sb;
#else
extern int FhgfsOps_showOptions(struct seq_file* sf, struct vfsmount* vfs)
{
   struct super_block* super = vfs->mnt_sb;
#endif

   App* app = FhgfsOps_getApp(super);
   MountConfig* mountConfig = App_getMountConfig(app);

   MountConfig_showOptions(mountConfig, sf);

   return 0;
}

