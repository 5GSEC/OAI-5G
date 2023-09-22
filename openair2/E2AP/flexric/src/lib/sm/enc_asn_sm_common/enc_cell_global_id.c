#include <assert.h>
#include <stdlib.h>
#include "../../../util/conversions.h"
#include "../../../sm/rc_sm/ie/asn/OCTET_STRING.h"

#include "../../../sm/rc_sm/ie/asn/CGI.h"
#include "../../../sm/rc_sm/ie/asn/NR-CGI.h"
#include "../../../sm/rc_sm/ie/asn/EUTRA-CGI.h"

#include "enc_cell_global_id.h"


CGI_t enc_cell_global_id_asn(const cell_global_id_t * cell_global_id)
{
    assert(cell_global_id->type == NR_CGI_RAT_TYPE || cell_global_id->type == EUTRA_CGI_RAT_TYPE);

    //CGI_t * cell_global_id_asn = calloc(1, sizeof(CGI_t));
    //assert(cell_global_id_asn != NULL && "Memory exhausted");
    CGI_t cell_global_id_asn = {0}; 

    switch (cell_global_id->type)
    {
      case NR_CGI_RAT_TYPE:
        {
        cell_global_id_asn.present = CGI_PR_nR_CGI;
        cell_global_id_asn.choice.nR_CGI = calloc(1, sizeof(NR_CGI_t));

        MCC_MNC_TO_PLMNID(cell_global_id->nr_cgi.plmn_id.mcc, cell_global_id->nr_cgi.plmn_id.mnc, cell_global_id->nr_cgi.plmn_id.mnc_digit_len, &cell_global_id_asn.choice.nR_CGI->pLMNIdentity);
        cell_global_id_asn.choice.nR_CGI->nRCellIdentity = cp_nr_cell_id_to_bit_string(cell_global_id->nr_cgi.nr_cell_id);
        break;
      }


      case EUTRA_CGI_RAT_TYPE:{
        cell_global_id_asn.present = CGI_PR_eUTRA_CGI;
        cell_global_id_asn.choice.eUTRA_CGI = calloc(1, sizeof(EUTRA_CGI_t));

        MCC_MNC_TO_PLMNID(cell_global_id->eutra.plmn_id.mcc, cell_global_id->eutra.plmn_id.mnc, cell_global_id->eutra.plmn_id.mnc_digit_len, &cell_global_id_asn.choice.eUTRA_CGI->pLMNIdentity);
        cell_global_id_asn.choice.eUTRA_CGI->eUTRACellIdentity = cp_eutra_cell_id_to_bit_string(cell_global_id->eutra.eutra_cell_id);
        break;
      }

      default:
          assert(0!=0 && "Unknown type");
    }

    return cell_global_id_asn;

}
