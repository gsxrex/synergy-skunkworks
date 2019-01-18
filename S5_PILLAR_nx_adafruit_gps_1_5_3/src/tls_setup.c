#include "iot.h"
#include "tls_cert.h"
#include "network_thread.h"

#define NO_OF_CA_CERTS          (6)
#define SIZE_OF_CA_CERT_BUF     (4 * 1024)

extern const NX_SECURE_TLS_CRYPTO nx_crypto_tls_ciphers_synergys7;
NX_SECURE_X509_CERT g_ca_cert_remote_issuer[NO_OF_CA_CERTS];
NX_SECURE_X509_CERT g_aws_private_cert;

CHAR    crypto_metadata[8850];      ///< Crypto metadata
UCHAR   tls_packet_buffer[16384];
UCHAR   g_remote_ca_cert_buffer[NO_OF_CA_CERTS][SIZE_OF_CA_CERT_BUF];

void mqtt_tls_setup(NXD_MQTT_CLIENT *p_client, NX_SECURE_TLS_SESSION *p_tls_session, NX_SECURE_X509_CERT *p_cert, NX_SECURE_X509_CERT *p_trusted_cert);
void mqtt_tls_setup(NXD_MQTT_CLIENT *p_client, NX_SECURE_TLS_SESSION *p_tls_session, NX_SECURE_X509_CERT *p_cert, NX_SECURE_X509_CERT *p_trusted_cert)
{
    UINT        status;
    uint32_t    i;
    static      bool certs_initialised = false;

    SSP_PARAMETER_NOT_USED(p_cert);

    status = nx_secure_tls_session_create(&(p_client->nxd_mqtt_tls_session), &nx_crypto_tls_ciphers_synergys7, crypto_metadata, sizeof(crypto_metadata));
    if (NX_SUCCESS != status)
    {
        //printf("Could not create TLS Session (0x%02x)\r\n", status);
        nxd_mqtt_client_delete(&g_mqtt_client0);

        return;
    }

    /* Allocate space for packet reassembly */
    status = nx_secure_tls_session_packet_buffer_set(&(p_client->nxd_mqtt_tls_session), tls_packet_buffer, sizeof(tls_packet_buffer));
    if (NX_SUCCESS != status)
    {
        //printf("Could not set TLS session packet buffer. (0x%02x)\r\n", status);
        nxd_mqtt_client_delete(&g_mqtt_client0);

        return;
    }
    if (false == certs_initialised)
    {
        certs_initialised = true;

        for(i=0; i<NO_OF_CA_CERTS; i++)
        {
            memset(g_remote_ca_cert_buffer[i], 0, SIZE_OF_CA_CERT_BUF);
            memset(&g_ca_cert_remote_issuer[i], 0, sizeof (NX_SECURE_X509_CERT));
            status = nx_secure_tls_remote_certificate_allocate(p_tls_session, &g_ca_cert_remote_issuer[i], &g_remote_ca_cert_buffer[i][0], SIZE_OF_CA_CERT_BUF);
            if (NX_SUCCESS != status)
            {
                //printf("Unable to allocate memory for remote certificate\r\n");
                return;
            }
        }
    }

    /* Add a CA Certificate to our trusted store for verifying incoming server certificates. */
    status = nx_secure_x509_certificate_initialize(p_trusted_cert,
                                                   (UCHAR *) g_ca_cert_der, (USHORT) CA_CERT_DER_LEN,
                                                   NX_NULL, 0, NULL, 0, NX_NULL);
    if (NX_SUCCESS != status)
    {
        //printf("Unable to initialize CA certificate\r\n");
        return;
    }

    status = nx_secure_tls_trusted_certificate_add(p_tls_session, p_trusted_cert);
    if (NX_SUCCESS != status)
    {
        //printf("Unable to add CA certificate to trusted store\r\n");
        return;
    }
}
