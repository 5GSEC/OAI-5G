#include <assert.h>

#include "dec_matching_cond_frm_3.h"
#include "dec_label_info.h"
#include "dec_test_info.h"


matching_condition_format_3_lst_t kpm_dec_matching_cond_asn(const MatchingCondItem_t * matching_cond_asn)
{
    matching_condition_format_3_lst_t matching_cond={0};

    if (matching_cond_asn->matchingCondChoice.present == MatchingCondItem_Choice_PR_measLabel)
    {
        matching_cond.cond_type = LABEL_INFO;
        matching_cond.label_info_lst = kpm_dec_label_info_asn(matching_cond_asn->matchingCondChoice.choice.measLabel);
    }
    else if (matching_cond_asn->matchingCondChoice.present == MatchingCondItem_Choice_PR_testCondInfo)
    {
        matching_cond.cond_type = TEST_INFO;
        matching_cond.test_info_lst = kpm_dec_test_info_asn(matching_cond_asn->matchingCondChoice.choice.testCondInfo);
    }
    else {
        assert(false && "Condition type out of the range");
    }
    

    if (matching_cond_asn->logicalOR != NULL)
    {
        assert(*matching_cond_asn->logicalOR == 0 && "has only one value (true)");
        matching_cond.logical_OR = calloc(1, sizeof(enum_value_e));
        assert(matching_cond.logical_OR != NULL && "Memory exhausted");
        *matching_cond.logical_OR = TRUE_ENUM_VALUE;
    }

    
    return matching_cond;
}
