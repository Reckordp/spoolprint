#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <ruby.h>

#define PANJANG_NAMA 1024;
// #define PANJANG_JUKIR_AWAL 2;

typedef struct {
  SOCKET hantar;
  struct sockaddr_in addr;
  FD_SET jukir;
  SOCKET* akhirSentuh;
  SOCKET* bergulir;
  // size_t panjang_lantai;
} server_ket, *serv_ket;

void server_free(void* keterangan) {
  serv_ket serv = keterangan;
	closesocket(serv->hantar);
  free(serv);
}

size_t server_size(const void* keterangan) {
	return sizeof(server_ket);
}

static const rb_data_type_t server_type = {
	.wrap_struct_name = "server", 
	.function = {
		.dmark = NULL, 
		.dfree = server_free, 
		.dsize = server_size
	}, 
	.data = NULL, 
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

VALUE server_alloc(VALUE self) {
  char nama[PANJANG_NAMA];
  serv_ket keterangan;
  struct hostent* host;

  gethostname(nama, PANJANG_NAMA);
  host = gethostbyname(nama);

  keterangan = malloc(sizeof(server_ket));
  memset(keterangan, 0, sizeof(server_ket));
  keterangan->hantar = socket(AF_INET, SOCK_STREAM, 0);
  keterangan->addr.sin_family = AF_INET;
  keterangan->addr.sin_addr.s_addr = host->h_addr->s_addr;
  keterangan->jukir = malloc(sizeof(FD_SET) * PANJANG_JUKIR_AWAL);
  keterangan->akhirSentuh = keterangan->jukir->fd_array;
  keterangan->bergulir = keterangan->akhirSentuh;
  // keterangan->panjang_pelanggan = PANJANG_JUKIR_AWAL;
  return TypedData_Wrap_Struct(self, &spoolprint_type, keterangan);
}

#define MUAT_DATA(var) TypedData_Get_Struct(self, server_ket, \
  &server_type, var)

VALUE server_init(VALUE self, VALUE port) {
  serv_ket ket;
  u_long mode = 1;

  MUAT_DATA(ket);
  ket->addr.sin_port = htons(FIX2INT(port));
  bind(ket->hantar, (struct sockaddr*)&ket->addr, sizeof(struct sockaddr_in));
  listen(ket->hantar, 20);
  ioctlsocket(ket->hantar, FIONBIO, &mode);
  return Qnil;
}

VALUE server_recvpoll(VALUE self) {
  VALUE resp;
  serv_ket ket;
  struct sockaddr_in pelanggan;
  struct timeval tm;
  SOCKET pelangganFd;
  byte flag;
  int sisa;
  FD_SET siap_dibaca;

  MUAT_DATA(ket);
  rb_need_block();

  memcpy(&siap_dibaca, &ket->jukir, sizeof(FD_SET));
  timerclear(&tm);
  sisa = select(siap_dibaca.fd_count, &siap_dibaca.fd_array, NULL, NULL, &tm);
  for (size_t i = 0; i < sisa; i++) {
    ket->bergulir = siap_dibaca.fd_array[i];
    read(ket->bergulir, &flag, 1);
    if (flag != 0x9f) {
      resp = rb_yield(rb_str_new2(flag));
      write(ket->bergulir, RSTRING_PTR(resp), RSTRING_LEN(resp));
    } else {
      closesocket(ket->bergulir);
      *ket->bergulir = *ket->akhirSentuh;
      ket->akhirSentuh--;
      ket->jukir.fd_count--;
    }
  }

  memset(&pelanggan, 0, sizeof(pelanggan));
  pelangganFd = accept(ket->hantar, &pelanggan, sizeof(pelanggan));
  if (pelangganFd != -1) {
    ket->akhirSentuh++;
    *ket->akhirSentuh = pelangganFd;
    ket->jukir.fd_count++;
  }
}

void Init_server(VALUE wrap) {
  VALUE cServer = rb_define_class_under(wrap, "Server", rb_cObject);
  rb_define_alloc_func(cServer, server_alloc);
  rb_define_method(cServer, "initialize", server_init, 1);
  rb_define_method(cServer, "receiver_poll", server_recvpoll, 0);
  // atexit(Finalize_server);
}

void Finalize_server() {
}