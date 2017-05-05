 /*
 * Copyright (c) 2017, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Copyright (c) 2017, Florida State University. Contributions from
 * the Computer Architecture and Systems Research Laboratory (CASTL)
 * at the Department of Computer Science.
 * Written by
 * 	Teng Wang tw15g@my.fsu.edu
 * 	Adam Moody moody20@llnl.gov
 * 	Weikuan Yu wyu3@fsu.edu
 * 	Kento Sato kento@llnl.gov
 * 	Kathryn Mohror. kathryn@llnl.gov
 * 	LLNL-CODE-728877.
 * All rights reserved.
 *
 * This file is part of BurstFS For details, see https://github.com/llnl/burstfs.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef BURSTFS_GLOBAL_H
#define BURSTFS_GLOBAL_H
#include <pthread.h>
#include <stdlib.h>
#include "burstfs_const.h"
#include "arraylist.h"

typedef enum {
	COMM_MOUNT, /*the list of addrs: appid, size of buffer, offset of data section, metadata section*/
	COMM_META,
	COMM_READ,
	COMM_UNMOUNT,
	COMM_DIGEST,
	COMM_SYNC_DEL,
}cmd_lst_t;

typedef enum {
	XFER_COMM_DATA,
	XFER_COMM_EXIT,
}service_cmd_lst_t;

typedef enum {
	ACK_SUCCESS,
	ACK_FAIL,
}ack_status_t;

typedef struct {
	int dest_app_id;
	int dest_client_id; /*note: the id is the app-side rank on the remote node*/
	long dest_offset;
	int dest_delegator_rank;
	long length;
	int src_delegator_rank;
	int src_cli_id;
	int src_app_id;
	int src_fid;
	long src_offset;
	int src_thrd;
	int src_dbg_rank;
	int arrival_time;
} send_msg_t;

typedef struct {
	long src_fid;
	long src_offset;
	long length;
}recv_msg_t;


typedef struct {
	int num;
	send_msg_t msg_meta[MAX_META_PER_SEND];
}msg_meta_t;

typedef struct {
	long superblock_sz;
	long meta_offset;
	long meta_size;
	long fmeta_offset;
	long fmeta_size;
	long data_offset;
	long data_size;
	int req_buf_sz;
	int recv_buf_sz;
	int num_procs_per_node;
	int client_ranks[MAX_NUM_CLIENTS];
	int thrd_idxs[MAX_NUM_CLIENTS];
	int shm_superblock_fds[MAX_NUM_CLIENTS];
	int shm_req_fds[MAX_NUM_CLIENTS];
	int shm_recv_fds[MAX_NUM_CLIENTS];
	char *shm_superblocks[MAX_NUM_CLIENTS];
	char *shm_req_bufs[MAX_NUM_CLIENTS];
	char *shm_recv_bufs[MAX_NUM_CLIENTS];
	int spill_log_fds[MAX_NUM_CLIENTS];
	int spill_index_log_fds[MAX_NUM_CLIENTS];
	int dbg_ranks[MAX_NUM_CLIENTS];
	char external_spill_dir[MAX_PATH_LEN];
	char recv_buf_name[MAX_NUM_CLIENTS][MAX_PATH_LEN];
	char req_buf_name[MAX_NUM_CLIENTS][MAX_PATH_LEN];
	char super_buf_name[MAX_NUM_CLIENTS][MAX_PATH_LEN];
	char spill_log_name[MAX_NUM_CLIENTS][MAX_PATH_LEN];
	char spill_index_log_name[MAX_NUM_CLIENTS][MAX_PATH_LEN];
}app_config_t;

typedef struct {
	int req_cnt; /*number of requests to this delegator*/
	int del_id; /*rank of delegator*/
}per_del_stat_t;

typedef struct {
	per_del_stat_t *req_stat;
	int del_cnt; /*number of unique delegators*/
}del_req_stat_t;

typedef struct {
	pthread_t thrd;
	pthread_cond_t	thrd_cond;
	pthread_mutex_t thrd_lock;

	int has_waiting_delegator;
	int has_waiting_dispatcher;
	/* a list of read requests to
	 * be sent to each
	 * delegator*/
	msg_meta_t *del_req_set;

	/* statistics of read requests
	 * to be sent to each delegator*/
	del_req_stat_t *del_req_stat;
	char del_req_msg_buf[REQ_BUF_LEN];
	char del_recv_msg_buf[RECV_BUF_CNT][RECV_BUF_LEN];
	int exit_flag;
}thrd_ctrl_t;

typedef struct {
	int app_id;
	int sock_id;
}cli_signature_t;

typedef int fattr_key_t;

typedef struct {
  char fname[ULFS_MAX_FILENAME];
  struct stat file_attr;
}fattr_val_t;

typedef struct {
	int fid;
	int gfid;
	char filename[ULFS_MAX_FILENAME];
	struct stat file_attr;
}burstfs_file_attr_t;
extern arraylist_t *app_config_list;
extern arraylist_t *thrd_list;
int invert_sock_ids[MAX_NUM_CLIENTS];
extern pthread_t data_thrd;
extern int glb_rank, glb_size;
extern int local_rank_idx;
extern int *local_rank_lst;
extern int local_rank_cnt;
extern long max_recs_per_slice;
#endif