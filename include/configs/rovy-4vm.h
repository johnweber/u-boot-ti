/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for K3 J721E EVM
 *
 * Copyright (C) 2018-2020 Texas Instruments Incorporated - https://www.ti.com/
 *	Lokesh Vutla <lokeshvutla@ti.com>
 */

#ifndef __CONFIG_ROVY_4VM_H
#define __CONFIG_ROVY_4VM_H

#include <linux/sizes.h>
#include <config_distro_bootcmd.h>
#include <environment/ti/mmc.h>
#include <environment/ti/k3_rproc.h>
#include <environment/ti/ufs.h>
#include <environment/ti/k3_dfu.h>

/* DDR Configuration */
#define CONFIG_SYS_SDRAM_BASE1		0x880000000
/* FLASH Configuration */
#define CONFIG_SYS_FLASH_BASE		0x000000000

/* SPL Loader Configuration */
#if defined(CONFIG_TARGET_ROVY_4VM_A72)
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SPL_TEXT_BASE + SZ_4M)
/* Image load address in RAM for DFU boot*/
#else
/*
 * Maximum size in memory allocated to the SPL BSS. Keep it as tight as
 * possible (to allow the build to go through), as this directly affects
 * our memory footprint. The less we use for BSS the more we have available
 * for everything else.
 */
#define CONFIG_SPL_BSS_MAX_SIZE		0xA000
/*
 * Link BSS to be within SPL in a dedicated region located near the top of
 * the MCU SRAM, this way making it available also before relocation. Note
 * that we are not using the actual top of the MCU SRAM as there is a memory
 * location filled in by the boot ROM that we want to read out without any
 * interference from the C context.
 */
#define CONFIG_SPL_BSS_START_ADDR	(CONFIG_SYS_K3_BOOT_PARAM_TABLE_INDEX -\
					 CONFIG_SPL_BSS_MAX_SIZE)
/* Set the stack right below the SPL BSS section */
#define CONFIG_SYS_INIT_SP_ADDR         CONFIG_SPL_BSS_START_ADDR
/* Configure R5 SPL post-relocation malloc pool in DDR */
#define CONFIG_SYS_SPL_MALLOC_START	0x84000000
#define CONFIG_SYS_SPL_MALLOC_SIZE	SZ_16M
/* Image load address in RAM for DFU boot*/
#endif

/* Base address of bootloader images to load from HyperFlash */
#if defined(CONFIG_TARGET_ROVY_4VM_A72)
#define CONFIG_SYS_UBOOT_BASE		0x50280000
#elif defined(CONFIG_TARGET_ROVY_4VM_R5)
#define CONFIG_SYS_UBOOT_BASE		0x50080000
#else
#define CONFIG_SYS_UBOOT_BASE		0x50100000
#endif

#ifdef CONFIG_SYS_K3_SPL_ATF
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"tispl.bin"
#endif

#define CONFIG_SPL_MAX_SIZE		CONFIG_SYS_K3_MAX_DOWNLODABLE_IMAGE_SIZE

#define CONFIG_SYS_BOOTM_LEN		SZ_64M
#define CONFIG_CQSPI_REF_CLK		133333333

/* HyperFlash related configuration */
#define CONFIG_SYS_MAX_FLASH_BANKS_DETECT 1

/* U-Boot general configuration */
#define EXTRA_ENV_J721E_BOARD_SETTINGS					\
	"default_device_tree=" CONFIG_DEFAULT_DEVICE_TREE ".dtb\0"	\
	"findfdt="							\
		"setenv name_fdt ${default_device_tree};"		\
		"if test $board_name = J721EX-PM1-SOM; then "		\
			"setenv name_fdt k3-j721e-proc-board-tps65917.dtb; fi;" \
		"if test $board_name = j721e; then "			\
			"setenv name_fdt k3-j721e-rovy-am68-evm.dtb; fi;" \
		"if test $board_name = j721e-eaik || test $board_name = j721e-sk; then " \
			"setenv name_fdt k3-j721e-sk.dtb; fi;"	\
		"setenv fdtfile ${name_fdt}\0"				\
	"name_kern=Image\0"						\
	"console=ttyS2,115200n8\0"					\
	"args_all=setenv optargs ${optargs} "				\
		"earlycon=ns16550a,mmio32,0x02800000 ${mtdparts}\0"	\
	"run_kern=booti ${loadaddr} ${rd_spec} ${fdtaddr}\0"

#define PARTS_DEFAULT \
	/* Linux partitions */ \
	"uuid_disk=${uuid_gpt_disk};" \
	"name=rootfs,start=0,size=-,uuid=${uuid_gpt_rootfs}\0"

