/*
 * THIS FILE IS AUTOMATICALLY GENERATED
 * DONT EDIT THIS FILE
 */

/*	$OpenBSD: cn30xxgmxreg.h,v 1.3 2016/06/09 15:29:22 visa Exp $	*/

/*
 * Copyright (c) 2007 Internet Initiative Japan, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Cavium Networks OCTEON CN30XX Hardware Reference Manual
 * CN30XX-HM-1.0
 * 13.8 GMX Registers
 */

#ifndef _CN30XXGMXREG_H_
#define _CN30XXGMXREG_H_

#define	GMX0_RX0_INT_REG			0x000
#define	GMX0_RX0_INT_EN				0x008
#define	GMX0_PRT0_CFG				0x010
#define	GMX0_RX0_FRM_CTL			0x018
#define	GMX0_RX0_FRM_CHK			0x020
#define	GMX0_RX0_FRM_MIN			0x028
#define	GMX0_RX0_FRM_MAX			0x030
#define	GMX0_RX0_JABBER				0x038
#define	GMX0_RX0_DECISION			0x040
#define	GMX0_RX0_UDD_SKP			0x048
#define	GMX0_RX0_STATS_CTL			0x050
#define	GMX0_RX0_IFG				0x058
#define	GMX0_RX0_RX_INBND			0x060
#define	GMX0_RX0_STATS_PKTS			0x080
#define	GMX0_RX0_STATS_OCTS			0x088
#define	GMX0_RX0_STATS_PKTS_CTL			0x090
#define	GMX0_RX0_STATS_OCTS_CTL			0x098
#define	GMX0_RX0_STATS_PKTS_DMAC		0x0a0
#define	GMX0_RX0_STATS_OCTS_DMAC		0x0a8
#define	GMX0_RX0_STATS_PKTS_DRP			0x0b0
#define	GMX0_RX0_STATS_OCTS_DRP			0x0b8
#define	GMX0_RX0_STATS_PKTS_BAD			0x0c0
#define	GMX0_RX0_ADR_CTL			0x100
#define	GMX0_RX0_ADR_CAM_EN			0x108
#define	GMX0_RX0_ADR_CAM0			0x180
#define	GMX0_RX0_ADR_CAM1			0x188
#define	GMX0_RX0_ADR_CAM2			0x190
#define	GMX0_RX0_ADR_CAM3			0x198
#define	GMX0_RX0_ADR_CAM4			0x1a0
#define	GMX0_RX0_ADR_CAM5			0x1a8
#define	GMX0_TX0_CLK				0x208
#define	GMX0_TX0_THRESH				0x210
#define	GMX0_TX0_APPEND				0x218
#define	GMX0_TX0_SLOT				0x220
#define	GMX0_TX0_BURST				0x228
#define	GMX0_SMAC0				0x230
#define	GMX0_TX0_PAUSE_PKT_TIME			0x238
#define	GMX0_TX0_MIN_PKT			0x240
#define	GMX0_TX0_PAUSE_PKT_INTERVAL		0x248
#define	GMX0_TX0_SOFT_PAUSE			0x250
#define	GMX0_TX0_PAUSE_TOGO			0x258
#define	GMX0_TX0_PAUSE_ZERO			0x260
#define	GMX0_TX0_STATS_CTL			0x268
#define	GMX0_TX0_CTL				0x270
#define	GMX0_TX0_STAT0				0x280
#define	GMX0_TX0_STAT1				0x288
#define	GMX0_TX0_STAT2				0x290
#define	GMX0_TX0_STAT3				0x298
#define	GMX0_TX0_STAT4				0x2a0
#define	GMX0_TX0_STAT5				0x2a8
#define	GMX0_TX0_STAT6				0x2b0
#define	GMX0_TX0_STAT7				0x2b8
#define	GMX0_TX0_STAT8				0x2c0
#define	GMX0_TX0_STAT9				0x2c8
#define	GMX0_BIST0				0x400
#define	GMX0_RX_PRTS				0x410
#define	GMX0_RX_BP_DROP0			0x420
#define	GMX0_RX_BP_DROP1			0x428
#define	GMX0_RX_BP_DROP2			0x430
#define	GMX0_RX_BP_ON0				0x440
#define	GMX0_RX_BP_ON1				0x448
#define	GMX0_RX_BP_ON2				0x450
#define	GMX0_RX_BP_OFF0				0x460
#define	GMX0_RX_BP_OFF1				0x468
#define	GMX0_RX_BP_OFF2				0x470
#define	GMX0_TX_PRTS				0x480
#define	GMX0_TX_IFG				0x488
#define	GMX0_TX_JAM				0x490
#define	GMX0_TX_COL_ATTEMPT			0x498
#define	GMX0_TX_PAUSE_PKT_DMAC			0x4a0
#define	GMX0_TX_PAUSE_PKT_TYPE			0x4a8
#define	GMX0_TX_OVR_BP				0x4c8
#define	GMX0_TX_BP				0x4d0
#define	GMX0_TX_CORRUPT				0x4d8
#define	GMX0_RX_PRT_INFO			0x4e8
#define	GMX0_TX_LFSR				0x4f8
#define	GMX0_TX_INT_REG				0x500
#define	GMX0_TX_INT_EN				0x508
#define	GMX0_NXA_ADR				0x510
#define	GMX0_BAD_REG				0x518
#define	GMX0_STAT_BP				0x520
#define	GMX0_TX_CLK_MSK0			0x780
#define	GMX0_TX_CLK_MSK1			0x788
#define	GMX0_RX_TX_STATUS			0x7e8
#define	GMX0_INF_MODE				0x7f8

