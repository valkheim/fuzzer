#pragma once

#define INSN_BREAK (0xcc)

long long unsigned get_value(pid_t child, long long unsigned address);
void set_breakpoint(long long unsigned bp_addr, long long unsigned original_value, pid_t child);
void unset_breakpoint(long long unsigned bp_addr, long long unsigned original_value, pid_t child);
void resume_execution(pid_t child);
struct user_regs_struct get_regs(pid_t pid);
void set_regs(pid_t child, struct user_regs_struct registers);
