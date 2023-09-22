#include <assert.h>

#include "enc_matching_cond_frm_3.h"
#include "enc_label_info.h"
#include "enc_test_info.h"

MatchingCondItem_t * kpm_enc_matching_cond_asn(const matching_condition_format_3_lst_t * matching_cond)
{
    MatchingCondItem_t * matching_item_asn = calloc(1, sizeof(MatchingCondItem_t));
    assert(matching_item_asn != NULL && "Memory exhausted");

    if (matching_cond->cond_type == LABEL_INFO)
    {
        matching_item_asn->matchingCondChoice.present = MatchingCondItem_Choice_PR_measLabel;
        
        LabelInfoItem_t * labels = kpm_enc_label_info_asn(&matching_cond->label_info_lst);
        matching_item_asn->matchingCondChoice.choice.measLabel = &labels->measLabel;
    }
    else if (matching_cond->cond_type == TEST_INFO)
    {
        matching_item_asn->matchingCondChoice.present = MatchingCondItem_Choice_PR_testCondInfo;
        matching_item_asn->matchingCondChoice.choice.testCondInfo = calloc(1, sizeof(*matching_item_asn->matchingCondChoice.choice.testCondInfo));
        assert(matching_item_asn->matchingCondChoice.choice.testCondInfo != NULL && "Memory exhausted");
        *matching_item_asn->matchingCondChoice.choice.testCondInfo = kpm_enc_test_info_asn(&matching_cond->test_info_lst);

    }
    else {
        assert(false && "Condition type out of the range");
    }


    if (matching_cond->logical_OR != NULL)
    {
        assert(*matching_cond->logical_OR == TRUE_ENUM_VALUE && "has only one value (true)");
        matching_item_asn->logicalOR = calloc(1, sizeof(LogicalOR_t));
        assert(matching_item_asn->logicalOR != NULL && "Memory exhausted");
        *matching_item_asn->logicalOR = 0;
    }
    

    return matching_item_asn;

}