/* -------------------------------------------------------------------------- */

/* GMX Interrupt Registers */

#define	RXN_INT_REG_XXX_63_19			0xfffffffffff80000ULL
#define	RXN_INT_REG_PHY_DUPX			0x0000000000040000ULL
#define	RXN_INT_REG_PHY_SPD			0x0000000000020000ULL
#define	RXN_INT_REG_PHY_LINK			0x0000000000010000ULL
#define	RXN_INT_REG_IFGERR			0x0000000000008000ULL
#define	RXN_INT_REG_COLDET			0x0000000000004000ULL
#define	RXN_INT_REG_FALERR			0x0000000000002000ULL
#define	RXN_INT_REG_RSVERR			0x0000000000001000ULL
#define	RXN_INT_REG_PCTERR			0x0000000000000800ULL
#define	RXN_INT_REG_OVRERR			0x0000000000000400ULL
#define	RXN_INT_REG_NIBERR			0x0000000000000200ULL
#define	RXN_INT_REG_SKPERR			0x0000000000000100ULL
#define	RXN_INT_REG_RCVERR			0x0000000000000080ULL
#define	RXN_INT_REG_LENERR			0x0000000000000040ULL
#define	RXN_INT_REG_ALNERR			0x0000000000000020ULL
#define	RXN_INT_REG_FCSERR			0x0000000000000010ULL
#define	RXN_INT_REG_JABBER			0x0000000000000008ULL
#define	RXN_INT_REG_MAXERR			0x0000000000000004ULL
#define	RXN_INT_REG_CAREXT			0x0000000000000002ULL
#define	RXN_INT_REG_MINERR			0x0000000000000001ULL

/* GMX Interrupt-Enable Registers */

#define	RXN_INT_EN_XXX_63_19			0xfffffffffff80000ULL
#define	RXN_INT_EN_PHY_DUPX			0x0000000000040000ULL
#define	RXN_INT_EN_PHY_SPD			0x0000000000020000ULL
#define	RXN_INT_EN_PHY_LINK			0x0000000000010000ULL
#define	RXN_INT_EN_IFGERR			0x0000000000008000ULL
#define	RXN_INT_EN_COLDET			0x0000000000004000ULL
#define	RXN_INT_EN_FALERR			0x0000000000002000ULL
#define	RXN_INT_EN_RSVERR			0x0000000000001000ULL
#define	RXN_INT_EN_PCTERR			0x0000000000000800ULL
#define	RXN_INT_EN_OVRERR			0x0000000000000400ULL
#define	RXN_INT_EN_NIBERR			0x0000000000000200ULL
#define	RXN_INT_EN_SKPERR			0x0000000000000100ULL
#define	RXN_INT_EN_RCVERR			0x0000000000000080ULL
#define	RXN_INT_EN_LENERR			0x0000000000000040ULL
#define	RXN_INT_EN_ALNERR			0x0000000000000020ULL
#define	RXN_INT_EN_FCSERR			0x0000000000000010ULL
#define	RXN_INT_EN_JABBER			0x0000000000000008ULL
#define	RXN_INT_EN_MAXERR			0x0000000000000004ULL
#define	RXN_INT_EN_CAREXT			0x0000000000000002ULL
#define	RXN_INT_EN_MINERR			0x0000000000000001ULL

/* GMX Port Configuration Registers */

#define	PRTN_CFG_XXX_63_9			0xfffffffffffffe00ULL
#define	PRTN_CFG_SPEED_MSB			0x0000000000000100ULL
#define	PRTN_CFG_XXX_7_4			0x00000000000000f0ULL
#define	PRTN_CFG_SLOTTIME			0x0000000000000008ULL
#define	PRTN_CFG_DUPLEX				0x0000000000000004ULL
#define	PRTN_CFG_SPEED				0x0000000000000002ULL
#define	PRTN_CFG_EN				0x0000000000000001ULL

