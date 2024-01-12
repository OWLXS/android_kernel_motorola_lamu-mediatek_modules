/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __CONNFEM_SKU_H__
#define __CONNFEM_SKU_H__

/*******************************************************************************
 *				M A C R O S
 ******************************************************************************/
#define DT_SKUS_HW_PROP_SIZE		4
#define DT_SKUS_HW_PROP_NAME_SIZE	64

#define CONNFEM_SKU_FEM_COUNT		8
#define CONNFEM_FEM_PIN_COUNT		8
#define CONNFEM_FEM_LOGIC_COUNT		32
#define CONNFEM_FEM_LOGIC_CAT_COUNT	8

#define CONNFEM_SKU_INVALID_IDX		0xFFFFFFFF

#define CONNFEM_SKU_LAYOUT_COUNT	16
#define CONNFEM_SKU_LAYOUT_PIN_COUNT	16

#define CONNFEM_SKU_LOG_SIZE		256

/*******************************************************************************
 *			    D A T A   T Y P E S
 ******************************************************************************/
enum cfm_array_type {
	CFM_ARRAY_TYPE_UINT,
	CFM_ARRAY_TYPE_CHAR,
	CFM_ARRAY_TYPE_NUM
};

/* FEM Basic Info */
struct connfem_sku_fem_info {
	unsigned short vid;
	unsigned short pid;
	unsigned int flag;
	char name[CONNFEM_PART_NAME_SIZE];
};

/* FEM Control PIN */
struct connfem_sku_fem_ctrlpin {
	unsigned int count;
	unsigned char id[CONNFEM_FEM_PIN_COUNT];
};

/* Logic Truth Table */
struct connfem_sku_fem_logic {
	unsigned int op;
	unsigned int binary;
};

struct connfem_sku_fem_truth_table {
	unsigned int logic_count;
	struct connfem_sku_fem_logic logic[CONNFEM_FEM_LOGIC_COUNT];
};

/* Truth Table Usage */
struct connfem_sku_fem_logic_cat {
	unsigned int id;
	unsigned int op_count;
	unsigned int op[CONNFEM_FEM_LOGIC_COUNT];
};

struct connfem_sku_fem_truth_table_usage {
	unsigned int cat_count;
	struct connfem_sku_fem_logic_cat cat[CONNFEM_FEM_LOGIC_CAT_COUNT];
};

/* Keep all sku information */
struct connfem_sku_fem {
	unsigned int magic_num;	/* CONNFEM_FEM_MAGIC_NUMBER */
	struct connfem_sku_fem_info info;
	struct connfem_sku_fem_ctrlpin ctrl_pin;
	struct connfem_sku_fem_truth_table tt;
	struct connfem_sku_fem_truth_table_usage tt_usage_wf;
	struct connfem_sku_fem_truth_table_usage tt_usage_bt; /* Reserved */
};

struct connfem_sku_pinmap {
	unsigned char pin1;	/* Antsel */
	unsigned char pin2;	/* FEM Control PIN, or MD BPI PIN for LAA 4x4 */
	unsigned char flag;	/* Polarity, or reserved for PIN mapping attribute */
};

struct connfem_sku_layout {
	unsigned int fem_idx;

	unsigned char bandpath[CONNFEM_SUBSYS_NUM];

	unsigned int pin_count;
	struct connfem_sku_pinmap pinmap[CONNFEM_SKU_LAYOUT_PIN_COUNT];
};

struct connfem_sku_spdt {
	unsigned int magic_num;	/* CONNFEM_SPDT_MAGIC_NUMBER */
	unsigned int pin_count;
	struct connfem_sku_pinmap pinmap[CONNFEM_SKU_LAYOUT_PIN_COUNT];
};

struct connfem_sku {
	unsigned int fem_count;
	struct connfem_sku_fem fem[CONNFEM_SKU_FEM_COUNT];

	unsigned int layout_flag;

	unsigned int layout_count;
	struct connfem_sku_layout layout[CONNFEM_SKU_LAYOUT_COUNT];

	struct connfem_sku_spdt spdt;
};

/*******************************************************************************
 *			    P U B L I C   D A T A
 ******************************************************************************/

/*******************************************************************************
 *			      F U N C T I O N S
 ******************************************************************************/

extern int cfm_sku_fem_info_populate(struct device_node *np,
		struct connfem_sku_fem_info *fem_info);

extern int cfm_sku_fem_ttbl_populate(struct device_node *np,
		struct connfem_sku_fem_truth_table *tt);

extern int cfm_sku_fem_ctrl_pin_populate(struct device_node *np,
		struct connfem_sku_fem_ctrlpin *ctrl_pin);

extern int cfm_sku_fem_ttbl_usg_populate(struct device_node *np,
		struct connfem_sku_fem_truth_table_usage *tt_usage);

extern int cfm_dt_prop_substr_np_fetch(struct device_node *np,
		const char *prop_name, const char *needle,
		struct device_node **matched_np);

extern int cfm_sku_fem_layout_populate(struct device_node *np,
		struct connfem_sku *sku,
		struct connfem_sku_layout *layout);

extern void cfm_skus_pininfo_dump(unsigned int pin_count,
		struct connfem_sku_pinmap *pinmap);

extern int cfm_sku_generic_layout_hdl(struct device_node *np,
		unsigned int *pin_cnt,
		struct connfem_sku_pinmap *pinmap);

extern int cfm_sku_prop_val_get(struct device_node *np,
		const char* propname,
		unsigned int *res_val);

extern int cfm_sku_ttbl_usg_hdl(struct device_node *np,
		struct connfem_sku *sku);

extern int cfm_sku_flags_config_get(void* ctx,
		struct cfm_epaelna_flags_config** flags_config);

extern int cfm_sku_available_get(void* ctx, bool *avail);

extern void cfm_sku_data_dump(struct connfem_sku *sku);

#endif /* __CONNFEM_SKU_H__ */
