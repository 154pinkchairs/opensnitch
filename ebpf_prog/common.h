#ifndef OPENSNITCH_COMMON_H
#define OPENSNITCH_COMMON_H

#include <linux/sched.h>
#include <linux/ptrace.h>
#include <uapi/linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h> 
#include <bpf/bpf_core_read.h> 

//https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/limits.h#L13
#ifndef MAX_PATH_LEN
 #define MAX_PATH_LEN  4096
#endif

//https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/binfmts.h#L16
#define MAX_CMDLINE_LEN 4096
// max args that I've been able to use before hitting the error:
// "dereference of modified ctx ptr disallowed"
#define MAX_ARGS 20
#define MAX_ARG_SIZE 256

// flags to indicate if we were able to read all the cmdline arguments,
// or if one of the arguments is >= MAX_ARG_SIZE, or there more than MAX_ARGS
#define COMPLETE_ARGS 0
#define INCOMPLETE_ARGS 1

#define MAPSIZE 12000

#ifndef TASK_COMM_LEN
 #define TASK_COMM_LEN 16
#endif

#define BUF_SIZE_MAP_NS 256
#define GLOBAL_MAP_NS "256"

//-------------------------------map definitions 
// which github.com/iovisor/gobpf/elf expects

typedef struct bpf_map_def {
	unsigned int type;
	unsigned int key_size;
	unsigned int value_size;
	unsigned int max_entries;
	unsigned int map_flags;
	unsigned int pinning;
	char namespace[BUF_SIZE_MAP_NS];
} bpf_map_def;

enum bpf_pin_type {
	PIN_NONE = 0,
	PIN_OBJECT_NS,
	PIN_GLOBAL_NS,
	PIN_CUSTOM_NS,
};
//-----------------------------------

// even though we only need 32 bits of pid, on x86_32 ebpf verifier complained when pid type was set to u32
typedef u64 pid_size_t;
typedef u64 uid_size_t; 

enum events_type {
    EVENT_NONE = 0,
    EVENT_EXEC,
    EVENT_EXECVEAT,
    EVENT_FORK,
    EVENT_SCHED_EXIT,
};

struct data_t {
    u64 type;
    u64 pid;  // PID as in the userspace term (i.e. task->tgid in kernel)
    u64 ppid; // Parent PID as in the userspace term (i.e task->real_parent->tgid in kernel)
    u64 uid;
    u64 args_count;
    u64 args_partial;
    char filename[MAX_PATH_LEN];
    char args[MAX_ARGS][MAX_ARG_SIZE];
    char comm[TASK_COMM_LEN];
}__attribute__((packed));

//-----------------------------------------------------------------------------
// maps

struct bpf_map_def SEC("maps/heapstore") heapstore = {
	.type = BPF_MAP_TYPE_PERCPU_ARRAY,
	.key_size = sizeof(u32),
	.value_size = sizeof(struct data_t),
	.max_entries = 1
};

#endif
