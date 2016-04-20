/*
 * File Name : nmea.h
 * Author :ram krishnan (rkris@wisense.in)
 * Created : Nov/15/2015
 *
 *
 * Copyright (c) <2015>, <ram krishnan>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the names of its contributors
 *     may be used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#ifndef __NMEA_H__
#define __NMEA_H__

/*
 * NMEA stands for national maritime electronics association. All GPS mfrs
 * support the NMEA format.
 */

/*
 * <From http://www.gpsinformation.org/dale/nmea.htm>
 *
 * Each sentence begins with a '$' and ends with a carriage return/line feed
 * sequence and can be no longer than 80 characters of visible text (plus the
 * line terminators). The data is contained within this single line with data
 * items separated by commas. The data itself is just ascii text and may extend
 * over multiple sentences in certain specialized instances but is normally
 * fully contained in one variable length sentence. The data may vary in the
 * amount of precision contained in the message. For example time might be
 * indicated to decimal parts of a second or location may be show with 3 or even
 * 4 digits after the decimal point. Programs that read the data should only use
 * the commas to determine the field boundaries and not depend on column positions.
 * There is a provision for a checksum at the end of each sentence which may or
 * may not be checked by the unit that reads the data. The checksum field consists
 * of a '*' and two hex digits representing an 8 bit exclusive OR of all characters
 * between, but not including, the '$' and '*'. A checksum is required on some
 * sentences.
 */


#define NMEA_MSG_MAX_LEN  (80 + 2)


#define NMEA_GPS_MSG_TYPE_AAM  0x00  // Waypoint Arrival Alarm
#define NMEA_GPS_MSG_TYPE_ALM  0x01  // Almanac data
#define NMEA_GPS_MSG_TYPE_APA  0x02  // Auto Pilot A sentence
#define NMEA_GPS_MSG_TYPE_APB  0x03  // Auto Pilot B sentence
#define NMEA_GPS_MSG_TYPE_BOD  0x04  // Bearing Origin to Destination
#define NMEA_GPS_MSG_TYPE_BWC  0x05  // Bearing using Great Circle route
#define NMEA_GPS_MSG_TYPE_DTM  0x06  // Datum being used.
#define NMEA_GPS_MSG_TYPE_GGA  0x07  // Fix information
#define NMEA_GPS_MSG_TYPE_GLL  0x08  // Lat/Lon data
#define NMEA_GPS_MSG_TYPE_GRS  0x09  // GPS Range Residuals
#define NMEA_GPS_MSG_TYPE_GSA  0x0a  // Overall Satellite data
#define NMEA_GPS_MSG_TYPE_GST  0x0b  // GPS Pseudorange Noise Statistics
#define NMEA_GPS_MSG_TYPE_GSV  0x0c  // Detailed Satellite data
#define NMEA_GPS_MSG_TYPE_MSK  0x0d  // send control for a beacon receiver
#define NMEA_GPS_MSG_TYPE_MSS  0x0e  // Beacon receiver status information.
#define NMEA_GPS_MSG_TYPE_RMA  0x0f  // recommended Loran data
#define NMEA_GPS_MSG_TYPE_RMB  0x10  // recommended navigation data for gps
#define NMEA_GPS_MSG_TYPE_RMC  0x11  // recommended minimum data for gps
#define NMEA_GPS_MSG_TYPE_RTE  0x12  // route message
#define NMEA_GPS_MSG_TYPE_TRF  0x13  // Transit Fix Data
#define NMEA_GPS_MSG_TYPE_STN  0x14  // Multiple Data ID
#define NMEA_GPS_MSG_TYPE_VBW  0x15  // dual Ground / Water Spped
#define NMEA_GPS_MSG_TYPE_VTG  0x16  // Vector track an Speed over the Ground
#define NMEA_GPS_MSG_TYPE_WCV  0x17  // Waypoint closure velocity (Velocity Made Good)
#define NMEA_GPS_MSG_TYPE_WPL  0x18  // Waypoint Location information
#define NMEA_GPS_MSG_TYPE_XTC  0x19  // cross track error
#define NMEA_GPS_MSG_TYPE_XTE  0x1a  // measured cross track error
#define NMEA_GPS_MSG_TYPE_ZTG  0x1b  // Zulu (UTC) time and time to go (to destination)
#define NMEA_GPS_MSG_TYPE_ZDA  0x1c  // Date and Time

#endif
