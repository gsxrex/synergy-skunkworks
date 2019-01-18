#include "iot.h"
#include "quectel_gps.h"
#include "network_thread.h"
#include "stdio.h"

#define TEN_MS          (1)
#define ONE_HUNDRED_MS  (10)
#define ONE_SECOND      (ONE_HUNDRED_MS * 10)
#define TWO_SECONDS     (ONE_SECOND * 2)
#define FIVE_SECONDS    (ONE_SECOND * 5)

#define GREEN_LED       (IOPORT_PORT_07_PIN_08)
#define RED_LED         (IOPORT_PORT_07_PIN_11)
#define YELLOW_LED      (IOPORT_PORT_07_PIN_10)


static const sf_cellular_provisioning_t cell_prov_info =
{
    .airplane_mode  = SF_CELLULAR_AIRPLANE_MODE_OFF,
    .auth_type      = SF_CELLULAR_AUTH_TYPE_NONE,
    .username       = "",
    .password       = "",
    .apn            = CELLULAR_APN,
    .context_id     = 1,
    .pdp_type       = SF_CELLULAR_PDP_TYPE_IP,
};

/*
 * structure to describe data point specific to Adafruit IO GPS widget
 * https://cdn-learn.adafruit.com/downloads/pdf/adafruit-io-basics-gps.pdf
 */
struct gps_data {
    float   speed;
    char    latitude[TOKEN_LENGTH];
    char    longitude[TOKEN_LENGTH];
    float   altitude;
};

char gps_raw_string[100];

static char g_interval_topic[NXD_MQTT_MAX_TOPIC_NAME_LENGTH] = ADAFRUIT_USERNAME;
static char g_enable_topic[NXD_MQTT_MAX_TOPIC_NAME_LENGTH] = ADAFRUIT_USERNAME;
static bool g_is_enabled = true;
static int  g_interval = 5; /* 5 second default send interval */

void mqtt_tls_setup(NXD_MQTT_CLIENT *, NX_SECURE_TLS_SESSION *, NX_SECURE_X509_CERT *, NX_SECURE_X509_CERT *);
void mqtt_receive(NXD_MQTT_CLIENT *, UINT);

ssp_err_t gps_init(sf_cellular_ctrl_t* p_ctrl);
void read_gps_coordinates(struct gps_data *gps);

/*
 * Registered user callback function for UART associated with GPS operation.
 *
 */
void user_uart_callback(uart_callback_args_t *p_args)
{
    switch(p_args->event)
    {
        case UART_EVENT_RX_CHAR:
            tx_queue_send(&g_gps_queue,&(p_args->data),TX_NO_WAIT);
            break;
        case UART_EVENT_ERR_PARITY:
        case UART_EVENT_ERR_FRAMING:
        case UART_EVENT_BREAK_DETECT:
        case UART_EVENT_ERR_OVERFLOW:
        case UART_EVENT_ERR_RXBUF_OVERFLOW:
            break;
        default:
            break;
    }
}

