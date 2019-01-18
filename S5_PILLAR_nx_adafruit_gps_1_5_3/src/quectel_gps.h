#ifndef QUECTEL_GPS_H_
#define QUECTEL_GPS_H_

/*
 * token map for GGA NMEA string
 * 0:       preamble
 * 1:       UTC time
 * 2:       latitude
 * 3:       direction (N/S)
 * 4:       longitude
 * 5:       direction (E/W)
 * 6:       fix type (1-> GPS fix)
 * 7:       # of satellites
 * 8:       horizontal dilution
 * 9,10:    altitude
 * 11,12:   geoid
 * 13:      blank
 * 14:      end of NMEA string
 */
#define GGA_PREAMBLE        "$GPGGA"
#define GGA_END             '*'
#define GGA_DELIMITER       ','
#define TOKEN_COUNT         15
#define TOKEN_LENGTH        15
#define GGA_LATITUDE        2
#define GGA_LAT_DIRECTION   3
#define GGA_LONGITUDE       4
#define GGA_LONG_DIRECTION  5
#define GGA_ALTITUDE        9
#define GGA_NORTH           'N'
#define GGA_SOUTH           'S'
#define GGA_EAST            'E'
#define GGA_WEST            'W'

#endif /* QUECTEL_GPS_H_ */
