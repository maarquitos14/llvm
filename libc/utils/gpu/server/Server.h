//===-- Shared memory RPC server instantiation ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_UTILS_GPU_SERVER_RPC_SERVER_H
#define LLVM_LIBC_UTILS_GPU_SERVER_RPC_SERVER_H

#include <stdint.h>

#include "llvm-libc-types/rpc_opcodes_t.h"

#ifdef __cplusplus
extern "C" {
#endif

/// The maxium number of ports that can be opened for any server.
const uint64_t RPC_MAXIMUM_PORT_COUNT = 64;

/// status codes.
typedef enum {
  RPC_STATUS_SUCCESS = 0x0,
  RPC_STATUS_CONTINUE = 0x1,
  RPC_STATUS_ERROR = 0x1000,
  RPC_STATUS_OUT_OF_RANGE = 0x1001,
  RPC_STATUS_UNHANDLED_OPCODE = 0x1002,
  RPC_STATUS_INVALID_LANE_SIZE = 0x1003,
} rpc_status_t;

/// A struct containing an opaque handle to an RPC port. This is what allows the
/// server to communicate with the client.
typedef struct rpc_port_s {
  uint64_t handle;
  uint32_t lane_size;
} rpc_port_t;

/// A fixed-size buffer containing the payload sent from the client.
typedef struct rpc_buffer_s {
  uint64_t data[8];
} rpc_buffer_t;

/// A function used to allocate \p bytes for use by the RPC server and client.
/// The memory should support asynchronous and atomic access from both the
/// client and server.
typedef void *(*rpc_alloc_ty)(uint64_t size, void *data);

/// A function used to free the \p ptr previously allocated.
typedef void (*rpc_free_ty)(void *ptr, void *data);

/// A callback function provided with a \p port to communicate with the RPC
/// client. This will be called by the server to handle an opcode.
typedef void (*rpc_opcode_callback_ty)(rpc_port_t port, void *data);

/// A callback function to use the port to receive or send a \p buffer.
typedef void (*rpc_port_callback_ty)(rpc_buffer_t *buffer, void *data);

/// Initialize the rpc library for general use on \p num_devices.
rpc_status_t rpc_init(uint32_t num_devices);

/// Shut down the rpc interface.
rpc_status_t rpc_shutdown(void);

/// Initialize the server for a given device.
rpc_status_t rpc_server_init(uint32_t device_id, uint64_t num_ports,
                             uint32_t lane_size, rpc_alloc_ty alloc,
                             void *data);

/// Shut down the server for a given device.
rpc_status_t rpc_server_shutdown(uint32_t device_id, rpc_free_ty dealloc,
                                 void *data);

/// Queries the RPC clients at least once and performs server-side work if there
/// are any active requests. Runs until all work on the server is completed.
rpc_status_t rpc_handle_server(uint32_t device_id);

/// Register a callback to handle an opcode from the RPC client. The associated
/// data must remain accessible as long as the user intends to handle the server
/// with this callback.
rpc_status_t rpc_register_callback(uint32_t device_id, rpc_opcode_t opcode,
                                   rpc_opcode_callback_ty callback, void *data);

/// Obtain a pointer to the memory buffer used to run the RPC client and server.
void *rpc_get_buffer(uint32_t device_id);

/// Use the \p port to receive and send a buffer using the \p callback.
void rpc_recv_and_send(rpc_port_t port, rpc_port_callback_ty callback,
                       void *data);

#ifdef __cplusplus
}
#endif

#endif