/* Frame Control Registers */

#define	RXN_FRM_CTL_XXX_63_11			0xfffffffffffff800ULL
#define	RXN_FRM_CTL_NULL_DIS			0x0000000000000400ULL
#define	RXN_FRM_CTL_PRE_ALIGN			0x0000000000000200ULL
#define	RXN_FRM_CTL_PAD_LEN			0x0000000000000100ULL
#define	RXN_FRM_CTL_VLAN_LEN			0x0000000000000080ULL
#define	RXN_FRM_CTL_PRE_FREE			0x0000000000000040ULL
#define	RXN_FRM_CTL_CTL_SMAC			0x0000000000000020ULL
#define	RXN_FRM_CTL_CTL_MCST			0x0000000000000010ULL
#define	RXN_FRM_CTL_CTL_BCK			0x0000000000000008ULL
#define	RXN_FRM_CTL_CTL_DRP			0x0000000000000004ULL
#define	RXN_FRM_CTL_PRE_STRP			0x0000000000000002ULL
#define	RXN_FRM_CTL_PRE_CHK			0x0000000000000001ULL

/* Frame Check Registers */

#define RXN_FRM_CKK_XXX_63_10			0xfffffffffffffc00ULL
#define	RXN_FRM_CHK_NIBERR			0x0000000000000200ULL
#define	RXN_FRM_CHK_SKPERR			0x0000000000000100ULL
#define	RXN_FRM_CHK_RCVERR			0x0000000000000080ULL
#define	RXN_FRM_CHK_LENERR			0x0000000000000040ULL
#define	RXN_FRM_CHK_ALNERR			0x0000000000000020ULL
#define	RXN_FRM_CHK_FCSERR			0x0000000000000010ULL
#define	RXN_FRM_CHK_JABBER			0x0000000000000008ULL
#define	RXN_FRM_CHK_MAXERR			0x0000000000000004ULL
#define	RXN_FRM_CHK_CAREXT			0x0000000000000002ULL
#define	RXN_FRM_CHK_MINERR			0x0000000000000001ULL

/* Frame Minimum-Length Registers */

#define	RXN_RRM_MIN_XXX_63_16			0xffffffffffff0000ULL
#define	RXN_RRM_MIN_LEN				0x000000000000ffffULL

/* Frame Maximun-Length Registers */

#define	RXN_RRM_MAX_XXX_63_16			0xffffffffffff0000ULL
#define	RXN_RRM_MAX_LEN				0x000000000000ffffULL

/* GMX Maximun Packet-Size Registers */

#define	RXN_JABBER_XXX_63_16			0xffffffffffff0000ULL
#define	RXN_JABBER_CNT				0x000000000000ffffULL

/* GMX Packet Decision Registers */

#define	RXN_DECISION_XXX_63_5			0xffffffffffffffe0ULL
#define	RXN_DECISION_CNT			0x000000000000001fULL

/* GMX User-Defined Data Skip Registers */

#define	RXN_UDD_SKP_XXX_63_9			0xfffffffffffffe00ULL
#define	RXN_UDD_SKP_FCSSEL			0x0000000000000100ULL
#define	RXN_UDD_SKP_XXX_7			0x0000000000000080ULL
#define	RXN_UDD_SKP_LEN				0x000000000000007fULL

/* GMX RX Statistics Control Registers */

#define	RXN_STATS_CTL_XXX_63_1			0xfffffffffffffffeULL
#define	RXN_STATS_CTL_RD_CLR			0x0000000000000001ULL

/* GMX Minimun Interface-Gap Cycles Registers */

#define	RXN_IFG_XXX_63_4			0xfffffffffffffff0ULL
#define	RXN_IFG_IFG				0x000000000000000fULL

/* InBand Link Status Registers */

