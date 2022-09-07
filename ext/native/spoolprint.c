#include <ruby.h>
#include <stdlib.h>

#define ASAL_NAMA_DOC "Resi Toko"
#define PANJANG_TEKS 4096
#define NAMA_JOB_MAX 255

extern int spoolprint_dalam(char*, void*, size_t);

typedef struct {
	char teks[PANJANG_TEKS];
	byte panjangNamaJob;
	char* tumpuan;
	void* sambungan;
} spoolprint_keterangan, *sp_ket;

void spoolprint_free(void* keterangan) {
	sp_ket sambungan;
	while (keterangan != NULL) {
		sambungan = ((sp_ket)keterangan)->sambungan;
		free(keterangan);
		keterangan = sambungan;
	}
}

size_t spoolprint_size(const void* keterangan) {
	return sizeof(spoolprint_keterangan);
}

static const rb_data_type_t spoolprint_type = {
	.wrap_struct_name = "spoolprint", 
	.function = {
		.dmark = NULL, 
		.dfree = spoolprint_free, 
		.dsize = spoolprint_size
	}, 
	.data = NULL, 
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

#define MUAT_DATA(var) TypedData_Get_Struct(self, \
	spoolprint_keterangan, &spoolprint_type, var)

VALUE spoolprint_alloc(VALUE self) {
	sp_ket keterangan = malloc(sizeof(spoolprint_keterangan));
	memset(keterangan, 0, sizeof(spoolprint_keterangan));
	keterangan->tumpuan = keterangan->teks;
	return TypedData_Wrap_Struct(self, &spoolprint_type, keterangan);
}

VALUE spoolprintInit(int argc, VALUE* argv, VALUE self) {
	VALUE namaDoc;
	sp_ket ket;
	char* nama = "\0";
	size_t tempuh = 0;
	MUAT_DATA(ket);

	rb_scan_args(argc, argv, "01", &namaDoc);
	if (argc > 0) {
		Check_Type(namaDoc, RUBY_T_STRING);
		nama = RSTRING_PTR(namaDoc);
	}

	while(*nama != 0 && ++tempuh < NAMA_JOB_MAX) {
		*ket->tumpuan++ = *nama++;
	}
	ket->panjangNamaJob = ket->tumpuan - ket->teks;
	*ket->tumpuan++ = 0;
}

VALUE spoolprintCetak(VALUE self) {
	char *nDoc = ASAL_NAMA_DOC, *dicetak;
	size_t panjang;
	sp_ket ket;
	MUAT_DATA(ket);

	if (ket->panjangNamaJob > 0) {
		nDoc = ket->teks;
		dicetak = ket->teks + ket->panjangNamaJob + 2;
	} else {
		dicetak = ket->teks + 1;
	}
	
	if (*dicetak != 0x1b || *(dicetak + 1) != 0x40) return Qnil;
	panjang = ket->tumpuan - dicetak;
	return INT2FIX(spoolprint_dalam(nDoc, dicetak, panjang));
}

VALUE spoolprintRaw(VALUE self, VALUE raw) {
	char* jukir;
	size_t panjang;
	sp_ket ket;

	MUAT_DATA(ket);
	Check_Type(raw, RUBY_T_STRING);
	jukir = RSTRING_PTR(raw);
	panjang = RSTRING_LEN(raw);

	if (ket->tumpuan - ket->teks + panjang < PANJANG_TEKS) {
		while (panjang--) *ket->tumpuan++ = *jukir++;
		return Qtrue;
	} else {
		return Qfalse;
	}
}

void Init_spoolprint() {
	VALUE mSpoolprint = rb_define_module("Spoolprint");
	VALUE cSpoolPrint = rb_define_class_under(mSpoolprint, "SpoolPrint", rb_cObject);
	rb_define_alloc_func(cSpoolPrint, spoolprint_alloc);
	rb_define_method(cSpoolPrint, "initialize", spoolprintInit, -1);
	rb_define_method(cSpoolPrint, "taruh_raw", spoolprintRaw, 1);
	rb_define_method(cSpoolPrint, "cetak", spoolprintCetak, 0);
}