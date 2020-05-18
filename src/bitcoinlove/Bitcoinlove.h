#ifndef ZC_BLVCASH_H_
#define ZC_BLVCASH_H_

#define ZC_NUM_JS_INPUTS 2
#define ZC_NUM_JS_OUTPUTS 2
#define INCREMENTAL_MERKLE_TREE_DEPTH 29
#define INCREMENTAL_MERKLE_TREE_DEPTH_TESTING 4

#define SAPLING_INCREMENTAL_MERKLE_TREE_DEPTH 32

#define NOTEENCRYPTION_AUTH_BYTES 16

#define ZC_NOTEPLAINTEXT_LEADING 1
#define ZC_V_SIZE 8
#define ZC_RHO_SIZE 32
#define ZC_R_SIZE 32
#define ZC_MEMO_SIZE 512
#define ZC_DIVERSIFIER_SIZE 11
#define ZC_JUBJUB_POINT_SIZE 32
#define ZC_JUBJUB_SCALAR_SIZE 32

#define ZC_NOTEPLAINTEXT_SIZE (ZC_NOTEPLAINTEXT_LEADING + ZC_V_SIZE + ZC_RHO_SIZE + ZC_R_SIZE + ZC_MEMO_SIZE)

#define ZC_SAPLING_ENCPLAINTEXT_SIZE (ZC_NOTEPLAINTEXT_LEADING + ZC_DIVERSIFIER_SIZE + ZC_V_SIZE + ZC_R_SIZE + ZC_MEMO_SIZE)
#define ZC_SAPLING_OUTPLAINTEXT_SIZE (ZC_JUBJUB_POINT_SIZE + ZC_JUBJUB_SCALAR_SIZE)

#define ZC_SAPLING_ENCCIPHERTEXT_SIZE (ZC_SAPLING_ENCPLAINTEXT_SIZE + NOTEENCRYPTION_AUTH_BYTES)
#define ZC_SAPLING_OUTCIPHERTEXT_SIZE (ZC_SAPLING_OUTPLAINTEXT_SIZE + NOTEENCRYPTION_AUTH_BYTES)

#endif // ZC_BLVCASH_H_