#define	RXN_RX_INBND_XXX_63_4			0xfffffffffffffff0ULL
#define	RXN_RX_INBND_DUPLEX			0x0000000000000008ULL
#define	 RXN_RX_INBND_DUPLEX_SHIFT		3
#define	  RXN_RX_INBND_DUPLEX_HALF		(0ULL << RXN_RX_INBND_DUPLEX_SHIFT)
#define	  RXN_RX_INBND_DUPLEX_FULL		(1ULL << RXN_RX_INBND_DUPLEX_SHIFT)
#define	RXN_RX_INBND_SPEED			0x0000000000000006ULL
#define	 RXN_RX_INBND_SPEED_SHIFT		1
#define	  RXN_RX_INBND_SPEED_2_5		(0ULL << RXN_RX_INBND_SPEED_SHIFT)
#define	  RXN_RX_INBND_SPEED_25			(1ULL << RXN_RX_INBND_SPEED_SHIFT)
#define	  RXN_RX_INBND_SPEED_125		(2ULL << RXN_RX_INBND_SPEED_SHIFT)
#define	  RXN_RX_INBND_SPEED_XXX_3		(3ULL << RXN_RX_INBND_SPEED_SHIFT)
#define	RXN_RX_INBND_STATUS			0x0000000000000001ULL

/* GMX RX Good Packets Registers */

#define	RXN_STATS_PKTS_XXX_63_32		0xffffffff00000000ULL
#define	RXN_STATS_PKTS_CNT			0x00000000ffffffffULL

/* GMX RX Good Packets Octet Registers */

#define	RXN_STATS_OCTS_XXX_63_48		0xffff000000000000ULL
#define	RXN_STATS_OCTS_CNT			0x0000ffffffffffffULL

/* GMX RX Pause Packets Registers */

#define	RXN_STATS_PKTS_CTL_XXX_63_32		0xffffffff00000000ULL
#define	RXN_STATS_PKTS_CTL_CNT			0x00000000ffffffffULL

/* GMX RX Pause Packets Octet Registers */

#define	RXN_STATS_OCTS_CTL_XXX_63_48		0xffff000000000000ULL
#define	RXN_STATS_OCTS_CTL_CNT			0x0000ffffffffffffULL

/* GMX RX DMAC Packets Registers */

#define	RXN_STATS_PKTS_DMAC_XXX_63_32		0xffffffff00000000ULL
#define	RXN_STATS_PKTS_DMAC_CNT			0x00000000ffffffffULL

/* GMX RX DMAC Packets Octet Registers */

#define	RXN_STATS_OCTS_DMAC_XXX_63_48		0xffff000000000000ULL
#define	RXN_STATS_OCTS_DMAC_CNT			0x0000ffffffffffffULL

/* GMX RX Overflow Packets Registers */

#define	RXN_STATS_PKTS_DRP_XXX_63_48		0xffffffff00000000ULL
#define	RXN_STATS_PKTS_DRP_CNT			0x00000000ffffffffULL

/* GMX RX Overflow Packets Octet Registers */

#define	RXN_STATS_OCTS_DRP_XXX_63_48		0xffff000000000000ULL
#define	RXN_STATS_OCTS_DRP_CNT			0x0000ffffffffffffULL

/* GMX RX Bad Packets Registers */

#define	RXN_STATS_PKTS_BAD_XXX_63_48		0xffffffff00000000ULL
#define	RXN_STATS_PKTS_BAD_CNT			0x00000000ffffffffULL

/* Address-Filtering Control Registers */

#define	RXN_ADR_CTL_XXX_63_4			0xfffffffffffffff0ULL
#define	RXN_ADR_CTL_CAM_MODE			0x0000000000000008ULL
#define	 RXN_ADR_CTL_CAM_MODE_SHIFT		3
#define	  RXN_ADR_CTL_CAM_MODE_REJECT		(0ULL << RXN_ADR_CTL_CAM_MODE_SHIFT)
#define	  RXN_ADR_CTL_CAM_MODE_ACCEPT		(1ULL << RXN_ADR_CTL_CAM_MODE_SHIFT)
#define	RXN_ADR_CTL_MCST			0x0000000000000006ULL
#define	 RXN_ADR_CTL_MCST_SHIFT			1
#define	  RXN_ADR_CTL_MCST_AFCAM		(0ULL << RXN_ADR_CTL_MCST_SHIFT)
#define	  RXN_ADR_CTL_MCST_REJECT		(1ULL << RXN_ADR_CTL_MCST_SHIFT)
#define	  RXN_ADR_CTL_MCST_ACCEPT		(2ULL << RXN_ADR_CTL_MCST_SHIFT)
#define	  RXN_ADR_CTL_MCST_XXX_3		(3ULL << RXN_ADR_CTL_MCST_SHIFT)
#define	RXN_ADR_CTL_BCST			0x0000000000000001ULL

/* Address-Filtering Control Enable Registers */

#define	RXN_ADR_CAM_EN_XXX_63_8			0xffffffffffffff00ULL
#define	RXN_ADR_CAM_EN_EN			0x00000000000000ffULL

/* Address-Filtering CAM Control Registers */
#define	RXN_ADR_CAMN_ADR			0xffffffffffffffffULL