#ifdef CONFIG_SYS_K3_SPL_ATF
#if defined(CONFIG_TARGET_ROVY_4VM_R5)
#define EXTRA_ENV_R5_SPL_RPROC_FW_ARGS_MMC				\
	"addr_mcur5f0_0load=0x89000000\0"				\
	"name_mcur5f0_0fw=/lib/firmware/j7-mcu-r5f0_0-fw\0"
#endif /* CONFIG_TARGET_ROVY_4VM_R5 */
#else
#define EXTRA_ENV_R5_SPL_RPROC_FW_ARGS_MMC ""
#endif /* CONFIG_SYS_K3_SPL_ATF */

/* U-Boot MMC-specific configuration */
#define EXTRA_ENV_J721E_BOARD_SETTINGS_MMC				\
	"boot=mmc\0"							\
	"mmcdev=1\0"							\
	"bootpart=1:2\0"						\
	"bootdir=/boot\0"						\
	EXTRA_ENV_R5_SPL_RPROC_FW_ARGS_MMC				\
	"rd_spec=-\0"							\
	"init_mmc=run args_all args_mmc\0"				\
	"get_fdt_mmc=load mmc ${bootpart} ${fdtaddr} ${bootdir}/${name_fdt}\0" \
	"get_overlay_mmc="						\
		"fdt address ${fdtaddr};"				\
		"fdt resize 0x100000;"					\
		"for overlay in $name_overlays;"			\
		"do;"							\
		"load mmc ${bootpart} ${dtboaddr} ${bootdir}/${overlay} && "	\
		"fdt apply ${dtboaddr};"				\
		"done;\0"						\
	"partitions=" PARTS_DEFAULT					\
	"get_kern_mmc=load mmc ${bootpart} ${loadaddr} "		\
		"${bootdir}/${name_kern}\0"				\
	"get_fit_mmc=load mmc ${bootpart} ${addr_fit} "			\
		"${bootdir}/${name_fit}\0"				\
	"partitions=" PARTS_DEFAULT

/* Set the default list of remote processors to boot */
#if defined(CONFIG_TARGET_J7200_A72_EVM)
#define EXTRA_ENV_CONFIG_MAIN_CPSW0_QSGMII_PHY				\
	"do_main_cpsw0_qsgmii_phyinit=1\0"				\
	"init_main_cpsw0_qsgmii_phy=gpio set gpio@22_17;"		\
		 "gpio clear gpio@22_16\0"				\
	"main_cpsw0_qsgmii_phyinit="					\
	"if test ${do_main_cpsw0_qsgmii_phyinit} -eq 1 && test ${dorprocboot} -eq 1 && " \
			"test ${boot} = mmc; then "			\
		"run init_main_cpsw0_qsgmii_phy;"			\
	"fi;\0"
#ifdef DEFAULT_RPROCS
#undef DEFAULT_RPROCS
#endif
#elif defined(CONFIG_TARGET_ROVY_4VM_A72)
#define EXTRA_ENV_CONFIG_MAIN_CPSW0_QSGMII_PHY				\
	"init_main_cpsw0_qsgmii_phy=gpio set gpio@22_17;"		\
		 "gpio clear gpio@22_16\0"				\
	"main_cpsw0_qsgmii_phyinit="					\
	"if test $board_name = J721EX-PM1-SOM || test $board_name = J721EX-PM2-SOM " \
	"|| test $board_name = j721e; then " \
	"do_main_cpsw0_qsgmii_phyinit=1; else "			\
	"do_main_cpsw0_qsgmii_phyinit=0; fi;"			\
	"if test ${do_main_cpsw0_qsgmii_phyinit} -eq 1 && test ${dorprocboot} -eq 1 && " \
			"test ${boot} = mmc; then "			\
		"run init_main_cpsw0_qsgmii_phy;"			\
	"fi;\0"
#ifdef DEFAULT_RPROCS
#undef DEFAULT_RPROCS
#endif
#endif

#ifdef CONFIG_TARGET_ROVY_4VM_A72
#define DEFAULT_RPROCS	""						\
		"2 /lib/firmware/j7-main-r5f0_0-fw "			\
		"3 /lib/firmware/j7-main-r5f0_1-fw "			\
		"4 /lib/firmware/j7-main-r5f1_0-fw "			\
		"5 /lib/firmware/j7-main-r5f1_1-fw "			\
		"6 /lib/firmware/j7-c66_0-fw "				\
		"7 /lib/firmware/j7-c66_1-fw "				\
		"8 /lib/firmware/j7-c71_0-fw "
#endif /* CONFIG_TARGET_J721E_A72_EVM */

#ifndef EXTRA_ENV_CONFIG_MAIN_CPSW0_QSGMII_PHY
#define EXTRA_ENV_CONFIG_MAIN_CPSW0_QSGMII_PHY
#endif

