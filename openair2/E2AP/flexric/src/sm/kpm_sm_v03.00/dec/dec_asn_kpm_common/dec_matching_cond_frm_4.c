#include <assert.h>

#include "dec_matching_cond_frm_4.h"
#include "dec_test_info.h"

matching_condition_format_4_lst_t kpm_dec_matching_cond_frm_4_asn(const MatchingUeCondPerSubItem_t * matching_cond_asn)
{
    matching_condition_format_4_lst_t matching_cond={0};

    matching_cond.test_info_lst = kpm_dec_test_info_asn(&matching_cond_asn->testCondInfo);

    // logical OR not yet implemented in ASN.1

    return matching_cond;
}