/* GMX TX Clock Generation Registers */

#define	TXN_CLK_XXX_63_6			0xffffffffffffffc0ULL
#define	TXN_CLK_CLK_CNT				0x000000000000003fULL

/* TX Threshold Registers */

#define	TXN_THRESH_XXX_63_6			0xffffffffffffffc0ULL
#define	TXN_THRESH_CNT				0x000000000000003fULL

/* TX Append Control Registers */

#define	TXN_APPEND_XXX_63_4			0xfffffffffffffff0ULL
#define	TXN_APPEND_FORCE_FCS			0x0000000000000008ULL
#define	TXN_APPEND_FCS				0x0000000000000004ULL
#define	TXN_APPEND_PAD				0x0000000000000002ULL
#define	TXN_APPEND_PREAMBLE			0x0000000000000001ULL

/* TX Slottime Counter Registers */

#define	TXN_SLOT_XXX_63_10			0xfffffffffffffc00ULL
#define	TXN_SLOT_SLOT				0x00000000000003ffULL

/* TX Burst-Counter Registers */

#define	TXN_BURST_XXX_63_16			0xffffffffffff0000ULL
#define	TXN_BURST_BURST				0x000000000000ffffULL

/* RGMII SMAC Registers */

#define	SMACN_XXX_63_48				0xffff000000000000ULL
#define	SMACN_SMAC				0x0000ffffffffffffULL

/* TX Pause Packet Pause-Time Registers */

#define	TXN_PAUSE_PKT_TIME_XXX_63_16		0xffffffffffff0000ULL
#define	TXN_PAUSE_PKT_TIME_TIME			0x000000000000ffffULL

/* RGMII TX Minimum-Size-Packet Registers */

#define	TXN_MIN_PKT_XXX_63_8			0xffffffffffffff00ULL
#define	TXN_MIN_PKT_MIN_SIZE			0x00000000000000ffULL

/* TX Pause-Packet Transmission-Interval Registers */

#define	TXN_PAUSE_PKT_INTERVAL_XXX_63_16	0xffffffffffff0000ULL
#define	TXN_PAUSE_PKT_INTERVAL_INTERVAL		0x000000000000ffffULL

/* TX Software-Pause Registers */

#define	TXN_SOFT_PAUSE_XXX_63_16		0xffffffffffff0000ULL
#define	TXN_SOFT_PAUSE_TIME			0x000000000000ffffULL

/* TX Time-to-Backpressure Registers */

#define	TXN_PAUSE_TOGO_XXX_63_16		0xffffffffffff0000ULL
#define	TXN_PAUSE_TOGO_TIME			0x000000000000ffffULL

/* TX Pause-Zero-Enable Registers */

#define	TXN_PAUSE_ZERO_XXX_63_1			0xfffffffffffffffeULL
#define	TXN_PAUSE_ZERO_SEND			0x0000000000000001ULL

/* GMX TX Statistics Control Registers */

#define	TXN_STATS_CTL_XXX_63_1			0xfffffffffffffffeULL
#define	TXN_STATS_CTL_RD_CLR			0x0000000000000001ULL

/* GMX TX Transmit Control Registers */

#define	TXN_CTL_XXX_63_2			0xfffffffffffffffcULL
#define	TXN_CTL_XSDEF_EN			0x0000000000000002ULL
#define	TXN_CTL_XSCOL_EN			0x0000000000000001ULL

/* Transmit Statistics Registers 0 */

#define	TXN_STAT0_XSDEF				0xffffffff00000000ULL
#define	TXN_STAT0_XSCOL				0x00000000ffffffffULL

/* Transmit Statistics Registers 1 */

#define	TXN_STAT1_SCOL				0xffffffff00000000ULL
#define	TXN_STAT1_MSCOL				0x00000000ffffffffULL

/* Transmit Statistics Registers 2 */

#define	TXN_STAT2_XXX_63_48			0xffff000000000000ULL
#define	TXN_STAT2_OCTS				0x0000ffffffffffffULL

/* Transmit Statistics Registers 3 */

#define	TXN_STAT3_XXX_63_48			0xffffffff00000000ULL
#define	TXN_STAT3_PKTS				0x00000000ffffffffULL

/* Transmit Statistics Registers 4 */

#define	TXN_STAT4_HIST1				0xffffffff00000000ULL
#define	TXN_STAT4_HIST0				0x00000000ffffffffULL

/* Transmit Statistics Registers 5 */

#define	TXN_STAT5_HIST3				0xffffffff00000000ULL
#define	TXN_STAT5_HIST2				0x00000000ffffffffULL

/* Transmit Statistics Registers 6 */

