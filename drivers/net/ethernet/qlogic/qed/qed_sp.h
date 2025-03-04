/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */
/* QLogic qed NIC Driver
 * Copyright (c) 2015-2017  QLogic Corporation
 * Copyright (c) 2019-2020 Marvell International Ltd.
 */

#ifndef _QED_SP_H
#define _QED_SP_H

#include <freax/types.h>
#include <freax/kernel.h>
#include <freax/list.h>
#include <freax/slab.h>
#include <freax/spinlock.h>
#include <freax/qed/qed_chain.h>
#include "qed.h"
#include "qed_hsi.h"

enum spq_mode {
	QED_SPQ_MODE_BLOCK,     /* Client will poll a designated mem. address */
	QED_SPQ_MODE_CB,        /* Client supplies a callback */
	QED_SPQ_MODE_EBLOCK,    /* QED should block until completion */
};

struct qed_spq_comp_cb {
	void	(*function)(struct qed_hwfn *p_hwfn,
			    void *cookie,
			    union event_ring_data *data,
			    u8 fw_return_code);
	void	*cookie;
};

/**
 * qed_eth_cqe_completion(): handles the completion of a
 *                           ramrod on the cqe ring.
 *
 * @p_hwfn: HW device data.
 * @cqe: CQE.
 *
 * Return: Int.
 */
int qed_eth_cqe_completion(struct qed_hwfn *p_hwfn,
			   struct eth_slow_path_rx_cqe *cqe);

 /*  QED Slow-hwfn queue interface */
union ramrod_data {
	struct pf_start_ramrod_data pf_start;
	struct pf_update_ramrod_data pf_update;
	struct rx_queue_start_ramrod_data rx_queue_start;
	struct rx_queue_update_ramrod_data rx_queue_update;
	struct rx_queue_stop_ramrod_data rx_queue_stop;
	struct tx_queue_start_ramrod_data tx_queue_start;
	struct tx_queue_stop_ramrod_data tx_queue_stop;
	struct vport_start_ramrod_data vport_start;
	struct vport_stop_ramrod_data vport_stop;
	struct rx_update_gft_filter_ramrod_data rx_update_gft;
	struct vport_update_ramrod_data vport_update;
	struct core_rx_start_ramrod_data core_rx_queue_start;
	struct core_rx_stop_ramrod_data core_rx_queue_stop;
	struct core_tx_start_ramrod_data core_tx_queue_start;
	struct core_tx_stop_ramrod_data core_tx_queue_stop;
	struct vport_filter_update_ramrod_data vport_filter_update;

	struct rdma_init_func_ramrod_data rdma_init_func;
	struct rdma_close_func_ramrod_data rdma_close_func;
	struct rdma_register_tid_ramrod_data rdma_register_tid;
	struct rdma_deregister_tid_ramrod_data rdma_deregister_tid;
	struct roce_create_qp_resp_ramrod_data roce_create_qp_resp;
	struct roce_create_qp_req_ramrod_data roce_create_qp_req;
	struct roce_modify_qp_resp_ramrod_data roce_modify_qp_resp;
	struct roce_modify_qp_req_ramrod_data roce_modify_qp_req;
	struct roce_query_qp_resp_ramrod_data roce_query_qp_resp;
	struct roce_query_qp_req_ramrod_data roce_query_qp_req;
	struct roce_destroy_qp_resp_ramrod_data roce_destroy_qp_resp;
	struct roce_destroy_qp_req_ramrod_data roce_destroy_qp_req;
	struct roce_init_func_ramrod_data roce_init_func;
	struct rdma_create_cq_ramrod_data rdma_create_cq;
	struct rdma_destroy_cq_ramrod_data rdma_destroy_cq;
	struct rdma_srq_create_ramrod_data rdma_create_srq;
	struct rdma_srq_destroy_ramrod_data rdma_destroy_srq;
	struct rdma_srq_modify_ramrod_data rdma_modify_srq;
	struct iwarp_create_qp_ramrod_data iwarp_create_qp;
	struct iwarp_tcp_offload_ramrod_data iwarp_tcp_offload;
	struct iwarp_mpa_offload_ramrod_data iwarp_mpa_offload;
	struct iwarp_modify_qp_ramrod_data iwarp_modify_qp;
	struct iwarp_init_func_ramrod_data iwarp_init_func;
	struct fcoe_init_ramrod_params fcoe_init;
	struct fcoe_conn_offload_ramrod_params fcoe_conn_ofld;
	struct fcoe_conn_terminate_ramrod_params fcoe_conn_terminate;
	struct fcoe_stat_ramrod_params fcoe_stat;

