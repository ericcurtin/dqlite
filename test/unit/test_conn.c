#include <raft.h>
#include <raft/io_uv.h>

#include "../lib/heap.h"
#include "../lib/logger.h"
#include "../lib/options.h"
#include "../lib/raft.h"
#include "../lib/registry.h"
#include "../lib/replication.h"
#include "../lib/runner.h"
#include "../lib/socket.h"
#include "../lib/sqlite.h"
#include "../lib/vfs.h"

#include "../../src/client.h"
#include "../../src/conn.h"
#include "../../src/gateway.h"
#include "../../src/lib/transport.h"
#include "../../src/transport.h"

TEST_MODULE(conn);

/******************************************************************************
 *
 * Fixture
 *
 ******************************************************************************/

#define FIXTURE                          \
	FIXTURE_LOGGER;                  \
	FIXTURE_VFS;                     \
	FIXTURE_OPTIONS;                 \
	FIXTURE_REGISTRY;                \
	FIXTURE_RAFT;                    \
	FIXTURE_REPLICATION;             \
	struct test_socket_pair sockets; \
	struct conn conn;                \
	struct client client

#define SETUP                                                         \
	int rv;                                                       \
	SETUP_HEAP;                                                   \
	SETUP_SQLITE;                                                 \
	SETUP_LOGGER;                                                 \
	SETUP_VFS;                                                    \
	SETUP_OPTIONS;                                                \
	SETUP_REGISTRY;                                               \
	SETUP_RAFT;                                                   \
	SETUP_REPLICATION;                                            \
	RAFT_BOOTSTRAP;                                               \
	RAFT_START;                                                   \
	test_socket_pair_setup(params, &f->sockets);                  \
	rv = conn__start(&f->conn, &f->logger, &f->loop, &f->options, \
			 &f->registry, &f->raft, f->sockets.server,   \
			 &f->raft_transport, NULL);                   \
	munit_assert_int(rv, ==, 0);                                  \
	client__init(&f->client, f->sockets.client)

#define TEAR_DOWN                                \
	client__close(&f->client);               \
	conn__stop(&f->conn);                    \
	f->sockets.client_disconnected = true;   \
	f->sockets.server_disconnected = true;   \
	test_socket_pair_tear_down(&f->sockets); \
	TEAR_DOWN_REPLICATION;                   \
	TEAR_DOWN_RAFT;                          \
	TEAR_DOWN_REGISTRY;                      \
	TEAR_DOWN_OPTIONS;                       \
	TEAR_DOWN_VFS;                           \
	TEAR_DOWN_LOGGER;                        \
	TEAR_DOWN_SQLITE;                        \
	TEAR_DOWN_HEAP

/******************************************************************************
 *
 * Helper macros.
 *
 ******************************************************************************/

/* Send the initial client handshake. */
#define HANDSHAKE                                         \
	{                                                 \
		int rv2;                                  \
		rv2 = client__send_handshake(&f->client); \
		munit_assert_int(rv2, ==, 0);             \
		test_uv_run(&f->loop, 1);                 \
	}

/* Open a test database. */
#define OPEN                                                 \
	{                                                    \
		int rv2;                                     \
		rv2 = client__send_open(&f->client, "test"); \
		munit_assert_int(rv2, ==, 0);                \
		test_uv_run(&f->loop, 2);                    \
		rv2 = client__recv_db(&f->client);           \
	}

/******************************************************************************
 *
 * Assertions.
 *
 ******************************************************************************/

/******************************************************************************
 *
 * Handle the handshake
 *
 ******************************************************************************/

TEST_SUITE(handshake);

struct handshake_fixture
{
	FIXTURE;
};

TEST_SETUP(handshake)
{
	struct handshake_fixture *f = munit_malloc(sizeof *f);
	SETUP;
	return f;
}

TEST_TEAR_DOWN(handshake)
{
	struct handshake_fixture *f = data;
	TEAR_DOWN;
	free(f);
}

TEST_CASE(handshake, success, NULL)
{
	struct handshake_fixture *f = data;
	(void)params;
	HANDSHAKE;
	return MUNIT_OK;
}

/******************************************************************************
 *
 * Handle an open request
 *
 ******************************************************************************/

TEST_SUITE(open);

struct open_fixture
{
	FIXTURE;
};

TEST_SETUP(open)
{
	struct open_fixture *f = munit_malloc(sizeof *f);
	SETUP;
	HANDSHAKE;
	return f;
}

TEST_TEAR_DOWN(open)
{
	struct open_fixture *f = data;
	TEAR_DOWN;
	free(f);
}

TEST_CASE(open, success, NULL)
{
	struct open_fixture *f = data;
	(void)params;
	OPEN;
	return MUNIT_OK;
}