#define	TXN_STAT6_HIST5				0xffffffff00000000ULL
#define	TXN_STAT6_HIST4				0x00000000ffffffffULL

/* Transmit Statistics Registers 7 */

#define	TXN_STAT7_HIST7				0xffffffff00000000ULL
#define	TXN_STAT7_HIST6				0x00000000ffffffffULL

/* Transmit Statistics Registers 8 */

#define	TXN_STAT8_MCST				0xffffffff00000000ULL
#define	TXN_STAT8_BCST				0x00000000ffffffffULL

/* Transmit Statistics Register 9 */

#define	TXN_STAT9_UNDFLW			0xffffffff00000000ULL
#define	TXN_STAT9_CTL				0x00000000ffffffffULL

/* BMX BIST Results Register */

#define	BIST_XXX_63_10				0xfffffffffffffc00ULL
#define	BIST_STATUS				0x00000000000003ffULL

/* RX Ports Register */

#define	RX_PRTS_XXX_63_3			0xfffffffffffffff8ULL
#define	RX_PRTS_PRTS				0x0000000000000007ULL

/* RX FIFO Packet-Drop Registers */

#define	RX_BP_DROPN_XXX_63_6			0xffffffffffffffc0ULL
#define	RX_BP_DROPN_MARK			0x000000000000003fULL

/* RX Backpressure On Registers */

#define	RX_BP_ONN_XXX_63_9			0xfffffffffffffe00ULL
#define	RX_BP_ONN_MARK				0x00000000000001ffULL

/* RX Backpressure Off Registers */

#define	RX_BP_OFFN_XXX_63_6			0xffffffffffffffc0ULL
#define	RX_BP_OFFN_MARK				0x000000000000003fULL

/* TX Ports Register */

#define	TX_PRTS_XXX_63_5			0xffffffffffffffe0ULL
#define	TX_PRTS_PRTS				0x000000000000001fULL

/* TX Interframe Gap Register */

#define	TX_IFG_XXX_63_8				0xffffffffffffff00ULL
#define	TX_IFG_IFG2				0x00000000000000f0ULL
#define	TX_IFG_IFG1				0x000000000000000fULL

/* TX Jam Pattern Register */

#define	TX_JAM_XXX_63_8				0xffffffffffffff00ULL
#define	TX_JAM_JAM				0x00000000000000ffULL

/* TX Collision Attempts Before Dropping Frame Register */

#define	TX_COL_ATTEMPT_XXX_63_5			0xffffffffffffffe0ULL
#define	TX_COL_ATTEMPT_LIMIT			0x000000000000001fULL

/* TX Pause-Packet DMAC-Field Register */

#define	TX_PAUSE_PKT_DMAC_XXX_63_48		0xffff000000000000ULL
#define	TX_PAUSE_PKT_DMAC_DMAC			0x0000ffffffffffffULL

/* TX Pause Packet Type Field Register */

#define	TX_PAUSE_PKT_TYPE_XXX_63_16		0xffffffffffff0000ULL
#define	TX_PAUSE_PKT_TYPE_TYPE			0x000000000000ffffULL

/* TX Override Backpressure Register */

#define	TX_OVR_BP_XXX_63_12			0xfffffffffffff000ULL
#define	TX_OVR_BP_XXX_11			0x0000000000000800ULL
#define	TX_OVR_BP_EN				0x0000000000000700ULL
#define	 TX_OVR_BP_EN_SHIFT			8
#define	TX_OVR_BP_XXX_7				0x0000000000000080ULL
#define	TX_OVR_BP_BP				0x0000000000000070ULL
#define	 TX_OVR_BP_BP_SHIFT			4
#define	TX_OVR_BP_XXX_3				0x0000000000000008ULL
#define	TX_OVR_BP_IGN_FULL			0x0000000000000007ULL
#define	 TX_OVR_BP_IGN_FULL_SHIFT		0

/* TX Override Backpressure Register */

#define	TX_OVR_BP_XXX_63_12			0xfffffffffffff000ULL
#define	TX_OVR_BP_XXX_11			0x0000000000000800ULL
#define	TX_OVR_BP_EN				0x0000000000000700ULL
#define	TX_OVR_BP_XXX_7				0x0000000000000080ULL
#define	TX_OVR_BP_BP				0x0000000000000070ULL
#define	TX_OVR_BP_XXX_3				0x0000000000000008ULL
#define	TX_OVR_BP_IGN_FULL			0x0000000000000007ULL

/* TX Backpressure Status Register */

#define	TX_BP_SR_XXX_63_3			0xfffffffffffffff8ULL
#define	TX_BP_SR_BP				0x0000000000000007ULL

