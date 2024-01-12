/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

typedef void (*tracepoint_fp)(void *p, unsigned long long *regs, long id);
typedef void (*heavy_fp)(int jank, int pid);
int register_tracepoint_callback(tracepoint_fp cb);
int unregister_tracepoint_callback(tracepoint_fp cb);
int register_heavy_callback(heavy_fp cb);
int unregister_heavy_callback(heavy_fp cb);
int is_feature_enabled(unsigned int feature);

enum feature_flags {
    FEATURE_FPSGO = 1 << 0,
    FEATURE_SBE   = 1 << 1
};