#ifdef CONFIG_TARGET_J7200_A72_EVM
#define EXTRA_ENV_DFUARGS \
	DFU_ALT_INFO_MMC \
	DFU_ALT_INFO_EMMC_COMBINED \
	DFU_ALT_INFO_RAM \
	DFU_ALT_INFO_OSPI_COMBINED
#else
#define EXTRA_ENV_DFUARGS \
	DFU_ALT_INFO_MMC \
	DFU_ALT_INFO_EMMC \
	DFU_ALT_INFO_RAM \
	DFU_ALT_INFO_OSPI
#endif

#if defined(CONFIG_TARGET_ROVY_4VM_A72)
#define EXTRA_ENV_J721E_BOARD_SETTINGS_MTD				\
	"mtdids=" CONFIG_MTDIDS_DEFAULT "\0"				\
	"mtdparts=" CONFIG_MTDPARTS_DEFAULT "\0"
#else
#define EXTRA_ENV_J721E_BOARD_SETTINGS_MTD
#endif

#define TN_FLASH_BOOTLOADER_IN_OSPI \
	"flash_ospi=" \
	"sf probe;" \
	"fatload mmc 1 ${loadaddr} tiboot3.bin;" \
	"sf update $loadaddr 0x0 $filesize;" \
	"fatload mmc 1 ${loadaddr} tispl.bin;" \
	"sf update $loadaddr 0x80000 $filesize;" \
	"fatload mmc 1 ${loadaddr} u-boot.img;" \
	"sf update $loadaddr 0x280000 $filesize;" \
	"fatload mmc 1 ${loadaddr} sysfw.itb;" \
	"sf update $loadaddr 0x6C0000 $filesize\0"

#define TN_CLEAN_OSPI \
	"clean_ospi=" \
	"sf probe;" \
	"sf erase 0x0 +0x9C0000\0"

#define DEFAULT_TN_BOOT_ENV \
	"bootenvfile=boot/uEnv.txt\0" \
	"loadbootenv=load ${devtype} ${bootpart} ${loadaddr} ${bootenvfile}\0" \
	"loadbootscript=load ${devtype} ${bootpart} ${loadaddr} boot/boot.scr\0" \
	"importbootenv=echo Importing environment from ${boot} ...; " \
		"env import -t ${loadaddr} ${filesize}\0" \
	"envboot_mmc=setenv devtype mmc; " \
		"setenv bootpart ${mmcdev}:2; " \
		"mmc dev ${mmcdev}; " \
		"if mmc rescan; then " \
			"echo SD/MMC found on device ${mmcdev};" \
			"if run loadbootscript; then " \
				"run bootscript;" \
			"else " \
				"if run loadbootenv; then " \
					"echo Loaded env from ${bootenvfile};" \
					"run importbootenv;" \
				"fi;" \
				"if test -n $uenvcmd; then " \
					"echo Running uenvcmd ...;" \
					"run uenvcmd;" \
				"fi;" \
			"fi;" \
		"fi;\0" \
	"envboot_ufs=setenv devtype scsi; " \
		"setenv bootpart 1:1; " \
		"ufs init; " \
		"if scsi scan;; then " \
			"echo UFS found; " \
			"if run loadbootscript; then " \
				"run bootscript;" \
			"else " \
				"if run loadbootenv; then " \
					"echo Loaded env from ${bootenvfile};" \
					"run importbootenv;" \
				"fi;" \
				"if test -n $uenvcmd; then " \
					"echo Running uenvcmd ...;" \
					"run uenvcmd;" \
				"fi;" \
			"fi;" \
		"fi;\0"

#define EXTRA_ENV_TN_ENV \
	TN_FLASH_BOOTLOADER_IN_OSPI \
	TN_CLEAN_OSPI

/* Incorporate settings into the U-Boot environment */
#define CONFIG_EXTRA_ENV_SETTINGS					\
	DEFAULT_LINUX_BOOT_ENV						\
	DEFAULT_MMC_TI_ARGS						\
	DEFAULT_FIT_TI_ARGS						\
	EXTRA_ENV_J721E_BOARD_SETTINGS					\
	EXTRA_ENV_J721E_BOARD_SETTINGS_MMC				\
	EXTRA_ENV_RPROC_SETTINGS					\
	EXTRA_ENV_DFUARGS						\
	DEFAULT_UFS_TI_ARGS						\
	EXTRA_ENV_TN_ENV						\
	DEFAULT_TN_BOOT_ENV						\
	EXTRA_ENV_J721E_BOARD_SETTINGS_MTD				\
	EXTRA_ENV_CONFIG_MAIN_CPSW0_QSGMII_PHY

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>

/* MMC ENV related defines */

#endif /* __CONFIG_ROVY_4VM_H */