	struct iscsi_init_ramrod_params iscsi_init;
	struct iscsi_spe_conn_offload iscsi_conn_offload;
	struct iscsi_conn_update_ramrod_params iscsi_conn_update;
	struct iscsi_spe_conn_mac_update iscsi_conn_mac_update;
	struct iscsi_spe_conn_termination iscsi_conn_terminate;

	struct nvmetcp_init_ramrod_params nvmetcp_init;
	struct nvmetcp_spe_conn_offload nvmetcp_conn_offload;
	struct nvmetcp_conn_update_ramrod_params nvmetcp_conn_update;
	struct nvmetcp_spe_conn_termination nvmetcp_conn_terminate;

	struct vf_start_ramrod_data vf_start;
	struct vf_stop_ramrod_data vf_stop;
};

#define EQ_MAX_CREDIT   0xffffffff

enum spq_priority {
	QED_SPQ_PRIORITY_NORMAL,
	QED_SPQ_PRIORITY_HIGH,
};

union qed_spq_req_comp {
	struct qed_spq_comp_cb	cb;
	u64			*done_addr;
};

struct qed_spq_comp_done {
	unsigned int	done;
	u8		fw_return_code;
};

struct qed_spq_entry {
	struct list_head		list;

	u8				flags;

	/* HSI slow path element */
	struct slow_path_element	elem;

	union ramrod_data		ramrod;

	enum spq_priority		priority;

	/* pending queue for this entry */
	struct list_head		*queue;

	enum spq_mode			comp_mode;
	struct qed_spq_comp_cb		comp_cb;
	struct qed_spq_comp_done	comp_done; /* SPQ_MODE_EBLOCK */

	/* Posted entry for unlimited list entry in EBLOCK mode */
	struct qed_spq_entry		*post_ent;
};

struct qed_eq {
	struct qed_chain	chain;
	u8			eq_sb_index;    /* index within the SB */
	__le16			*p_fw_cons;     /* ptr to index value */
};

struct qed_consq {
	struct qed_chain chain;
};

typedef int (*qed_spq_async_comp_cb)(struct qed_hwfn *p_hwfn, u8 opcode,
				     __le16 echo, union event_ring_data *data,
				     u8 fw_return_code);

int
qed_spq_register_async_cb(struct qed_hwfn *p_hwfn,
			  enum protocol_type protocol_id,
			  qed_spq_async_comp_cb cb);

void
qed_spq_unregister_async_cb(struct qed_hwfn *p_hwfn,
			    enum protocol_type protocol_id);

struct qed_spq {
	spinlock_t		lock; /* SPQ lock */

	struct list_head	unlimited_pending;
	struct list_head	pending;
	struct list_head	completion_pending;
	struct list_head	free_pool;

	struct qed_chain	chain;

	/* allocated dma-able memory for spq entries (+ramrod data) */
	dma_addr_t		p_phys;
	struct qed_spq_entry	*p_virt;

#define SPQ_RING_SIZE \
	(CORE_SPQE_PAGE_SIZE_BYTES / sizeof(struct slow_path_element))

	/* Bitmap for handling out-of-order completions */
	DECLARE_BITMAP(p_comp_bitmap, SPQ_RING_SIZE);
	u8			comp_bitmap_idx;

