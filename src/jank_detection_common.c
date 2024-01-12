// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 MediaTek Inc.
 */

#include <linux/tracepoint.h>
#include "jank_detection_common.h"

#define MAX_CALLBACKS 100
static tracepoint_fp tracepoint_callbacks[MAX_CALLBACKS] = {NULL};
static heavy_fp heavy_callbacks[MAX_CALLBACKS] = {NULL};
static struct kobject *jank_detection_kobject;
static struct tracepoint *ktp = NULL;
static unsigned int features = 0;

int register_heavy_callback(heavy_fp cb) {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (heavy_callbacks[i] == NULL) {
            heavy_callbacks[i] = cb;
            return 0;
        }
    }
    return -1;
}
EXPORT_SYMBOL(register_heavy_callback);

int unregister_heavy_callback(heavy_fp cb) {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (heavy_callbacks[i] == cb) {
            heavy_callbacks[i] = NULL;
            return 0;
        }
    }
    return -1;
}
EXPORT_SYMBOL(unregister_heavy_callback);

void receive_jank_detection(int jank, int pid) {
  for (int i = 0; i < MAX_CALLBACKS; i++) {
      if (heavy_callbacks[i] != NULL) {
          heavy_callbacks[i](jank, pid);
      }
  }
}
EXPORT_SYMBOL(receive_jank_detection);

int is_feature_enabled(unsigned int feature) {
    return (features & feature) != 0;
}
EXPORT_SYMBOL(is_feature_enabled);

int register_tracepoint_callback(tracepoint_fp cb) {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (tracepoint_callbacks[i] == NULL) {
            tracepoint_callbacks[i] = cb;
            return 0;
        }
    }
    return -1;
}
EXPORT_SYMBOL(register_tracepoint_callback);

int unregister_tracepoint_callback(tracepoint_fp cb) {
    for (int i = 0; i < MAX_CALLBACKS; i++) {
        if (tracepoint_callbacks[i] == cb) {
            tracepoint_callbacks[i] = NULL;
            return 0;
        }
    }
    return -1;
}
EXPORT_SYMBOL(unregister_tracepoint_callback);

static void tracepoint_callback_func(void *p, struct pt_regs *regs, long id) {

  for (int i = 0; i < MAX_CALLBACKS; i++) {
      if (tracepoint_callbacks[i] != NULL) {
          tracepoint_callbacks[i](p, regs->regs, id);
      }
  }
}

static void find_sys_enter_tracepoint(struct tracepoint *tp, void *priv) {
  if (strcmp(tp->name, "sys_enter") == 0) {
    ktp = tp;
  }
}

static int register_syscall(void) {
  int ret;

  for_each_kernel_tracepoint(find_sys_enter_tracepoint, NULL);

  if (!ktp) {
    return -ENODEV;
  }

  ret = tracepoint_probe_register(ktp, tracepoint_callback_func, NULL);
  if (ret) {
    return ret;
  }

  return 0;
}

static void unregister_syscall(void) {
  tracepoint_probe_unregister(ktp, tracepoint_callback_func, NULL);
}

static int jank_detection_enable(void) { return register_syscall(); }

static void jank_detection_disable(void) { unregister_syscall(); }

static ssize_t jank_detection_show(struct kobject *kobj,
                                    struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", features);
}

static ssize_t jank_detection_store(struct kobject *kobj,
                                     struct kobj_attribute *attr,
                                     const char *buf, size_t count) {
  int ret;
  int value;
  int enable;
  unsigned int feature_code;
  unsigned int prev_features;

  ret = kstrtoint(buf, 0, &value);
  if (ret < 0)
    return ret;

  prev_features = features;
  enable = (value & 0x10) >> 4;
  feature_code = value & 0x0F;

  if (enable) {
      switch (feature_code) {
          case FEATURE_FPSGO:
              features |= FEATURE_FPSGO;
              break;
          case FEATURE_SBE:
              features |= FEATURE_SBE;
              break;
      }
  } else {
      switch (feature_code) {
          case FEATURE_FPSGO:
              features &= ~FEATURE_FPSGO;
              break;
          case FEATURE_SBE:
              features &= ~FEATURE_SBE;
              break;
      }
  }


  if (!prev_features && ((features & FEATURE_FPSGO) || (features & FEATURE_SBE))) {
    ret = jank_detection_enable();
    if (ret < 0)
      return ret;
  } else if (prev_features && !features) {
    jank_detection_disable();
  }

  return count;
}

struct kobj_attribute jank_detection_attribute =
    __ATTR(jank_detection, 0664, jank_detection_show, jank_detection_store);

static int __init jank_detection_init(void) {
  int error = 0;

  jank_detection_kobject =
      kobject_create_and_add("jank_detection", kernel_kobj);
  if (!jank_detection_kobject)
    return -ENOMEM;

  error = sysfs_create_file(jank_detection_kobject,
                            &jank_detection_attribute.attr);

  //register_heavy_callback_fp = register_heavy_callback;
  //unregister_heavy_callback_fp = unregister_heavy_callback;

  return error;
}

static void __exit jank_detection_exit(void) {
  kobject_put(jank_detection_kobject);
}

module_init(jank_detection_init);
module_exit(jank_detection_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MediaTek jank Detection");
MODULE_AUTHOR("MediaTek Inc.");
