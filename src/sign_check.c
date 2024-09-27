#include "sign_check.h"

#include <psa/crypto.h>
#include <psa/crypto_extra.h>

const uint8_t m_pub_key[] = {
    0x8d, 0x56, 0x39, 0xbb, 0xef, 0x40, 0x55, 0x42,
    0x27, 0xa4, 0xc3, 0x14, 0x2a, 0x07, 0xec, 0xd9,
    0x60, 0xd6, 0xdc, 0xe3, 0x84, 0x26, 0x5f, 0xf0,
    0xe6, 0x23, 0x06, 0x7a, 0xba, 0x0d, 0x54, 0x4f,
    0x8f, 0x8b, 0xe2, 0xa2, 0x22, 0xd9, 0x5f, 0xbb,
    0xbe, 0x57, 0x88, 0x08, 0x9a, 0xbf, 0xc2, 0x4c,
    0xa1, 0x2f, 0x2a, 0x29, 0x71, 0x01, 0xfe, 0x8f,
    0x43, 0x16, 0x62, 0xd3, 0x61, 0xa4, 0x5f, 0x92};

static psa_key_id_t pub_key_id;

CL_Result_t crypto_init(void)
{
    psa_status_t status;

    /* Initialize PSA Crypto */
    status = psa_crypto_init();
    if (status != PSA_SUCCESS)
        return CL_ResFailed;

    return CL_ResSuccess;
}

CL_Result_t import_ecdsa_pub_key(void)
{
    /* Configure the key attributes */
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status;

    /* Configure the key attributes */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_K1));
    psa_set_key_bits(&key_attributes, 256);

    status = psa_import_key(&key_attributes, m_pub_key, sizeof(m_pub_key), &pub_key_id);
    if (status != PSA_SUCCESS)
    {
        return CL_ResFailed;
    }

    /* Reset key attributes and free any allocated resources. */
    psa_reset_key_attributes(&key_attributes);

    return CL_ResSuccess;
}

CL_Result_t verify_message(const uint8_t *msg, uint32_t len, const uint8_t *sig, uint32_t sig_len)
{
    static uint8_t m_hash[32]; // sha256 256bits
    psa_status_t status;

    uint32_t output_len;
    status = psa_hash_compute(PSA_ALG_SHA_256,
                              msg,
                              len,
                              m_hash,
                              sizeof(m_hash),
                              &output_len);
    if (status != PSA_SUCCESS)
        return CL_ResFailed;

    /* Verify the signature of the hash */
    status = psa_verify_hash(pub_key_id,
                             PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                             m_hash,
                             sizeof(m_hash),
                             sig,
                             sig_len); // 64字节
    if (status != PSA_SUCCESS)
        return CL_ResFailed;

    return CL_ResSuccess;
}

CL_Result_t crypto_finish(void)
{
    psa_status_t status;

    status = psa_destroy_key(pub_key_id);
    if (status != PSA_SUCCESS)
        return CL_ResFailed;

    return CL_ResSuccess;
}

void SignCheck_Init(void)
{
    if (crypto_init() != CL_ResSuccess)
    { // todo error
    }
}

CL_Result_t SignCheck(const uint8_t *data, uint32_t dataSize, const uint8_t *sign, uint32_t signSize)
{
    if(import_ecdsa_pub_key()!= CL_ResSuccess)
        return CL_ResFailed;

    CL_Result_t res = verify_message(data, dataSize, sign, signSize);

    crypto_finish();
    return res;
}