	/* Statistics */
	u32			unlimited_pending_count;
	u32			normal_count;
	u32			high_count;
	u32			comp_sent_count;
	u32			comp_count;

	u32			cid;
	u32			db_addr_offset;
	struct core_db_data	db_data;
	qed_spq_async_comp_cb	async_comp_cb[MAX_PROTOCOL_TYPE];
};

/**
 * qed_spq_post(): Posts a Slow hwfn request to FW, or lacking that
 *                 Pends it to the future list.
 *
 * @p_hwfn: HW device data.
 * @p_ent: Ent.
 * @fw_return_code: Return code from firmware.
 *
 * Return: Int.
 */
int qed_spq_post(struct qed_hwfn *p_hwfn,
		 struct qed_spq_entry *p_ent,
		 u8 *fw_return_code);

/**
 * qed_spq_alloc(): Alloocates & initializes the SPQ and EQ.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Int.
 */
int qed_spq_alloc(struct qed_hwfn *p_hwfn);

/**
 * qed_spq_setup(): Reset the SPQ to its start state.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Void.
 */
void qed_spq_setup(struct qed_hwfn *p_hwfn);

/**
 * qed_spq_free(): Deallocates the given SPQ struct.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Void.
 */
void qed_spq_free(struct qed_hwfn *p_hwfn);

/**
 * qed_spq_get_entry(): Obtain an entrry from the spq
 *                      free pool list.
 *
 * @p_hwfn: HW device data.
 * @pp_ent: PP ENT.
 *
 * Return: Int.
 */
int
qed_spq_get_entry(struct qed_hwfn *p_hwfn,
		  struct qed_spq_entry **pp_ent);

/**
 * qed_spq_return_entry(): Return an entry to spq free pool list.
 *
 * @p_hwfn: HW device data.
 * @p_ent: P ENT.
 *
 * Return: Void.
 */
void qed_spq_return_entry(struct qed_hwfn *p_hwfn,
			  struct qed_spq_entry *p_ent);
/**
 * qed_eq_alloc(): Allocates & initializes an EQ struct.
 *
 * @p_hwfn: HW device data.
 * @num_elem: number of elements in the eq.
 *
 * Return: Int.
 */
int qed_eq_alloc(struct qed_hwfn *p_hwfn, u16 num_elem);

/**
 * qed_eq_setup(): Reset the EQ to its start state.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Void.
 */
void qed_eq_setup(struct qed_hwfn *p_hwfn);

/**
 * qed_eq_free(): deallocates the given EQ struct.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Void.
 */
void qed_eq_free(struct qed_hwfn *p_hwfn);

/**
 * qed_eq_prod_update(): update the FW with default EQ producer.
 *
 * @p_hwfn: HW device data.
 * @prod: Prod.
 *
 * Return: Void.
 */
void qed_eq_prod_update(struct qed_hwfn *p_hwfn,
			u16 prod);

/**
 * qed_eq_completion(): Completes currently pending EQ elements.
 *
 * @p_hwfn: HW device data.
 * @cookie: Cookie.
 *
 * Return: Int.
 */
int qed_eq_completion(struct qed_hwfn *p_hwfn,
		      void *cookie);

/**
 * qed_spq_completion(): Completes a single event.
 *
 * @p_hwfn: HW device data.
 * @echo: echo value from cookie (used for determining completion).
 * @fw_return_code: FW return code.
 * @p_data: data from cookie (used in callback function if applicable).
 *
 * Return: Int.
 */
int qed_spq_completion(struct qed_hwfn *p_hwfn,
		       __le16 echo,
		       u8 fw_return_code,
		       union event_ring_data *p_data);

/**
 * qed_spq_get_cid(): Given p_hwfn, return cid for the hwfn's SPQ.
 *
 * @p_hwfn: HW device data.
 *
 * Return: u32 - SPQ CID.
 */
u32 qed_spq_get_cid(struct qed_hwfn *p_hwfn);

