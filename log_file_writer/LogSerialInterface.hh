// Copyright 2020 Brett M. Leedy

/*
  Log serial interface shared constants


  Log Lines Protocol:
  0x01 0x01 0x01              |   3 bytes header
  uint8_t                     |   Session File ID (starts at zero at powerup)
  '|'                         |   delimiter
  <arbitrary amount of data>  |   body data to be written
  0x03 0x03 0x03              |   3 bytes endline delimiter

*/

#ifndef LOG_FILE_WRITER_LOGSERIALINTERFACE_HH_
#define LOG_FILE_WRITER_LOGSERIALINTERFACE_HH_

#define HEADER_START  0x01   // ASCII Start Of Header
#define START_TEXT    0x02   // ASCII Start of Text
#define END_TEXT      0x03   // ASCII End of Text
#define SOFTWARESERIAL_BAUD  9600
#define SOFTWARESERIAL_RX_PIN  2
#define SOFTWARESERIAL_TX_PIN  3

#define ASCII_BODY_MAX 126
#define ASCII_BODY_MIN  32
#define MAX_BYTES_PER_ROW 200

#endif  // LOG_FILE_WRITER_LOGSERIALINTERFACE_HH_
