#include <assert.h>
#include <stdlib.h>
// #include "../../ap/ie/asn/OCTET_STRING.h"
#include "../../../util/conversions.h"
#include "../../../sm/rc_sm/ie/asn/OCTET_STRING.h"
#include "../../../sm/rc_sm/ie/asn/NR-CGI.h"
#include "../../../sm/rc_sm/ie/asn/EUTRA-CGI.h"

#include "dec_cell_global_id.h"


cell_global_id_t dec_cell_global_id_asn(const CGI_t * cell_global_id_asn)
{
    assert(cell_global_id_asn->present == CGI_PR_nR_CGI || cell_global_id_asn->present == CGI_PR_eUTRA_CGI);

    cell_global_id_t cell_global_id = {0}; //calloc(1, sizeof(cell_global_id_t));
//    assert(cell_global_id != NULL && "Memory exhausted");

    switch (cell_global_id_asn->present)
    {
    case CGI_PR_nR_CGI:
      {
        cell_global_id.type = NR_CGI_RAT_TYPE;
        
        PLMNID_TO_MCC_MNC(&cell_global_id_asn->choice.nR_CGI->pLMNIdentity, cell_global_id.nr_cgi.plmn_id.mcc, cell_global_id.nr_cgi.plmn_id.mnc, cell_global_id.nr_cgi.plmn_id.mnc_digit_len);
        cell_global_id.nr_cgi.nr_cell_id = cp_nr_cell_id_to_u64(cell_global_id_asn->choice.nR_CGI->nRCellIdentity);
        break;
      } 
    case CGI_PR_eUTRA_CGI:
      {
        cell_global_id.type = EUTRA_CGI_RAT_TYPE;

        PLMNID_TO_MCC_MNC(&cell_global_id_asn->choice.eUTRA_CGI->pLMNIdentity, cell_global_id.eutra.plmn_id.mcc, cell_global_id.eutra.plmn_id.mnc, cell_global_id.eutra.plmn_id.mnc_digit_len);
        cell_global_id.eutra.eutra_cell_id = cp_eutra_cell_id_to_u32(cell_global_id_asn->choice.eUTRA_CGI->eUTRACellIdentity);
        break;
      }
    default:
      assert(0!=0 && "Unknown type" );
    }

    return cell_global_id;
}
