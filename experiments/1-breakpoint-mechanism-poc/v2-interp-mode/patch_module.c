#include "shiva.h"
#include <signal.h>
#include <stdio.h>

static void
say(const char *s)
{
	puts(s);
	fflush(NULL);
}

static void
my_handler(int sig, siginfo_t *info, void *ucontext)
{
	(void)sig; (void)info; (void)ucontext;
	say("[patch_module] BREAKPOINT HIT via sigaction (no ptrace involved)");
	_exit(42);
}

void
shakti_main(void *arg)
{
	struct shiva_ctx *ctx = (struct shiva_ctx *)arg;
	shiva_error_t error;
	struct elf_symbol sym;
	uint64_t addr;

	say("[patch_module] shakti_main() running");

	if (elf_symbol_by_name(&ctx->elfobj, "target_func", &sym) == false) {
		say("[patch_module] could not find target_func symbol");
		return;
	}
	addr = sym.value + ctx->ulexec.base_vaddr;

	shiva_trace(ctx, 0, SHIVA_TRACE_OP_ATTACH, NULL, NULL, 0, &error);

	if (shiva_trace_register_handler(ctx, (void *(*)(void *))my_handler,
	    SHIVA_TRACE_BP_SIGILL, &error) == false) {
		say("[patch_module] shiva_trace_register_handler failed");
		return;
	}
	if (shiva_trace_set_breakpoint(ctx, (void *(*)(void *))my_handler,
	    addr, NULL, &error) == false) {
		say("[patch_module] shiva_trace_set_breakpoint failed");
		return;
	}
	say("[patch_module] breakpoint installed, returning to Shiva for real transfer");
	return;
}