/* TX Corrupt Packets Register */

#define	TX_CORRUPT_XXX_63_3			0xfffffffffffffff8ULL
#define	TX_CORRUPT_CORRUPT			0x0000000000000007ULL

/* RX Port State Information Register */

#define	RX_PRT_INFO_XXX_63_19			0xfffffffffff80000ULL
#define	RX_PRT_INFO_DROP			0x0000000000070000ULL
#define	RX_PRT_INFO_XXX_15_3			0x000000000000fff8ULL
#define	RX_PRT_INFO_COMMIT			0x0000000000000007ULL

/* TX LFSR Register */

#define	TX_LFSR_XXX_63_16			0xffffffffffff0000ULL
#define	TX_LFSR_LFSR				0x000000000000ffffULL

/* TX Interrupt Register */

#define	TX_INT_REG_XXX_63_20			0xfffffffffff00000ULL
#define	TX_INT_REG_XXX_19			0x0000000000080000ULL
#define	TX_INT_REG_LATE_COL			0x0000000000070000ULL
#define	TX_INT_REG_XXX_15			0x0000000000008000ULL
#define	TX_INT_REG_XSDEF			0x0000000000007000ULL
#define	TX_INT_REG_XXX_11			0x0000000000000800ULL
#define	TX_INT_REG_XSCOL			0x0000000000000700ULL
#define	TX_INT_REG_XXX_7_5			0x00000000000000e0ULL
#define	TX_INT_REG_UNDFLW			0x000000000000001cULL
#define	TX_INT_REG_XXX_1			0x0000000000000002ULL
#define	TX_INT_REG_PKO_NXA			0x0000000000000001ULL

/* TX Interrupt Register */

#define	TX_INT_EN_XXX_63_20			0xfffffffffff00000ULL
#define	TX_INT_EN_XXX_19			0x0000000000080000ULL
#define	TX_INT_EN_LATE_COL			0x0000000000070000ULL
#define	TX_INT_EN_XXX_15			0x0000000000008000ULL
#define	TX_INT_EN_XSDEF				0x0000000000007000ULL
#define	TX_INT_EN_XXX_11			0x0000000000000800ULL
#define	TX_INT_EN_XSCOL				0x0000000000000700ULL
#define	TX_INT_EN_XXX_7_5			0x00000000000000e0ULL
#define	TX_INT_EN_UNDFLW			0x000000000000001cULL
#define	TX_INT_EN_XXX_1				0x0000000000000002ULL
#define	TX_INT_EN_PKO_NXA			0x0000000000000001ULL

/* Address-out-of-Range Error Register */

#define	NXA_ADR_XXX_63_6			0xffffffffffffffc0ULL
#define	NXA_ADR_PRT				0x000000000000003fULL

/* GMX Miscellaneous Error Register */

#define	BAD_REG_XXX_63_31			0xffffffff80000000ULL
#define	BAD_REG_INB_NXA				0x0000000078000000ULL
#define	BAD_REG_STATOVR				0x0000000004000000ULL
#define	BAD_REG_XXX_25				0x0000000002000000ULL
#define	BAD_REG_LOSTSTAT			0x0000000001c00000ULL
#define	BAD_REG_XXX_21_18			0x00000000003c0000ULL
#define	BAD_REG_XXX_17_5			0x000000000003ffe0ULL
#define	BAD_REG_OUT_OVR				0x000000000000001cULL
#define	BAD_REG_XXX_1_0				0x0000000000000003ULL

/* GMX Backpressure Statistics Register */

#define	STAT_BP_XXX_63_17			0xfffffffffffe0000ULL
#define	STAT_BP_BP				0x0000000000010000ULL
#define	STAT_BP_CNT				0x000000000000ffffULL

/* Mode Change Mask Registers */

#define	TX_CLK_MSKN_XXX_63_1			0xfffffffffffffffeULL
#define	TX_CLK_MSKN_MSK				0x0000000000000001ULL

/* GMX RX/TX Status Register */

#define	RX_TX_STATUS_XXX_63_7			0xffffffffffffff80ULL
#define	RX_TX_STATUS_TX				0x0000000000000070ULL
#define	RX_TX_STATUS_XXX_3			0x0000000000000008ULL
#define	RX_TX_STATUS_RX				0x0000000000000007ULL

/* Interface Mode Register */

#define	INF_MODE_XXX_63_3			0xfffffffffffffff8ULL
#define	INF_MODE_P0MII				0x0000000000000004ULL
#define	INF_MODE_EN				0x0000000000000002ULL
#define	INF_MODE_TYPE				0x0000000000000001ULL