/**
 * qed_consq_alloc(): Allocates & initializes an ConsQ struct.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Int.
 */
int qed_consq_alloc(struct qed_hwfn *p_hwfn);

/**
 * qed_consq_setup(): Reset the ConsQ to its start state.
 *
 * @p_hwfn: HW device data.
 *
 * Return Void.
 */
void qed_consq_setup(struct qed_hwfn *p_hwfn);

/**
 * qed_consq_free(): deallocates the given ConsQ struct.
 *
 * @p_hwfn: HW device data.
 *
 * Return Void.
 */
void qed_consq_free(struct qed_hwfn *p_hwfn);
int qed_spq_pend_post(struct qed_hwfn *p_hwfn);

/* Slow-hwfn low-level commands (Ramrods) function definitions. */

#define QED_SP_EQ_COMPLETION  0x01
#define QED_SP_CQE_COMPLETION 0x02

struct qed_sp_init_data {
	u32			cid;
	u16			opaque_fid;

	/* Information regarding operation upon sending & completion */
	enum spq_mode		comp_mode;
	struct qed_spq_comp_cb *p_comp_data;
};

/**
 * qed_sp_destroy_request(): Returns a SPQ entry to the pool / frees the
 *                           entry if allocated. Should be called on in error
 *                           flows after initializing the SPQ entry
 *                           and before posting it.
 *
 * @p_hwfn: HW device data.
 * @p_ent: Ent.
 *
 * Return: Void.
 */
void qed_sp_destroy_request(struct qed_hwfn *p_hwfn,
			    struct qed_spq_entry *p_ent);

int qed_sp_init_request(struct qed_hwfn *p_hwfn,
			struct qed_spq_entry **pp_ent,
			u8 cmd,
			u8 protocol,
			struct qed_sp_init_data *p_data);

/**
 * qed_sp_pf_start(): PF Function Start Ramrod.
 *
 * @p_hwfn: HW device data.
 * @p_ptt: P_ptt.
 * @p_tunn: P_tunn.
 * @allow_npar_tx_switch: Allow NPAR TX Switch.
 *
 * Return: Int.
 *
 * This ramrod is sent to initialize a physical function (PF). It will
 * configure the function related parameters and write its completion to the
 * event ring specified in the parameters.
 *
 * Ramrods complete on the common event ring for the PF. This ring is
 * allocated by the driver on host memory and its parameters are written
 * to the internal RAM of the UStorm by the Function Start Ramrod.
 *
 */

int qed_sp_pf_start(struct qed_hwfn *p_hwfn,
		    struct qed_ptt *p_ptt,
		    struct qed_tunnel_info *p_tunn,
		    bool allow_npar_tx_switch);

/**
 * qed_sp_pf_update(): PF Function Update Ramrod.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Int.
 *
 * This ramrod updates function-related parameters. Every parameter can be
 * updated independently, according to configuration flags.
 */

int qed_sp_pf_update(struct qed_hwfn *p_hwfn);

/**
 * qed_sp_pf_update_stag(): Update firmware of new outer tag.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Int.
 */
int qed_sp_pf_update_stag(struct qed_hwfn *p_hwfn);

/**
 * qed_sp_pf_update_ufp(): PF ufp update Ramrod.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Int.
 */
int qed_sp_pf_update_ufp(struct qed_hwfn *p_hwfn);

int qed_sp_pf_stop(struct qed_hwfn *p_hwfn);

int qed_sp_pf_update_tunn_cfg(struct qed_hwfn *p_hwfn,
			      struct qed_ptt *p_ptt,
			      struct qed_tunnel_info *p_tunn,
			      enum spq_mode comp_mode,
			      struct qed_spq_comp_cb *p_comp_data);
/**
 * qed_sp_heartbeat_ramrod(): Send empty Ramrod.
 *
 * @p_hwfn: HW device data.
 *
 * Return: Int.
 */

int qed_sp_heartbeat_ramrod(struct qed_hwfn *p_hwfn);

#endif
