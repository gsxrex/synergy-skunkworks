#ifndef IOT_H_
#define IOT_H_

#include "quectel_gps.h"

/*
 * different APN (access point names) for different cellular
 * service providers
 */
//#define CELLULAR_APN            ("10569.mcs")           // Digikey AT&T
//#define CELLULAR_APN            ("m2m.com.attz")        // AT&T
//#define CELLULAR_APN            ("mnet.bell.ca.ioe")    // Bell Canada
#if !defined(CELLULAR_APN)
#error "Cellular APN not defined!"
#endif

#define ADAFRUIT_SERVER         ("io.adafruit.com")

/*
 * login details for adafruit user account
 * https://io.adafruit.com
 */
//#define ADAFRUIT_USERNAME       ("") /* *** Adafruit username *** */
//#define ADAFRUIT_KEY            ("") /* *** Adafruit IO key *** */
#if !defined(ADAFRUIT_USERNAME) || !defined(ADAFRUIT_KEY)
#error "Adafruit IO credentials not defined!"
#endif

#endif /* IOT_H_ */
