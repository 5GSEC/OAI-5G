#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h> // htonl

#include "../../../../util/conversions.h"


#include "enc_ric_ind_hdr_frm_1.h"

// #define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))

// uint64_t htonll(uint64_t x)
// {
//   return (uint64_t)((uint64_t)htonl((x) & 0xFFFFFFFFLL) << 32) | (uint32_t)htonl(x);
// }

uint64_t htonll(uint64_t x)
{
    uint32_t x1 = (uint32_t)((x & 0xFFFFFFFF00000000LL) >> 32);
    uint32_t x2 = (uint32_t)(x & 0xFFFFFFFFLL);
    x1 = htonl(x1);
    x2 = htonl(x2);
    uint64_t v = ((uint64_t)x1) << 32 | x2;

    return v;
}

E2SM_KPM_IndicationHeader_Format1_t * kpm_enc_ind_hdr_frm_1_asn(const kpm_ric_ind_hdr_format_1_t * ind_hdr)
{
    E2SM_KPM_IndicationHeader_Format1_t * ind_hdr_asn = calloc(1, sizeof(E2SM_KPM_IndicationHeader_Format1_t));
    assert(ind_hdr_asn != NULL && "Memory exhausted");


    uint64_t ts = htonll(ind_hdr->collectStartTime);
    ind_hdr_asn->colletStartTime = int_64_to_octet_string(ts);


    int ret;

    if (ind_hdr->fileformat_version != NULL){
        ind_hdr_asn->fileFormatversion = calloc(1, sizeof(*ind_hdr_asn->fileFormatversion));

        const size_t len = ind_hdr->fileformat_version->len;
        ret = OCTET_STRING_fromBuf(ind_hdr_asn->fileFormatversion, (char *)(ind_hdr->fileformat_version->buf), len);
        assert(ret == 0);
    }

    if (ind_hdr->sender_name != NULL){
        ind_hdr_asn->senderName = calloc(1, sizeof(*ind_hdr_asn->senderName));

        const size_t len = ind_hdr->sender_name->len;
        ret = OCTET_STRING_fromBuf(ind_hdr_asn->senderName, (char *)(ind_hdr->sender_name->buf), len);
        assert(ret == 0);
    }

    if (ind_hdr->sender_type != NULL){
        ind_hdr_asn->senderType = calloc(1, sizeof(*ind_hdr_asn->senderType));

        const size_t len = ind_hdr->sender_type->len;
        ret = OCTET_STRING_fromBuf(ind_hdr_asn->senderType, (char *)(ind_hdr->sender_type->buf), len);
        assert(ret == 0);
    }

	if (ind_hdr->vendor_name != NULL) {
        ind_hdr_asn->vendorName = calloc(1, sizeof(*ind_hdr_asn->vendorName));

        const size_t len = ind_hdr->vendor_name->len;
        ret = OCTET_STRING_fromBuf(ind_hdr_asn->vendorName, (char *)(ind_hdr->vendor_name->buf), len);
        assert(ret == 0);
    }

    return ind_hdr_asn;
}