void read_gps_coordinates(struct gps_data *gps)
{
    UINT retval = 0;

    uint8_t i = 0;

    char* token_start = NULL;
    int   token_length;
    char* curr_ptr = NULL;
    char  token[TOKEN_COUNT][TOKEN_LENGTH];
    char str[TOKEN_LENGTH];
    uint32_t index = 0;

    float value = 0;


    /* Keep reading till there is data in the queue */
    while(g_gps_queue.tx_queue_enqueued)
    {
        tx_thread_sleep(1);

        /* Read a character from the queue */
        retval = tx_queue_receive(&g_gps_queue, (gps_raw_string + index), TX_NO_WAIT);

        if(TX_SUCCESS == retval)
        {
            /* Check for end token (*) of GPS location string */
            if(GGA_END == gps_raw_string[index++])
            {
                /* Flush queue to receive new updates */
                tx_queue_flush(&g_gps_queue);

                /* Null terminate GPS location information string */
                gps_raw_string[index] = 0;
                break;
            }

            if(index > (sizeof(gps_raw_string) - 1))
                index = 0;
        }
    }

    /* verify string has beginning preamble and end markers */
    if ((strstr(gps_raw_string, GGA_PREAMBLE) == NULL) ||
        (strchr(gps_raw_string, GGA_END) == NULL))
    {
        //printf("GPS string not valid!\r\n");
        return;
    }

    memset(token, 0, sizeof(token[0][0]) * TOKEN_COUNT * TOKEN_LENGTH);
    for (curr_ptr = gps_raw_string; curr_ptr < gps_raw_string + strlen(gps_raw_string); curr_ptr++)
    {
        if (curr_ptr == gps_raw_string)
        {
            /* start of GGA string */
            token_start = curr_ptr;
        }

        if (*curr_ptr == GGA_DELIMITER)
        {
            /* end of the token, so save it away */
            token_length = curr_ptr - token_start;
            strncpy(token[i++], token_start, (unsigned int)token_length);

            /* setup for next token */
            token_start = curr_ptr + 1;
        }
    }

    /* only send a reading if lat/long are present */
    if ((strlen(token[GGA_LATITUDE]) > 0) && (strlen(token[GGA_LONGITUDE]) > 0))
    {
        /*
         * degree/minute to decimal-degree conversion
         *
         * D + (M.m)/60
         *
         * west and south are negative direction
         */
        /*
         * latitude format:
         * ==========================================================
         * | byte 0 | byte 1 | byte 2 | byte 3 | byte 4 | byte 5..n |
         * ==========================================================
         * |     degrees     |     minutes     |   dec  | fractional|
         * |                                     point     minutes  |
         * ==========================================================
         */

        /* convert M.m to decimal degrees */
        value = (float)atof(token[GGA_LATITUDE] + 2);
        value /= 60;
        memset(token[GGA_LATITUDE] + 2, 0, 1); /* zero out M.m so atof only gets the degrees portion */
        value += (float)atof(token[GGA_LATITUDE]); /* add whole number degrees to fractional degrees */
        sprintf(str, "%2.7f", value);

        if (NULL != gps)
        {
            /* Check if we receive South in GPS location */
            if (strchr(token[GGA_LAT_DIRECTION], GGA_SOUTH))
            {
                /* Add - sign to latitude */
                strcpy(gps->latitude, "-");
            }
            strcpy(gps->latitude, str);

            /*
             * longitude format:
             * ===================================================================
             * | byte 0 | byte 1 | byte 2 | byte 3 | byte 4 | byte 5 | byte 6..n |
             * ===================================================================
             * |          degrees         |     minutes     |   dec  | fractional|
             * |                                              point     minutes  |
             * ===================================================================
             */

            /* convert M.m to decimal degrees */
            value = (float)atof(token[GGA_LONGITUDE] + 3);
            value /= 60;
            memset(token[GGA_LONGITUDE] + 3, 0, 1); /* zero out M.m so atof only gets the degrees portion */
            value += (float)atof(token[GGA_LONGITUDE]); /* add whole number degrees to fractional degrees */
            sprintf(str, "%3.7f", value);

            /* Check if we receive West in GPS location */
            if (GGA_WEST == *token[GGA_LONG_DIRECTION])
            {
                /* Add - sign to longitude */
                strcpy(gps->longitude, "-");
            }
            strcat(gps->longitude, str);
        }

        gps->altitude = (float)atof(token[GGA_ALTITUDE]);
        gps->speed = 0.0f; /* no speed data */
    }
}

