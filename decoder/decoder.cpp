#include "decoder.h"

int
Decoder::run_decoder(const LDPC &cc, const rece_seq &sq, double ebn0, de_algo algo) {
    // 简单判断长度是否相等
    assert(cc.get_len() == sq.size());
    switch (algo)
    {
        case msa:
            return run_use_msa(cc, sq, ebn0);
            break;
        case spa:
            return run_use_spa(cc, sq, ebn0);
            break;
        default:
            break;
    }
    return 0;
}

/**@brief run msa decoding
 * @param[in]  cc            LDPC code
 * @param[in]  sq            received sequence for channel
 * @param[in]  ebno          channel condition
 * @param[out] int           1 -> success, 0 -> fail
*/
int 
Decoder::run_use_msa(const LDPC &cc, const rece_seq &sq, double ebn0) {
    int len = cc.get_len();
    get_llr(sq, cc.get_code_ratio(), ebn0);

    cns_llr.assign(cc.get_row(), llr_seq(cc.get_dc(), 0));
    vns_llr.assign(cc.get_col(), llr_seq(cc.get_dv(), 0));
    sum_llr.assign(cc.get_col(), 0);
    out_seqs.assign(cc.get_col(), 0);

    // initial matrix
    for (int i = 0; i < cc.get_col(); ++i) {
        for (int j = 0; j < cc.get_dv(); ++j) {
            vns_llr[i][j] = cc.get_vns_xy(i, j) < 0 ? 0 : initial_llr[i];
        }
    }

    // per iteration
    int iter = 0;
    bool decode_ok = false;
    while (iter < iteration && !decode_ok) {
        cn_update(cc);
        vn_update(cc);

        // check all
        for (int i = 0; i < cc.get_col(); ++i) {
            out_seqs[i] = (sum_llr[i] >= 0) ? 0 : 1;
        }

        decode_ok = check(cc) == 0;
        iter++;
    } // per iteration or decode_ok
    if (!decode_ok) checkfailed++;
#ifndef DEBUG
    if (!decode_ok) {
        printf("check bad %u \n", checkfailed);
    }
#endif
    return decode_ok;
}

/**@brief check node info update
 * @param[in] cc    LDPC code
 *
*/
void 
Decoder::cn_update(const LDPC &cc) {
    vector<char> sign_of_llr(cc.get_dc(), 0); // 0 means >= 0 else < 0
    for (int i = 0; i < cc.get_row(); ++i) {
        llr_width min_llr = 10000, sub_min_llr = 10000;
        char sign_all = 0;
        pos_t cn_index = i / cc.get_gf();
        pos_t min_llr_index = 0;
        pos_t vn_index = 0;
        llr_width cur_llr = 0;
        llr_width cur_llr_abs = 0;

        for (int j = 0; j < cc.get_dc(); ++j) {
            vn_index = cc.get_cns_xy(i, j);
            if (vn_index < 0) continue;

            cur_llr = vns_llr[vn_index][cn_index];
            cur_llr_abs = fabs(cur_llr);
            // sign of result
            sign_of_llr[j] = cur_llr >= 0 ? 0 : 1;
            sign_all ^= sign_of_llr[j];

            // compare to find min llr
            if (min_llr > cur_llr_abs) {
                sub_min_llr = min_llr;
                min_llr = cur_llr_abs;
                min_llr_index = j;
            }
            else if (sub_min_llr > cur_llr_abs) {
                sub_min_llr = cur_llr_abs;
            }
        } // per vn

        // with attenuation
        min_llr     *= attenuation;
        sub_min_llr *= attenuation;
        for (int j = 0; j < cc.get_dc(); ++j) {
            if (sign_all ^ sign_of_llr[j] == 1)
                cns_llr[i][j] = -min_llr;
            else
                cns_llr[i][j] = min_llr;
        } // write

        // deal with special case
        if (sign_all ^ sign_of_llr[min_llr_index] == 1)
            cns_llr[i][min_llr_index] = -sub_min_llr;
        else
            cns_llr[i][min_llr_index] = sub_min_llr;

    } // per cn
}

/**@brief variable node info update
 * @param[in] cc    LDPC code
 *
*/
void
Decoder::vn_update(const LDPC &cc) {
    // sum of llr all
    for (int i = 0; i < cc.get_col(); ++i) {
        pos_t vn_index = i / cc.get_gf();
        sum_llr[i] = initial_llr[i];
        for (int j = 0; j < cc.get_dv(); ++j) {
            int cn_index = cc.get_vns_xy(i, j);
            if (cn_index < 0) continue;
            sum_llr[i] += cns_llr[cn_index][vn_index];
        } // sum llr
    } // per vn


    // vnupdate
    for (int i = 0; i < cc.get_col(); ++i) {
        pos_t vn_index = i / cc.get_gf();
        for (int j = 0; j < cc.get_dv(); ++j) {
            int cn_index = cc.get_vns_xy(i, j);
            if (cn_index < 0) continue; // null point
            vns_llr[i][j] = sum_llr[i] - cns_llr[cn_index][vn_index];
        } // per cn
    } // per cn
}

/**@brief check if decoding success
 * @param[in]  cc    LDPC code
 * @param[out] int   0 -> check success, else -> check fail
*/
int
Decoder::check(const LDPC &cc) {
    int checksum = 0;
    for (int i = 0; i < cc.get_row(); ++i) {
        bit_t row_check = 0;
        for (int j = 0; j < cc.get_dc(); ++j) {
            if (cc.get_cns_xy(i, j) < 0) continue;
            row_check ^= out_seqs[cc.get_cns_xy(i, j)]; // with correction
        }
        checksum += row_check;
    }
    return checksum;
}

/**@brief calc LLR from received sequence at channel condition ebn0
 * @param[in] sq          channel output to decoder
 * @param[in] code_ratio  code ration of ldpc code
 * @param[in] ebn0        channel condition
*/
void
Decoder::get_llr(const rece_seq &sq, double code_ratio, double ebn0) {
    initial_llr.assign(sq.size()+1, 0);
    double EsN0 = code_ratio * pow(10, ebn0/10.0);
    for (int i = 0; i < sq.size(); ++i) {
        initial_llr[i] = 4 * sq[i] * EsN0;
    }
}

int
Decoder::run_use_spa(const LDPC &cc, const rece_seq &sq, double ebn0) {

    return 0;
}
