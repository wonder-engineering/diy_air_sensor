// Copyright 2020 Brett M. Leedy

#include "LogFileSender.hh"

LogFileSender::LogFileSender() {
  // init software serial port
  this->set_pinmode(SOFTWARESERIAL_RX_PIN, INPUT);
  this->set_pinmode(SOFTWARESERIAL_TX_PIN, OUTPUT);
  this->serialPort = new SoftwareSerial(SOFTWARESERIAL_RX_PIN,
                                        SOFTWARESERIAL_TX_PIN);
  this->serialPort->begin(SOFTWARESERIAL_BAUD);
}

void LogFileSender::open_line(uint32_t id, uint32_t timestamp) {

  // Transmit the start character 3 times
  for (int index = 0; index < 3; index++) {
    this->serialPort->write(HEADER_START);
  }

  // Transmit the current file ID
  this->serialPort->write(this->current_id);

  // Transmit the start of text delimiter
  this->serialPort->write(START_TEXT);
  // Now we will write everything that comes over the port to a file
  // todo: maybe implement NACK checking for writer failures

  // Write sender-side ID and timestamp
  this->serialPort->print(id);
  this->serialPort->print(F(","));
  this->serialPort->print(timestamp);
  this->serialPort->print(F(","));
}

void LogFileSender::write_field(uint32_t field) {
  delay(5);  // todo: temporary
  this->serialPort->print(field);
  this->serialPort->print(F(","));
}

void LogFileSender::write_field(float field) {
  delay(5);  // todo: temporary
  this->serialPort->print(field, 2);
  this->serialPort->print(F(","));
}

void LogFileSender::write_field(const char * field) {
  this->serialPort->print(F(","));
}

void LogFileSender::close_line() {
  // Write the delimiter for the end of text
  for (int index = 0; index < 3; index++) {
    this->serialPort->write(END_TEXT);
  }
}

void LogFileSender::rotate_file() {
  this->current_id++;
}

void LogFileSender::override_file_number(uint8_t new_id) {
  this->current_id = new_id;
}

void LogFileSender::set_pinmode(uint8_t pin, uint8_t flags) {
  pinMode(pin, flags);  // just set pinmode
}