/* Network Thread entry function */
void network_thread_entry(void)
{
    UINT  status;
    ULONG ip_status;

    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &ip_status, 12000);
    if (NX_SUCCESS != status)
    {
        //printf("Cellular link not established within specified time period\n\r");
        __BKPT(0);
    }

    ULONG ip_addr = 0, ip_mask = 0;

    nx_ip_address_get(&g_ip0, &ip_addr, &ip_mask);

    //printf("IP address: %lu.%lu.%lu.%lu\r\n", (ip_addr >> 24) & 0xFF,
    //                                          (ip_addr >> 16) & 0xFF,
    //                                          (ip_addr >>  8) & 0xFF,
    //                                          (ip_addr)       & 0xFF);

    //printf("Subnet mask: %lu.%lu.%lu.%lu\r\n", (ip_mask >> 24) & 0xFF,
    //                                           (ip_mask >> 16) & 0xFF,
    //                                           (ip_mask >>  8) & 0xFF,
    //                                           (ip_mask)       & 0xFF);

    ULONG ip_dns = 0;

    status = nx_ppp_dns_address_get(&g_nx_ppp0, &ip_dns);
    if ((NX_SUCCESS != status) || (0 == ip_dns))
    {
        //printf("Failed to get DNS address from IPCP (error code %d)\n\r", status);
        __BKPT(0);
    }

    status = nx_dns_server_add(&g_dns0, ip_dns);
    if (NX_SUCCESS != status)
    {
        //printf("Failed to set DNS address (error code %d)\n\r", status);
        __BKPT(0);
    }

    NXD_ADDRESS ip_server = {0};

    status = nxd_dns_host_by_name_get(&g_dns0, (UCHAR *) ADAFRUIT_SERVER,
                                      &ip_server, 2000, NX_IP_VERSION_V4);

    if (NX_SUCCESS == status)
    {
        //printf("Successfully resolved IP for %s: %lu.%lu.%lu.%lu\n\r",
        //       ADAFRUIT_SERVER,
        //       (ip_server.nxd_ip_address.v4 >> 24) & 0xFF, (ip_server.nxd_ip_address.v4 >> 16) & 0xFF,
        //       (ip_server.nxd_ip_address.v4 >>  8) & 0xFF, (ip_server.nxd_ip_address.v4)       & 0xFF);
    }
    else
    {
        //printf("Failed to resolve IP address for %s (error code %d)\n\r",
        //       (char *) ADAFRUIT_SERVER, status);
        __BKPT(0);
    }

    status = nxd_mqtt_client_login_set(&g_mqtt_client0,
                                       ADAFRUIT_USERNAME, strlen(ADAFRUIT_USERNAME),
                                       ADAFRUIT_KEY, strlen(ADAFRUIT_KEY));
    if (NX_SUCCESS != status)
    {
        //printf("Failed to set MQTT login details (error code %d)\n\r", status);
        __BKPT(0);
    }

    tx_thread_sleep(500);

    status = nxd_mqtt_client_secure_connect(&g_mqtt_client0, &ip_server, NXD_MQTT_TLS_PORT,
                                            mqtt_tls_setup, 600, NX_TRUE, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        //printf("Failed to connect to MQTT server (error code %d)\n\r", status);
        __BKPT(0);
    }

    strcat(g_enable_topic, "/feeds/enable");
    strcat(g_interval_topic, "/feeds/interval");

    status = nxd_mqtt_client_receive_notify_set(&g_mqtt_client0, mqtt_receive);
    if (NX_SUCCESS != status)
    {
        //printf("Failed to register MQTT receive callback (error code %d)\n\r", status);
        __BKPT(0);
    }

    status = nxd_mqtt_client_subscribe(&g_mqtt_client0, g_enable_topic, strlen(g_enable_topic), 1);
    if(NX_SUCCESS != status)
    {
        //printf("Failed to subscribe to led topic (error code %d)\r\n", status);
        __BKPT(0);
    }

    status = nxd_mqtt_client_subscribe(&g_mqtt_client0, g_interval_topic, strlen(g_interval_topic), 1);
    if(NX_SUCCESS != status)
    {
        //printf("Failed to subscribe to interval topic (error code %d)\r\n", status);
        __BKPT(0);
    }

    while (1)
    {
        if (g_is_enabled)
        {
            struct gps_data gps_data_pt = {
                .speed = 0.0f,
                .latitude = "",
                .longitude = "",
                .altitude = 0.0f
            };

            read_gps_coordinates(&gps_data_pt);

            if (strlen(gps_data_pt.latitude) > 0)
            {
                char topic[NXD_MQTT_MAX_TOPIC_NAME_LENGTH] = {0};
                char message[64] = {0};

                /* Adafruit GPS data goes to a special subfeed csv under the main feed*/
                sprintf(topic, "%s/feeds/gps/csv", ADAFRUIT_USERNAME);
                sprintf(message, "%3.2f,%s,%s,%5.2f", gps_data_pt.speed,
                                                      gps_data_pt.latitude,
                                                      gps_data_pt.longitude,
                                                      gps_data_pt.altitude);

                status = nxd_mqtt_client_publish(&g_mqtt_client0,
                                                 topic, strlen(topic),
                                                 message, strlen(message),
                                                 NX_TRUE, 0, NX_WAIT_FOREVER);
                if (NX_SUCCESS != status)
                {
                    break;
                }
            }
        }

        /* default 5 second interval unless we get a different interval from the MQTT broker */
        tx_thread_sleep((unsigned long)(g_interval * ONE_SECOND));
    }

    while (1)
    {
        tx_thread_suspend(tx_thread_identify());
    }
}


