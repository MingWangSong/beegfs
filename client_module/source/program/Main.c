#include <app/App.h>
#include <common/toolkit/SocketTk.h>
#include <common/Common.h>
#include <fault-inject/fault-inject.h>
#include <filesystem/FhgfsOpsSuper.h>
#include <filesystem/FhgfsOpsInode.h>
#include <filesystem/FhgfsOpsPages.h>
#include <filesystem/FhgfsOpsFileNative.h>
#include <filesystem/ProcFs.h>
#include <components/worker/RWPagesWork.h>
#include <net/filesystem/FhgfsOpsCommKit.h>
#include <net/filesystem/FhgfsOpsRemoting.h>

#define BEEGFS_LICENSE "GPL v2"

/**
 * 客户端内核模块初始化入口点。
 * 当模块被加载时由内核调用，负责初始化所有必要的子系统，
 * 并注册BeeGFS文件系统到Linux内核VFS层。
 *
 * 初始化流程:
 * 1. 初始化故障注入调试子系统
 * 2. 初始化本地文件和网络通信相关资源
 * 3. 初始化必要的缓存和工作队列
 * 4. 注册文件系统类型到Linux内核
 * 5. 创建procfs条目，提供状态信息和配置接口
 *
 * @return 0表示成功，负值表示错误代码
 */
static int __init init_fhgfs_client(void)
{
#define fail_to(target, msg) \
   do { \
      printk_fhgfs(KERN_WARNING, msg "\n"); \
      goto target; \
   } while (0)

   // 初始化故障注入子系统，用于测试和调试
   if (!beegfs_fault_inject_init() )
      fail_to(fail_fault, "could not register fault-injection debugfs dentry");

   // 初始化本地文件系统支持
   if (!beegfs_native_init() )
      fail_to(fail_native, "could not allocate emergency pools");

   // 初始化通信工具包紧急池，用于关键通信操作
   if (!FhgfsOpsCommKit_initEmergencyPools() )
      fail_to(fail_commkitpools, "could not allocate emergency pools");

   // 初始化网络通信套接字工具
   if (!SocketTk_initOnce() )
      fail_to(fail_socket, "SocketTk initialization failed");

   // 初始化inode缓存，用于文件系统inode管理
   if (!FhgfsOps_initInodeCache() )
      fail_to(fail_inode, "Inode cache initialization failed");

   // 初始化页面工作队列，处理异步读写操作
   if (!RWPagesWork_initworkQueue() )
      fail_to(fail_rwpages, "Page work queue registration failed");

   // 初始化消息缓冲区缓存，优化网络通信
   if (!FhgfsOpsRemoting_initMsgBufCache() )
      fail_to(fail_msgbuf, "Message cache initialization failed");

   // 初始化页列表向量缓存，优化内存使用
   if (!FhgfsOpsPages_initPageListVecCache() )
      fail_to(fail_pagelists, "PageVec cache initialization failed");

   // 注册文件系统到Linux内核
   if (FhgfsOps_registerFilesystem() )
      fail_to(fail_register, "File system registration failed");

   // 创建procfs目录，提供状态信息和配置接口
   ProcFs_createGeneralDir();

   printk_fhgfs(KERN_INFO, "File system registered. Type: %s. Version: %s\n",
      BEEGFS_MODULE_NAME_STR, App_getVersionStr() );

   return 0;

fail_register:
   FhgfsOpsPages_destroyPageListVecCache();
fail_pagelists:
   FhgfsOpsRemoting_destroyMsgBufCache();
fail_msgbuf:
   RWPagesWork_destroyWorkQueue();
fail_rwpages:
   FhgfsOps_destroyInodeCache();
fail_inode:
   SocketTk_uninitOnce();
fail_socket:
   FhgfsOpsCommKit_releaseEmergencyPools();
fail_commkitpools:
   beegfs_native_release();
fail_native:
   beegfs_fault_inject_release();
fail_fault:
   return -EPERM;
}

/**
 * 客户端内核模块卸载函数。
 * 当模块被卸载时由内核调用，负责清理所有资源并注销文件系统。
 *
 * 清理流程：
 * 1. 移除procfs条目
 * 2. 注销文件系统
 * 3. 清理和释放各种缓存和资源
 */
static void __exit exit_fhgfs_client(void)
{
   ProcFs_removeGeneralDir();
   BUG_ON(FhgfsOps_unregisterFilesystem() );
   FhgfsOpsPages_destroyPageListVecCache();
   FhgfsOpsRemoting_destroyMsgBufCache();
   RWPagesWork_destroyWorkQueue();
   FhgfsOps_destroyInodeCache();
   SocketTk_uninitOnce();
   FhgfsOpsCommKit_releaseEmergencyPools();
   beegfs_native_release();
   beegfs_fault_inject_release();

   printk_fhgfs(KERN_INFO, "BeeGFS client unloaded.\n");
}

// 注册模块初始化和退出函数
module_init(init_fhgfs_client)
module_exit(exit_fhgfs_client)

// 模块元信息
MODULE_LICENSE(BEEGFS_LICENSE);
MODULE_DESCRIPTION("BeeGFS parallel file system client (https://www.beegfs.io)");
MODULE_AUTHOR("ThinkParQ GmbH");
MODULE_ALIAS("fs-" BEEGFS_MODULE_NAME_STR);
MODULE_VERSION(BEEGFS_VERSION);
