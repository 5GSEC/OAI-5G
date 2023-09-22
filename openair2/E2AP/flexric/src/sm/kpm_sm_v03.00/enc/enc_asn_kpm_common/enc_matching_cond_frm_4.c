#include <assert.h>
#include <stdbool.h>

#include "enc_matching_cond_frm_4.h"
#include "enc_test_info.h"



MatchingUeCondPerSubItem_t * kpm_enc_matching_cond_frm_4_asn(const matching_condition_format_4_lst_t * matching_cond)
{

    MatchingUeCondPerSubItem_t * matching_cond_asn = calloc(1, sizeof(MatchingUeCondPerSubItem_t));
    assert(matching_cond_asn != NULL && "Memory exhausted");


    matching_cond_asn->testCondInfo = kpm_enc_test_info_asn(&matching_cond->test_info_lst);


    if (matching_cond->logical_OR != NULL) {
        assert(false && "Encryption of logical OR not yet implemented");
    }


    return matching_cond_asn;
}