ssp_err_t gps_init(sf_cellular_ctrl_t* p_ctrl)
{
    ssp_err_t status = SSP_SUCCESS;

    sf_cellular_cmd_resp_t  input_at_command;
    sf_cellular_cmd_resp_t  output;
    uint8_t                 response_buffer[64];
    uint8_t                 atcommand_buffer[64];
    int                     result;
    int                     retries = 0;

    output.p_buff = response_buffer;
    output.buff_len = sizeof(response_buffer);

    input_at_command.p_buff = atcommand_buffer;
    input_at_command.buff_len = sizeof(atcommand_buffer);

    // AT Command to turn off GNSS engine
    output.buff_len = sizeof(response_buffer);
    memset(response_buffer, 0, sizeof(response_buffer));
    memset(atcommand_buffer, 0, sizeof(atcommand_buffer));
    snprintf((char *)atcommand_buffer, sizeof(atcommand_buffer),
             "AT+QGPSEND\r\n");

    status = g_sf_cellular0.p_api->commandSend(p_ctrl, &input_at_command , &output, 50);
    if (SSP_SUCCESS != status)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        return status;
    }

    // AT Command to configure NMEA Sentences Output Port to UART3
    output.buff_len = sizeof(response_buffer);
    memset(response_buffer, 0, sizeof(response_buffer));
    memset(atcommand_buffer, 0, sizeof(atcommand_buffer));
    snprintf((char *)atcommand_buffer, sizeof(atcommand_buffer),
             "AT+QGPSCFG=\"outport\",\"uartnmea\"\r\n");

    status = g_sf_cellular0.p_api->commandSend(p_ctrl, &input_at_command , &output, 50);
    if (SSP_SUCCESS != status)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return status;
    }

    output.p_buff[output.buff_len] = 0;
    result = strcmp((char *)output.p_buff, "\r\nOK\r\n");
    if (0 != result)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return SSP_ERR_INTERNAL;
    }

    // AT Command to Enablee Acquisition of NMEA Sentences via AT+QGPSGNMEA
    output.buff_len = sizeof(response_buffer);
    memset(response_buffer, 0, sizeof(response_buffer));
    memset(atcommand_buffer, 0, sizeof(atcommand_buffer));
    snprintf((char *)atcommand_buffer, sizeof(atcommand_buffer),
             "AT+QGPSCFG=\"nmeasrc\",1\r\n");

    status = g_sf_cellular0.p_api->commandSend(p_ctrl, &input_at_command , &output, 50);
    if (SSP_SUCCESS != status)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return status;
    }

    output.p_buff[output.buff_len] = 0;
    result = strcmp((char *)output.p_buff, "\r\nOK\r\n");
    if (0 != result)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return SSP_ERR_INTERNAL;
    }

    // AT Command to Configure Output Type of GPS NMEA Sentences to GGA
    output.buff_len = sizeof(response_buffer);
    memset(response_buffer, 0, sizeof(response_buffer));
    memset(atcommand_buffer, 0, sizeof(atcommand_buffer));
    snprintf((char *)atcommand_buffer, sizeof(atcommand_buffer),
             "AT+QGPSCFG=\"gpsnmeatype\",1\r\n");

    status = g_sf_cellular0.p_api->commandSend(p_ctrl, &input_at_command , &output, 50);
    if (SSP_SUCCESS != status)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return status;
    }

    output.p_buff[output.buff_len] = 0;
    result = strcmp((char *)output.p_buff, "\r\nOK\r\n");
    if (0 != result)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return SSP_ERR_INTERNAL;
    }

    // AT Command to turn on GNSS engine
    output.buff_len = sizeof(response_buffer);
    memset(response_buffer, 0, sizeof(response_buffer));
    memset(atcommand_buffer, 0, sizeof(atcommand_buffer));
    snprintf((char *)atcommand_buffer, sizeof(atcommand_buffer),
             "AT+QGPS=1,30,50,5,1\r\n");

    status = g_sf_cellular0.p_api->commandSend(p_ctrl, &input_at_command , &output, 50);
    if (SSP_SUCCESS != status)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return status;
    }

    output.p_buff[output.buff_len] = 0;
    result = strcmp((char *)output.p_buff, "\r\nOK\r\n");
    if (0 != result)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return SSP_ERR_INTERNAL;
    }

    // AT Command to enable GNSS to run automatically
    output.buff_len = sizeof(response_buffer);
    memset(response_buffer, 0, sizeof(response_buffer));
    memset(atcommand_buffer, 0, sizeof(atcommand_buffer));
    snprintf((char *)atcommand_buffer, sizeof(atcommand_buffer),
             "AT+QGPSCFG=\"autogps\",1\r\n");

    status = g_sf_cellular0.p_api->commandSend(p_ctrl, &input_at_command , &output, 50);
    if (SSP_SUCCESS != status)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return status;
    }

    output.p_buff[output.buff_len] = 0;
    result = strcmp((char *)output.p_buff, "\r\nOK\r\n");
    if (0 != result)
    {
        //printf("Cellular provisioning failed (error code %d)\n\r", status);
        __BKPT(0);
        return SSP_ERR_INTERNAL;
    }

    while (retries < 5)
    {
        tx_thread_sleep(200);

        // AT Command to read current GNSS state
        output.buff_len = sizeof(response_buffer);
        memset(response_buffer, 0, sizeof(response_buffer));
        memset(atcommand_buffer, 0, sizeof(atcommand_buffer));
        snprintf((char *)atcommand_buffer, sizeof(atcommand_buffer),
                 "AT+QGPS?\r\n");

        status = g_sf_cellular0.p_api->commandSend(p_ctrl, &input_at_command , &output, 50);
        if (SSP_SUCCESS != status)
        {
            //printf("Cellular provisioning failed (error code %d)\n\r", status);
            return status;
        }

        output.p_buff[output.buff_len] = 0;
        result = strcmp((char *)output.p_buff, "\r\n+QGPS: 1\r\n\r\nOK\r\n");
        if (0 == result)
        {
            break;
        }

        retries++;
    }

    if (5 <= retries)
    {
        status = SSP_ERR_INTERNAL;
        __BKPT(0);
    }

    return status;
}

