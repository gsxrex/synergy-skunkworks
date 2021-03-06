#ifndef TLS_CERT_H_
#define TLS_CERT_H_

#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"

#define CA_CERT_DER_LEN         (sizeof(g_ca_cert_der))

/* Root CA certificate */
/* Convert the file (in PEM format) downloaded from CA into DER format using OpenSSL:
 *
 * openssl x509 -inform PEM -in root_ca.pem.crt.txt -outform DER -out root_ca.der.crt.txt
 *
 * Convert the DER format file into a C array using a tool such as Hexy (https://github.com/downloads/tristan2468/Hexy/hexy.exe)
 *
 * Copy the contents of the array output from Hexy into the array below. Do not change the name of the array.
 * Update the macro CA_CERT_DER_LEN in aws_certs.h
 *
 */
static const uint8_t g_ca_cert_der[] =
{
    0x30, 0x82, 0x04, 0x8D, 0x30, 0x82, 0x03, 0x75, 0xA0, 0x03, 0x02, 0x01,
    0x02, 0x02, 0x10, 0x0D, 0x07, 0x78, 0x2A, 0x13, 0x3F, 0xC6, 0xF9, 0xA5,
    0x72, 0x96, 0xE1, 0x31, 0xFF, 0xD1, 0x79, 0x30, 0x0D, 0x06, 0x09, 0x2A,
    0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B, 0x05, 0x00, 0x30, 0x61,
    0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55,
    0x53, 0x31, 0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x13, 0x0C,
    0x44, 0x69, 0x67, 0x69, 0x43, 0x65, 0x72, 0x74, 0x20, 0x49, 0x6E, 0x63,
    0x31, 0x19, 0x30, 0x17, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x13, 0x10, 0x77,
    0x77, 0x77, 0x2E, 0x64, 0x69, 0x67, 0x69, 0x63, 0x65, 0x72, 0x74, 0x2E,
    0x63, 0x6F, 0x6D, 0x31, 0x20, 0x30, 0x1E, 0x06, 0x03, 0x55, 0x04, 0x03,
    0x13, 0x17, 0x44, 0x69, 0x67, 0x69, 0x43, 0x65, 0x72, 0x74, 0x20, 0x47,
    0x6C, 0x6F, 0x62, 0x61, 0x6C, 0x20, 0x52, 0x6F, 0x6F, 0x74, 0x20, 0x47,
    0x32, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x37, 0x31, 0x31, 0x30, 0x32, 0x31,
    0x32, 0x32, 0x33, 0x33, 0x37, 0x5A, 0x17, 0x0D, 0x32, 0x37, 0x31, 0x31,
    0x30, 0x32, 0x31, 0x32, 0x32, 0x33, 0x33, 0x37, 0x5A, 0x30, 0x60, 0x31,
    0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53,
    0x31, 0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x13, 0x0C, 0x44,
    0x69, 0x67, 0x69, 0x43, 0x65, 0x72, 0x74, 0x20, 0x49, 0x6E, 0x63, 0x31,
    0x19, 0x30, 0x17, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x13, 0x10, 0x77, 0x77,
    0x77, 0x2E, 0x64, 0x69, 0x67, 0x69, 0x63, 0x65, 0x72, 0x74, 0x2E, 0x63,
    0x6F, 0x6D, 0x31, 0x1F, 0x30, 0x1D, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13,
    0x16, 0x47, 0x65, 0x6F, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x54, 0x4C,
    0x53, 0x20, 0x52, 0x53, 0x41, 0x20, 0x43, 0x41, 0x20, 0x47, 0x31, 0x30,
    0x82, 0x01, 0x22, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7,
    0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0F, 0x00, 0x30,
    0x82, 0x01, 0x0A, 0x02, 0x82, 0x01, 0x01, 0x00, 0xBE, 0x17, 0xE8, 0xEC,
    0xBE, 0x29, 0x0A, 0xCB, 0xFE, 0xB9, 0x2D, 0x61, 0x31, 0xFD, 0x33, 0x24,
    0x08, 0x32, 0x2E, 0x59, 0xE8, 0x21, 0xD4, 0xD8, 0x30, 0xBE, 0x6E, 0x10,
    0xC8, 0x84, 0xA0, 0x3F, 0xBA, 0x14, 0xE5, 0xDE, 0xFD, 0x7A, 0x8C, 0x92,
    0x1B, 0x7B, 0xCE, 0x84, 0x2D, 0xF0, 0xFF, 0x78, 0xC4, 0x32, 0xE8, 0xA9,
    0xA0, 0x7D, 0x5F, 0x06, 0xDA, 0x7B, 0x9B, 0x4B, 0x53, 0xA6, 0xC6, 0x1B,
    0x02, 0x17, 0x21, 0xE1, 0x70, 0x3B, 0xAD, 0xFB, 0x83, 0xEB, 0x08, 0x54,
    0x81, 0xA8, 0xDE, 0x12, 0xB2, 0xD5, 0xC6, 0x88, 0x96, 0x30, 0xF9, 0x02,
    0xFC, 0x39, 0xD4, 0xBD, 0xB8, 0x22, 0xEF, 0x80, 0x49, 0x99, 0xD0, 0x62,
    0xB8, 0x61, 0xD0, 0x49, 0xDE, 0xCB, 0xC2, 0xCB, 0x97, 0xA5, 0x31, 0x06,
    0x1B, 0xD7, 0xD8, 0x5D, 0xC6, 0xD3, 0x54, 0xDE, 0x52, 0x01, 0x36, 0x2A,
    0x0D, 0xF6, 0xDE, 0xC5, 0xB6, 0x31, 0x4C, 0xCC, 0x15, 0x25, 0x6A, 0x15,
    0x6F, 0xA9, 0x6B, 0x04, 0x48, 0x0C, 0xDE, 0x00, 0x41, 0xAA, 0x28, 0x80,
    0x8B, 0x2F, 0x34, 0xD3, 0x1B, 0xB5, 0x36, 0xAD, 0x3B, 0x25, 0xD0, 0x88,
    0x42, 0x40, 0x6C, 0x36, 0x91, 0x6D, 0x65, 0xB2, 0x19, 0x86, 0xC0, 0xD2,
    0x7F, 0x39, 0x46, 0x58, 0xFE, 0x30, 0x12, 0x60, 0x50, 0xDC, 0xEE, 0xBB,
    0x73, 0xE6, 0x57, 0x90, 0x5A, 0xF6, 0x0D, 0xCA, 0xD7, 0x04, 0x4B, 0x47,
    0x6A, 0x6F, 0x34, 0x1A, 0x9D, 0x92, 0x36, 0x1A, 0x2E, 0xD9, 0x4E, 0x54,
    0xED, 0x47, 0xAC, 0x0C, 0xBF, 0xF1, 0x80, 0xB2, 0xBA, 0xFF, 0x47, 0x7B,
    0xE9, 0x39, 0xC4, 0x54, 0xC4, 0x94, 0x54, 0x99, 0x19, 0xF1, 0x57, 0x99,
    0xAF, 0xE2, 0x14, 0x22, 0x5B, 0xE8, 0x2E, 0xBB, 0x63, 0x2D, 0xBA, 0xAE,
    0x81, 0xBD, 0x13, 0xDC, 0xE6, 0x17, 0x5B, 0xE0, 0x90, 0x53, 0x49, 0x01,
    0x02, 0x03, 0x01, 0x00, 0x01, 0xA3, 0x82, 0x01, 0x40, 0x30, 0x82, 0x01,
    0x3C, 0x30, 0x1D, 0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14,
    0x94, 0x4F, 0xD4, 0x5D, 0x8B, 0xE4, 0xA4, 0xE2, 0xA6, 0x80, 0xFE, 0xFD,
    0xD8, 0xF9, 0x00, 0xEF, 0xA3, 0xBE, 0x02, 0x57, 0x30, 0x1F, 0x06, 0x03,
    0x55, 0x1D, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x4E, 0x22, 0x54,
    0x20, 0x18, 0x95, 0xE6, 0xE3, 0x6E, 0xE6, 0x0F, 0xFA, 0xFA, 0xB9, 0x12,
    0xED, 0x06, 0x17, 0x8F, 0x39, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x1D, 0x0F,
    0x01, 0x01, 0xFF, 0x04, 0x04, 0x03, 0x02, 0x01, 0x86, 0x30, 0x1D, 0x06,
    0x03, 0x55, 0x1D, 0x25, 0x04, 0x16, 0x30, 0x14, 0x06, 0x08, 0x2B, 0x06,
    0x01, 0x05, 0x05, 0x07, 0x03, 0x01, 0x06, 0x08, 0x2B, 0x06, 0x01, 0x05,
    0x05, 0x07, 0x03, 0x02, 0x30, 0x12, 0x06, 0x03, 0x55, 0x1D, 0x13, 0x01,
    0x01, 0xFF, 0x04, 0x08, 0x30, 0x06, 0x01, 0x01, 0xFF, 0x02, 0x01, 0x00,
    0x30, 0x34, 0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x01,
    0x04, 0x28, 0x30, 0x26, 0x30, 0x24, 0x06, 0x08, 0x2B, 0x06, 0x01, 0x05,
    0x05, 0x07, 0x30, 0x01, 0x86, 0x18, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F,
    0x2F, 0x6F, 0x63, 0x73, 0x70, 0x2E, 0x64, 0x69, 0x67, 0x69, 0x63, 0x65,
    0x72, 0x74, 0x2E, 0x63, 0x6F, 0x6D, 0x30, 0x42, 0x06, 0x03, 0x55, 0x1D,
    0x1F, 0x04, 0x3B, 0x30, 0x39, 0x30, 0x37, 0xA0, 0x35, 0xA0, 0x33, 0x86,
    0x31, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x63, 0x72, 0x6C, 0x33,
    0x2E, 0x64, 0x69, 0x67, 0x69, 0x63, 0x65, 0x72, 0x74, 0x2E, 0x63, 0x6F,
    0x6D, 0x2F, 0x44, 0x69, 0x67, 0x69, 0x43, 0x65, 0x72, 0x74, 0x47, 0x6C,
    0x6F, 0x62, 0x61, 0x6C, 0x52, 0x6F, 0x6F, 0x74, 0x47, 0x32, 0x2E, 0x63,
    0x72, 0x6C, 0x30, 0x3D, 0x06, 0x03, 0x55, 0x1D, 0x20, 0x04, 0x36, 0x30,
    0x34, 0x30, 0x32, 0x06, 0x04, 0x55, 0x1D, 0x20, 0x00, 0x30, 0x2A, 0x30,
    0x28, 0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x02, 0x01, 0x16,
    0x1C, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77,
    0x2E, 0x64, 0x69, 0x67, 0x69, 0x63, 0x65, 0x72, 0x74, 0x2E, 0x63, 0x6F,
    0x6D, 0x2F, 0x43, 0x50, 0x53, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48,
    0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01,
    0x00, 0x82, 0x1C, 0x04, 0x3A, 0x82, 0xE9, 0xC5, 0xA9, 0xC8, 0x65, 0x12,
    0x5C, 0x08, 0xE3, 0x01, 0xC6, 0x30, 0xB0, 0xAE, 0x22, 0x88, 0x61, 0x7B,
    0x2B, 0x07, 0x86, 0xF7, 0xB8, 0xB5, 0x44, 0x9C, 0xF5, 0x93, 0x06, 0x61,
    0x46, 0x8A, 0x3A, 0xA2, 0xB5, 0x35, 0x26, 0xD0, 0x58, 0x9E, 0x3C, 0xC1,
    0x73, 0x8F, 0xBF, 0xB1, 0x79, 0xA7, 0x1C, 0x99, 0x37, 0x9A, 0x53, 0xF2,
    0x2F, 0x5C, 0x6E, 0x20, 0x0B, 0x7D, 0xB0, 0xC7, 0x82, 0xEF, 0xEB, 0xEB,
    0x79, 0x12, 0x05, 0xE7, 0x01, 0x41, 0x0B, 0x43, 0x02, 0x37, 0x9B, 0x1B,
    0x1F, 0x08, 0x4D, 0x37, 0x52, 0x73, 0x29, 0xB8, 0x9F, 0x35, 0xE3, 0xF9,
    0xA3, 0x3A, 0x59, 0x75, 0x01, 0x66, 0x90, 0x2A, 0x3E, 0x9D, 0xE0, 0x62,
    0x29, 0x1E, 0x87, 0xA8, 0x80, 0x3E, 0x2C, 0xC4, 0xCC, 0x08, 0xC3, 0x2E,
    0xBF, 0x69, 0x21, 0x18, 0x6B, 0x8F, 0x1E, 0x6B, 0x43, 0xCD, 0xEE, 0x06,
    0x98, 0x41, 0x99, 0xB5, 0x82, 0xF2, 0x17, 0xC1, 0x10, 0xA5, 0xAE, 0x58,
    0x8C, 0xFB, 0x95, 0xEA, 0x4D, 0x82, 0xAF, 0xF3, 0x77, 0x5F, 0x11, 0x69,
    0x4F, 0x78, 0x51, 0xA6, 0xDF, 0xF7, 0xBA, 0xC5, 0x3F, 0x65, 0xBF, 0x70,
    0x7C, 0xA3, 0xED, 0x5A, 0x9A, 0x33, 0x9D, 0x9E, 0x3F, 0xE3, 0x87, 0x35,
    0xA7, 0xDA, 0xF3, 0x15, 0xC6, 0x58, 0xAD, 0x69, 0x23, 0xDC, 0x0F, 0xBB,
    0x4C, 0xD0, 0x49, 0x13, 0x41, 0xA6, 0x3F, 0x67, 0xE1, 0x5E, 0x13, 0xF5,
    0x0F, 0xF5, 0xD5, 0x7C, 0x85, 0xBF, 0x87, 0x4C, 0x82, 0x24, 0x61, 0x22,
    0x57, 0xD1, 0xA5, 0x9B, 0xFB, 0x86, 0x39, 0xAD, 0xC3, 0x5C, 0x79, 0x66,
    0x6C, 0x07, 0x31, 0x4B, 0x20, 0x91, 0xAD, 0xA1, 0xBE, 0x64, 0x19, 0x00,
    0xEE, 0x1E, 0x12, 0x78, 0xCE, 0x98, 0xF2, 0x5F, 0xFB, 0x30, 0x14, 0x69,
    0x3C, 0x2C, 0xFA, 0x97, 0xC6
};

#endif /* TLS_CERT_H_ */
