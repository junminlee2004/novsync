/*
 * novsync - Automatic VSync Disable Plugin for PS Vita
 *
 * Mirrors VGi's "DROP ALL" killswitch approach exactly:
 * hooks all Wait functions and returns 0 to skip vsync waits.
 * Does NOT touch sceDisplaySetFrameBuf sync parameter.
 *
 * NIDs taken directly from VGi 0.7 source (Electry).
 */

#include <psp2/kernel/processmgr.h>
#include <taihen.h>

#define MAX_HOOKS 8
static SceUID g_hooks[MAX_HOOKS] = {-1,-1,-1,-1,-1,-1,-1,-1};
static tai_hook_ref_t g_refs[MAX_HOOKS];

static int hook_WaitVblankStart(void) { return 0; }
static int hook_WaitVblankStartCB(void) { return 0; }
static int hook_WaitVblankStartMulti(unsigned int v) { (void)v; return 0; }
static int hook_WaitVblankStartMultiCB(unsigned int v) { (void)v; return 0; }
static int hook_WaitSetFrameBuf(void) { return 0; }
static int hook_WaitSetFrameBufCB(void) { return 0; }
static int hook_WaitSetFrameBufMulti(unsigned int v) { (void)v; return 0; }
static int hook_WaitSetFrameBufMultiCB(unsigned int v) { (void)v; return 0; }

typedef struct {
    unsigned int nid;
    void *func;
} HookEntry;

static const HookEntry entries[MAX_HOOKS] = {
    { 0x5795E898, hook_WaitVblankStart },
    { 0x78B41B92, hook_WaitVblankStartCB },
    { 0xDD0A13B8, hook_WaitVblankStartMulti },
    { 0x05F27764, hook_WaitVblankStartMultiCB },
    { 0x9423560C, hook_WaitSetFrameBuf },
    { 0x814C90AF, hook_WaitSetFrameBufCB },
    { 0x7D9864A8, hook_WaitSetFrameBufMulti },
    { 0x3E796EF5, hook_WaitSetFrameBufMultiCB },
};

void _start() __attribute__((weak, alias("module_start")));

int module_start(SceSize args, void *argp) {
    for (int i = 0; i < MAX_HOOKS; i++) {
        g_hooks[i] = taiHookFunctionImport(
            &g_refs[i],
            TAI_MAIN_MODULE,
            TAI_ANY_LIBRARY,
            entries[i].nid,
            entries[i].func
        );
    }
    return 0;
}

int module_stop(SceSize args, void *argp) {
    for (int i = 0; i < MAX_HOOKS; i++) {
        if (g_hooks[i] >= 0)
            taiHookRelease(g_hooks[i], g_refs[i]);
    }
    return 0;
}