#define	MIO_QLM_CFG(x)				(0x0001180000001590ULL + (x)*8)

#define	MIO_QLM_CFG_CFG				0x000000000000000fULL

/* -------------------------------------------------------------------------- */

/* for bus_space(9) */

#define GMX_IF_NUNITS				1
#define GMX_PORT_NUNITS				4

#define	GMX0_BASE_PORT0				0x0001180008000000ULL
#define	GMX0_BASE_PORT1				0x0001180008000800ULL
#define	GMX0_BASE_PORT2				0x0001180008001000ULL
#define	GMX0_BASE_PORT_SIZE				0x00800
#define	GMX0_BASE_IF0				0x0001180008000000ULL
#define	GMX0_BASE_IF_SIZE(n)			(GMX0_BASE_PORT_SIZE * (n))

/* -------------------------------------------------------------------------- */

/* Low-level SGMII link control */

#define	PCS_BASE(g, i)	(0x00011800b0001000ULL + 0x20000 * (g) + 0x400 * (i))
#define	PCS_SIZE	0x98

#define	PCS_MR_CONTROL				0x00
#define	PCS_MR_STATUS				0x08
#define	PCS_LINK_TIMER_COUNT			0x40
#define	PCS_MISC_CTL				0x78

#define	PCS_MR_CONTROL_RES_16_63		0xffffffffffff0000ULL
#define	PCS_MR_CONTROL_RESET			0x0000000000008000ULL
#define	PCS_MR_CONTROL_LOOPBCK1			0x0000000000004000ULL
#define	PCS_MR_CONTROL_SPDLSB			0x0000000000002000ULL
#define	PCS_MR_CONTROL_AN_EN			0x0000000000001000ULL
#define	PCS_MR_CONTROL_PWR_DN			0x0000000000000800ULL
#define	PCS_MR_CONTROL_RES_10_10		0x0000000000000400ULL
#define	PCS_MR_CONTROL_RST_AN			0x0000000000000200ULL
#define	PCS_MR_CONTROL_DUPLEX			0x0000000000000100ULL
#define	PCS_MR_CONTROL_COLTST			0x0000000000000080ULL
#define	PCS_MR_CONTROL_SPDMSB			0x0000000000000040ULL
#define	PCS_MR_CONTROL_UNI			0x0000000000000020ULL
#define	PCS_MR_CONTROL_RES_0_4			0x000000000000001fULL

#define	PCS_MR_STATUS_RES_16_63			0xffffffffffff0000ULL
#define	PCS_MR_STATUS_HUN_T4			0x0000000000008000ULL
#define	PCS_MR_STATUS_HUN_XFD			0x0000000000004000ULL
#define	PCS_MR_STATUS_HUN_XHD			0x0000000000002000ULL
#define	PCS_MR_STATUS_TEN_FD			0x0000000000001000ULL
#define	PCS_MR_STATUS_TEN_HD			0x0000000000000800ULL
#define	PCS_MR_STATUS_HUN_T2FD			0x0000000000000400ULL
#define	PCS_MR_STATUS_HUN_T2HD			0x0000000000000200ULL
#define	PCS_MR_STATUS_EXT_ST			0x0000000000000100ULL
#define	PCS_MR_STATUS_RES_7_7			0x0000000000000080ULL
#define	PCS_MR_STATUS_PRB_SUP			0x0000000000000040ULL
#define	PCS_MR_STATUS_AN_CPT			0x0000000000000020ULL
#define	PCS_MR_STATUS_RM_FLT			0x0000000000000010ULL
#define	PCS_MR_STATUS_AN_ABIL			0x0000000000000008ULL
#define	PCS_MR_STATUS_LNK_ST			0x0000000000000004ULL
#define	PCS_MR_STATUS_RES_1_1			0x0000000000000002ULL
#define	PCS_MR_STATUS_EXTND			0x0000000000000001ULL

#define	PCS_LINK_TIMER_COUNT_MASK		0x000000000000ffffULL

#define	PCS_MISC_CTL_SGMII			0x0000000000001000ULL
#define	PCS_MISC_CTL_GMXENO			0x0000000000000800ULL
#define	PCS_MISC_CTL_LOOPBCK2			0x0000000000000400ULL
#define	PCS_MISC_CTL_MAC_PHY			0x0000000000000200ULL
#define	PCS_MISC_CTL_MODE			0x0000000000000100ULL
#define	PCS_MISC_CTL_AN_OVRD			0x0000000000000080ULL
#define	PCS_MISC_CTL_SAMP_PT			0x000000000000007fULL

#endif /* _CN30XXGMXREG_H_ */
