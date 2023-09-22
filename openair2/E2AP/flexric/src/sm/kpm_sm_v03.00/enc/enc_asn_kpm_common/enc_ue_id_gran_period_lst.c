#include "enc_ue_id_gran_period_lst.h"
#include "../../../../lib/sm/enc_asn_sm_common/enc_ue_id.h"
#include "../../ie/asn/asn_constant.h"
#include "../../ie/asn/MatchingUEidItem-PerGP.h"
#include "../../ie/asn/MatchingUEidList-PerGP.h"

#include <assert.h>

MatchingUEidPerGP_Item_t * kpm_enc_ue_id_gran_period_asn(const ue_id_gran_period_lst_t * ue_id_gran_period)
{
  MatchingUEidPerGP_Item_t * matching_gp_ue_item = calloc(1, sizeof(MatchingUEidPerGP_Item_t));
  assert(matching_gp_ue_item != NULL && "Memory exhausted");


  switch (ue_id_gran_period->num_matched_ue)
  {
    case NONE_MATCHED_UE:
      {
        matching_gp_ue_item->matchedPerGP.present = MatchingUEidPerGP_Item__matchedPerGP_PR_noUEmatched;
        matching_gp_ue_item->matchedPerGP.choice.noUEmatched = 0;
        break;
      }

    case ONE_OR_MORE_MATCHED_UE:
      {
        matching_gp_ue_item->matchedPerGP.present = MatchingUEidPerGP_Item__matchedPerGP_PR_oneOrMoreUEmatched;
        assert(ue_id_gran_period->matched_ue_lst.ue_lst_len >=1 && ue_id_gran_period->matched_ue_lst.ue_lst_len <= maxnoofUEID);

        matching_gp_ue_item->matchedPerGP.choice.oneOrMoreUEmatched = calloc(ue_id_gran_period->matched_ue_lst.ue_lst_len, sizeof(MatchingUEidList_PerGP_t));
        assert(matching_gp_ue_item->matchedPerGP.choice.oneOrMoreUEmatched != NULL && "Memory exhausted");

        for (size_t z = 0; z<ue_id_gran_period->matched_ue_lst.ue_lst_len; z++)
        {
          MatchingUEidItem_PerGP_t * gp_item = calloc(1, sizeof(MatchingUEidItem_PerGP_t));
          assert(gp_item != NULL && "Memory exhausted");

          gp_item->ueID = enc_ue_id_asn(&ue_id_gran_period->matched_ue_lst.ue_lst[z]);
          int rc1 = ASN_SEQUENCE_ADD(&matching_gp_ue_item->matchedPerGP.choice.oneOrMoreUEmatched->list, gp_item);
          assert(rc1 == 0);
        }
        break;
      }

    default:
      {
        assert(0!=0 && "Unknown type");
        break;
      }
  }

  return matching_gp_ue_item;
}
