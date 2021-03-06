/*
 *    ELF32 Linux ABI VDSO for Atomik
 *    Copyright (C) 2014  Gonzalo J. Carracedo
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <atomik.h>
#include <linux.h>
#include <unistd_32.h>
#include <errno.h>

#include <syscall_list.h>

static struct syscall_desc syscall_list[SYSCALL_LIST_MAX] =
{
  [0] =  {"restart_syscall", NULL},
  [1] =  {"exit", sys_exit},
  [2] =  {"fork", NULL},
  [3] =  {"read", NULL},
  [4] =  {"write", sys_write},
  [5] =  {"open", sys_open},
  [6] =  {"close", NULL},
  [7] =  {"waitpid", NULL},
  [8] =  {"creat", NULL},
  [9] =  {"link", NULL},
  [10] = {"unlink", NULL},
  [11] = {"execve", NULL},
  [12] = {"chdir", NULL},
  [13] = {"time", NULL},
  [14] = {"mknod", NULL},
  [15] = {"chmod", NULL},
  [16] = {"lchown", NULL},
  [17] = {"break", NULL},
  [18] = {"oldstat", NULL},
  [19] = {"lseek", NULL},
  [20] = {"getpid", NULL},
  [21] = {"mount", NULL},
  [22] = {"umount", NULL},
  [23] = {"setuid", NULL},
  [24] = {"getuid", NULL},
  [25] = {"stime", NULL},
  [26] = {"ptrace", NULL},
  [27] = {"alarm", NULL},
  [28] = {"oldfstat", NULL},
  [29] = {"pause", NULL},
  [30] = {"utime", NULL},
  [31] = {"stty", NULL},
  [32] = {"gtty", NULL},
  [33] = {"access", NULL},
  [34] = {"nice", NULL},
  [35] = {"ftime", NULL},
  [36] = {"sync", NULL},
  [37] = {"kill", NULL},
  [38] = {"rename", NULL},
  [39] = {"mkdir", NULL},
  [40] = {"rmdir", NULL},
  [41] = {"dup", NULL},
  [42] = {"pipe", NULL},
  [43] = {"times", NULL},
  [44] = {"prof", NULL},
  [45] = {"brk", sys_brk},
  [46] = {"setgid", NULL},
  [47] = {"getgid", NULL},
  [48] = {"signal", NULL},
  [49] = {"geteuid", NULL},
  [50] = {"getegid", NULL},
  [51] = {"acct", NULL},
  [52] = {"umount2", NULL},
  [53] = {"lock", NULL},
  [54] = {"ioctl", NULL},
  [55] = {"fcntl", NULL},
  [56] = {"mpx", NULL},
  [57] = {"setpgid", NULL},
  [58] = {"ulimit", NULL},
  [59] = {"oldolduname", NULL},
  [60] = {"umask", NULL},
  [61] = {"chroot", NULL},
  [62] = {"ustat", NULL},
  [63] = {"dup2", NULL},
  [64] = {"getppid", NULL},
  [65] = {"getpgrp", NULL},
  [66] = {"setsid", NULL},
  [67] = {"sigaction", NULL},
  [68] = {"sgetmask", NULL},
  [69] = {"ssetmask", NULL},
  [70] = {"setreuid", NULL},
  [71] = {"setregid", NULL},
  [72] = {"sigsuspend", NULL},
  [73] = {"sigpending", NULL},
  [74] = {"sethostname", NULL},
  [75] = {"setrlimit", NULL},
  [76] = {"getrlimit", NULL},
  [77] = {"getrusage", NULL},
  [78] = {"gettimeofday", NULL},
  [79] = {"settimeofday", NULL},
  [80] = {"getgroups", NULL},
  [81] = {"setgroups", NULL},
  [82] = {"select", NULL},
  [83] = {"symlink", NULL},
  [84] = {"oldlstat", NULL},
  [85] = {"readlink", NULL},
  [86] = {"uselib", NULL},
  [87] = {"swapon", NULL},
  [88] = {"reboot", NULL},
  [89] = {"readdir", NULL},
  [90] = {"mmap", NULL},
  [91] = {"munmap", NULL},
  [92] = {"truncate", NULL},
  [93] = {"ftruncate", NULL},
  [94] = {"fchmod", NULL},
  [95] = {"fchown", NULL},
  [96] = {"getpriority", NULL},
  [97] = {"setpriority", NULL},
  [98] = {"profil", NULL},
  [99] = {"statfs", NULL},
  [100] = {"fstatfs", NULL},
  [101] = {"ioperm", NULL},
  [102] = {"socketcall", NULL},
  [103] = {"syslog", NULL},
  [104] = {"setitimer", NULL},
  [105] = {"getitimer", NULL},
  [106] = {"stat", NULL},
  [107] = {"lstat", NULL},
  [108] = {"fstat", NULL},
  [109] = {"olduname", NULL},
  [110] = {"iopl", NULL},
  [111] = {"vhangup", NULL},
  [112] = {"idle", NULL},
  [113] = {"vm86old", NULL},
  [114] = {"wait4", NULL},
  [115] = {"swapoff", NULL},
  [116] = {"sysinfo", NULL},
  [117] = {"ipc", NULL},
  [118] = {"fsync", NULL},
  [119] = {"sigreturn", NULL},
  [120] = {"clone", NULL},
  [121] = {"setdomainname", NULL},
  [122] = {"uname", sys_uname},
  [123] = {"modify_ldt", NULL},
  [124] = {"adjtimex", NULL},
  [125] = {"mprotect", NULL},
  [126] = {"sigprocmask", NULL},
  [127] = {"create_module", NULL},
  [128] = {"init_module", NULL},
  [129] = {"delete_module", NULL},
  [130] = {"get_kernel_syms", NULL},
  [131] = {"quotactl", NULL},
  [132] = {"getpgid", NULL},
  [133] = {"fchdir", NULL},
  [134] = {"bdflush", NULL},
  [135] = {"sysfs", NULL},
  [136] = {"personality", NULL},
  [137] = {"afs_syscall", NULL},
  [138] = {"setfsuid", NULL},
  [139] = {"setfsgid", NULL},
  [140] = {"_llseek", NULL},
  [141] = {"getdents", NULL},
  [142] = {"_newselect", NULL},
  [143] = {"flock", NULL},
  [144] = {"msync", NULL},
  [145] = {"readv", NULL},
  [146] = {"writev", sys_writev},
  [147] = {"getsid", NULL},
  [148] = {"fdatasync", NULL},
  [149] = {"_sysctl", NULL},
  [150] = {"mlock", NULL},
  [151] = {"munlock", NULL},
  [152] = {"mlockall", NULL},
  [153] = {"munlockall", NULL},
  [154] = {"sched_setparam", NULL},
  [155] = {"sched_getparam", NULL},
  [156] = {"sched_setscheduler", NULL},
  [157] = {"sched_getscheduler", NULL},
  [158] = {"sched_yield", NULL},
  [159] = {"sched_get_priority_max", NULL},
  [160] = {"sched_get_priority_min", NULL},
  [161] = {"sched_rr_get_interval", NULL},
  [162] = {"nanosleep", NULL},
  [163] = {"mremap", NULL},
  [164] = {"setresuid", NULL},
  [165] = {"getresuid", NULL},
  [166] = {"vm86", NULL},
  [167] = {"query_module", NULL},
  [168] = {"poll", NULL},
  [169] = {"nfsservctl", NULL},
  [170] = {"setresgid", NULL},
  [171] = {"getresgid", NULL},
  [172] = {"prctl", NULL},
  [173] = {"rt_sigreturn", NULL},
  [174] = {"rt_sigaction", NULL},
  [175] = {"rt_sigprocmask", NULL},
  [176] = {"rt_sigpending", NULL},
  [177] = {"rt_sigtimedwait", NULL},
  [178] = {"rt_sigqueueinfo", NULL},
  [179] = {"rt_sigsuspend", NULL},
  [180] = {"pread64", NULL},
  [181] = {"pwrite64", NULL},
  [182] = {"chown", NULL},
  [183] = {"getcwd", NULL},
  [184] = {"capget", NULL},
  [185] = {"capset", NULL},
  [186] = {"sigaltstack", NULL},
  [187] = {"sendfile", NULL},
  [188] = {"getpmsg", NULL},
  [189] = {"putpmsg", NULL},
  [190] = {"vfork", NULL},
  [191] = {"ugetrlimit", NULL},
  [192] = {"mmap2", NULL},
  [193] = {"truncate64", NULL},
  [194] = {"ftruncate64", NULL},
  [195] = {"stat64", NULL},
  [196] = {"lstat64", NULL},
  [197] = {"fstat64", NULL},
  [198] = {"lchown32", NULL},
  [199] = {"getuid32", NULL},
  [200] = {"getgid32", NULL},
  [201] = {"geteuid32", NULL},
  [202] = {"getegid32", NULL},
  [203] = {"setreuid32", NULL},
  [204] = {"setregid32", NULL},
  [205] = {"getgroups32", NULL},
  [206] = {"setgroups32", NULL},
  [207] = {"fchown32", NULL},
  [208] = {"setresuid32", NULL},
  [209] = {"getresuid32", NULL},
  [210] = {"setresgid32", NULL},
  [211] = {"getresgid32", NULL},
  [212] = {"chown32", NULL},
  [213] = {"setuid32", NULL},
  [214] = {"setgid32", NULL},
  [215] = {"setfsuid32", NULL},
  [216] = {"setfsgid32", NULL},
  [217] = {"pivot_root", NULL},
  [218] = {"mincore", NULL},
  [219] = {"madvise", NULL},
  [219] = {"madvise1", NULL},
  [220] = {"getdents64", NULL},
  [221] = {"fcntl64", NULL},
  [224] = {"gettid", NULL},
  [225] = {"readahead", NULL},
  [226] = {"setxattr", NULL},
  [227] = {"lsetxattr", NULL},
  [228] = {"fsetxattr", NULL},
  [229] = {"getxattr", NULL},
  [230] = {"lgetxattr", NULL},
  [231] = {"fgetxattr", NULL},
  [232] = {"listxattr", NULL},
  [233] = {"llistxattr", NULL},
  [234] = {"flistxattr", NULL},
  [235] = {"removexattr", NULL},
  [236] = {"lremovexattr", NULL},
  [237] = {"fremovexattr", NULL},
  [238] = {"tkill", NULL},
  [239] = {"sendfile64", NULL},
  [240] = {"futex", NULL},
  [241] = {"sched_setaffinity", NULL},
  [242] = {"sched_getaffinity", NULL},
  [243] = {"set_thread_area", sys_set_thread_area},
  [244] = {"get_thread_area", NULL},
  [245] = {"io_setup", NULL},
  [246] = {"io_destroy", NULL},
  [247] = {"io_getevents", NULL},
  [248] = {"io_submit", NULL},
  [249] = {"io_cancel", NULL},
  [250] = {"fadvise64", NULL},
  [252] = {"exit_group", sys_exit},
  [253] = {"lookup_dcookie", NULL},
  [254] = {"epoll_create", NULL},
  [255] = {"epoll_ctl", NULL},
  [256] = {"epoll_wait", NULL},
  [257] = {"remap_file_pages", NULL},
  [258] = {"set_tid_address", NULL},
  [259] = {"timer_create", NULL},
  [260] = {"timer_settime", NULL},
  [261] = {"timer_gettime", NULL},
  [262] = {"timer_getoverrun", NULL},
  [263] = {"timer_delete", NULL},
  [264] = {"clock_settime", NULL},
  [265] = {"clock_gettime", NULL},
  [266] = {"clock_getres", NULL},
  [267] = {"clock_nanosleep", NULL},
  [268] = {"statfs64", NULL},
  [269] = {"fstatfs64", NULL},
  [270] = {"tgkill", NULL},
  [271] = {"utimes", NULL},
  [272] = {"fadvise64_64", NULL},
  [273] = {"vserver", NULL},
  [274] = {"mbind", NULL},
  [275] = {"get_mempolicy", NULL},
  [276] = {"set_mempolicy", NULL},
  [277] = {"mq_open", NULL},
  [278] = {"mq_unlink", NULL},
  [279] = {"mq_timedsend", NULL},
  [280] = {"mq_timedreceive", NULL},
  [281] = {"mq_notify", NULL},
  [282] = {"mq_getsetattr", NULL},
  [283] = {"kexec_load", NULL},
  [284] = {"waitid", NULL},
  [286] = {"add_key", NULL},
  [287] = {"request_key", NULL},
  [288] = {"keyctl", NULL},
  [289] = {"ioprio_set", NULL},
  [290] = {"ioprio_get", NULL},
  [291] = {"inotify_init", NULL},
  [292] = {"inotify_add_watch", NULL},
  [293] = {"inotify_rm_watch", NULL},
  [294] = {"migrate_pages", NULL},
  [295] = {"openat", NULL},
  [296] = {"mkdirat", NULL},
  [297] = {"mknodat", NULL},
  [298] = {"fchownat", NULL},
  [299] = {"futimesat", NULL},
  [300] = {"fstatat64", NULL},
  [301] = {"unlinkat", NULL},
  [302] = {"renameat", NULL},
  [303] = {"linkat", NULL},
  [304] = {"symlinkat", NULL},
  [305] = {"readlinkat", NULL},
  [306] = {"fchmodat", NULL},
  [307] = {"faccessat", NULL},
  [308] = {"pselect6", NULL},
  [309] = {"ppoll", NULL},
  [310] = {"unshare", NULL},
  [311] = {"set_robust_list", NULL},
  [312] = {"get_robust_list", NULL},
  [313] = {"splice", NULL},
  [314] = {"sync_file_range", NULL},
  [315] = {"tee", NULL},
  [316] = {"vmsplice", NULL},
  [317] = {"move_pages", NULL},
  [318] = {"getcpu", NULL},
  [319] = {"epoll_pwait", NULL},
  [320] = {"utimensat", NULL},
  [321] = {"signalfd", NULL},
  [322] = {"timerfd_create", NULL},
  [323] = {"eventfd", NULL},
  [324] = {"fallocate", NULL},
  [325] = {"timerfd_settime", NULL},
  [326] = {"timerfd_gettime", NULL},
  [327] = {"signalfd4", NULL},
  [328] = {"eventfd2", NULL},
  [329] = {"epoll_create1", NULL},
  [330] = {"dup3", NULL},
  [331] = {"pipe2", NULL},
  [332] = {"inotify_init1", NULL},
  [333] = {"preadv", NULL},
  [334] = {"pwritev", NULL},
  [335] = {"rt_tgsigqueueinfo", NULL},
  [336] = {"perf_event_open", NULL},
  [337] = {"recvmmsg", NULL},
  [338] = {"fanotify_init", NULL},
  [339] = {"fanotify_mark", NULL},
  [340] = {"prlimit64", NULL},
  [341] = {"name_to_handle_at", NULL},
  [342] = {"open_by_handle_at", NULL},
  [343] = {"clock_adjtime", NULL},
  [344] = {"syncfs", NULL},
  [345] = {"sendmmsg", NULL},
  [346] = {"setns", NULL},
  [347] = {"process_vm_readv", NULL},
  [348] = {"process_vm_writev", NULL}
};

const struct syscall_desc *
syscall_get (unsigned int nr)
{
  if (nr >= SYSCALL_LIST_MAX || syscall_list[nr].sd_name == NULL)
    return NULL;

  return syscall_list + nr;
}

