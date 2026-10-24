#include "ipc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
static unsigned calls;
static error_t echo(uint32_t key, void *payload, uint32_t *length)
{
  ++calls;
  if(key == 99) { *length = 0; return E_PARAM; }
  if(*length) ((unsigned char *)payload)[0] ^= 0x55;
  return E_OK;
}
static ipc_value_t stream;
static void *producer(void *unused)
{
  (void)unused;
  for(unsigned i = 1; i <= 200000; ++i)
    while(ipc_value_publish(&stream, (float)i, i ^ 0x1234u, i) == E_AGAIN) { }
  return NULL;
}
static void *consumer(void *unused)
{
  (void)unused;
  for(unsigned i = 1; i <= 200000; ++i) {
    float value; uint32_t valid, timestamp;
    while(ipc_value_consume(&stream, &value, &valid, &timestamp) == E_AGAIN) { }
    assert(value == (float)i && valid == (i ^ 0x1234u) && timestamp == i);
  }
  return NULL;
}
static unsigned rpc_clients_done;
static void *rpc_client(void *arg)
{
  uint32_t key = 1000u + (uint32_t)(uintptr_t)arg;
  for(unsigned i = 0; i < 2000; ++i) {
    unsigned char input[4] = {(unsigned char)i, (unsigned char)(i>>8), 123, 45};
    unsigned char output[4]; uint32_t n = sizeof(output);
    error_t err;
    do { err = ipc_rpc_call(0,key,input,sizeof(input),output,&n); } while(err == E_AGAIN);
    assert(err == E_OK && n == sizeof(output));
    input[0] ^= 0x55;
    assert(!memcmp(input,output,sizeof(input)));
  }
  __atomic_add_fetch(&rpc_clients_done,1u,__ATOMIC_RELEASE);
  return NULL;
}
static void *rpc_server(void *unused)
{
  (void)unused;
  while(__atomic_load_n(&rpc_clients_done,__ATOMIC_ACQUIRE) != 2u) ipc_rpc_service(1,echo);
  return NULL;
}
int main(void)
{
  ipc_initialize();
  unsigned char input[] = {1,2,3}, output[3]; uint32_t n = sizeof(output);
  assert(ipc_rpc_call(0,1,input,3,output,&n) == E_AGAIN);
  assert(g_ipc.requests[0][0].state == IPC_FREE); /* peer not ready */
  ipc_store(&g_ipc.cm4_ready,1);
  assert(ipc_rpc_call(0,1,input,3,output,&n) == E_AGAIN);
  input[0] = 42; /* arguments were copied */
  assert(ipc_rpc_call(0,1,input,3,output,&n) == E_AGAIN);
  assert(ipc_rpc_call(0,2,input,3,NULL,NULL) == E_AGAIN);
  assert(ipc_rpc_call(0,3,input,3,NULL,NULL) == E_AGAIN); /* full */
  ipc_rpc_service(1,echo);
  assert(calls == 2);
  assert(ipc_rpc_call(0,1,input,3,output,&n) == E_OK);
  assert(output[0] == (1 ^ 0x55) && n == 3);
  assert(ipc_rpc_call(0,2,input,3,NULL,NULL) == E_OK);
  assert(ipc_rpc_call(0,99,NULL,0,NULL,NULL) == E_AGAIN);
  ipc_rpc_service(1,echo);
  assert(ipc_rpc_call(0,99,NULL,0,NULL,NULL) == E_PARAM);
  assert(ipc_rpc_call(0,5,input,IPC_RPC_BYTES+1,NULL,NULL) == E_PARAM);
  assert(ipc_rpc_call(0,5,NULL,1,NULL,NULL) == E_PARAM);
  assert(ipc_rpc_call(2,5,NULL,0,NULL,NULL) == E_PARAM);
  assert(ipc_rpc_call(1,4,input,3,output,&n) == E_AGAIN);
  ipc_rpc_service(0,echo);
  n=1;
  assert(ipc_rpc_call(1,4,input,3,output,&n) == E_OVERFLOW);
  assert(n == 3);
  for(unsigned i = 0; i < 10000; ++i) {
    assert(ipc_rpc_call(i%2,7,input,3,NULL,NULL) == E_AGAIN);
    ipc_rpc_service(1-i%2,echo);
    assert(ipc_rpc_call(i%2,7,input,3,NULL,NULL) == E_OK);
  }
  static unsigned char large[IPC_RPC_BYTES], received[IPC_RPC_BYTES];
  for(unsigned i = 0; i < IPC_RPC_BYTES; ++i) large[i] = (unsigned char)i;
  n = sizeof(received);
  assert(ipc_rpc_call(0,123,large,sizeof(large),received,&n) == E_AGAIN);
  ipc_rpc_service(1,echo);
  assert(ipc_rpc_call(0,123,large,sizeof(large),received,&n) == E_OK);
  large[0] ^= 0x55;
  assert(n == sizeof(large) && !memcmp(large,received,sizeof(large)));
  pthread_t p,c;
  assert(!pthread_create(&p,NULL,producer,NULL));
  assert(!pthread_create(&c,NULL,consumer,NULL));
  pthread_join(p,NULL);pthread_join(c,NULL);
  pthread_t client1,client2,server;
  assert(!pthread_create(&server,NULL,rpc_server,NULL));
  assert(!pthread_create(&client1,NULL,rpc_client,(void *)(uintptr_t)1));
  assert(!pthread_create(&client2,NULL,rpc_client,(void *)(uintptr_t)2));
  pthread_join(client1,NULL);pthread_join(client2,NULL);pthread_join(server,NULL);
  printf("IPC tests passed: bidirectional RPC, full queues, copied payloads, errors, 10000 reuses, 4000 concurrent RPC calls, 200000 concurrent snapshots; shared bytes=%zu\n",sizeof(g_ipc));
}