ssp_err_t celr_prov_callback(sf_cellular_callback_args_t * p_args)
{
    ssp_err_t status = SSP_SUCCESS;

    if (SF_CELLULAR_EVENT_PROVISIONSET == p_args->event)
    {

        status = g_sf_cellular0.p_api->provisioningSet(g_sf_cellular0.p_ctrl, &cell_prov_info);

        sf_cellular_ctrl_t * p_ctrl = (sf_cellular_ctrl_t *) p_args->p_context;

        gps_init(p_ctrl);

        status = g_uart_gps.p_api->open(g_uart_gps.p_ctrl, g_uart_gps.p_cfg);
        if(SSP_SUCCESS != status)
        {
            //printf("Failed to initialize UART for GPS module!!!\r\n");
            __BKPT(0);
            return SSP_ERR_INTERNAL;
        }
    }

    return status;
}

void mqtt_client_id_callback(char * client_id, uint32_t * client_id_length)
{
    strcpy(client_id, "Synergy");
    * client_id_length = strlen("Synergy");
}


void mqtt_receive(NXD_MQTT_CLIENT * p_mqtt, UINT count)
{
    UCHAR topic[NXD_MQTT_MAX_TOPIC_NAME_LENGTH];
    UCHAR message[NXD_MQTT_MAX_MESSAGE_LENGTH];
    UINT  topic_length;
    UINT  message_length;
    UINT  status;

    for(UINT i = 0; i < count; i++)
    {
        status = nxd_mqtt_client_message_get(p_mqtt,
                                             topic, sizeof(topic), &topic_length,
                                             message, sizeof(message), &message_length);
        if (NXD_MQTT_SUCCESS == status)
        {
            /* Terminate the string */
            topic[topic_length] = 0;
            message[message_length] = 0;

            if (0 == strcmp((char*)topic, g_interval_topic))
            {
                g_interval = atoi((char*)message);

                if (g_interval <= 20)
                {
                    g_ioport.p_api->pinWrite(GREEN_LED, IOPORT_LEVEL_HIGH);
                }
                else if (g_interval <= 40)
                {
                    g_ioport.p_api->pinWrite(YELLOW_LED, IOPORT_LEVEL_HIGH);
                }
                else if (g_interval <= 50)
                {
                    g_ioport.p_api->pinWrite(RED_LED, IOPORT_LEVEL_HIGH);
                }
                else
                {
                    g_ioport.p_api->pinWrite(GREEN_LED, IOPORT_LEVEL_LOW);
                    g_ioport.p_api->pinWrite(YELLOW_LED, IOPORT_LEVEL_LOW);
                    g_ioport.p_api->pinWrite(RED_LED, IOPORT_LEVEL_LOW);
                }
            }
            else if (0 == strcmp((char*)topic, g_enable_topic))
            {
                if (strstr((char*)message, "START"))
                {
                    g_is_enabled = true;
                    g_ioport.p_api->pinWrite(GREEN_LED, IOPORT_LEVEL_HIGH);
                    g_ioport.p_api->pinWrite(YELLOW_LED, IOPORT_LEVEL_HIGH);
                    g_ioport.p_api->pinWrite(RED_LED, IOPORT_LEVEL_HIGH);
                }
                else
                {
                    g_is_enabled = false;
                    g_ioport.p_api->pinWrite(GREEN_LED, IOPORT_LEVEL_LOW);
                    g_ioport.p_api->pinWrite(YELLOW_LED, IOPORT_LEVEL_LOW);
                    g_ioport.p_api->pinWrite(RED_LED, IOPORT_LEVEL_LOW);
                }
            }
        }
    }
}
