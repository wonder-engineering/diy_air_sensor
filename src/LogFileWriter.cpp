// Copyright 2020 Brett M. Leedy

#include "LogFile.h"

#define HEADER_START  0x01   // ASCII Start Of Header
#define START_TEXT    0x02   // ASCII Start of Text
#define END_TEXT      0x03   // ASCII End of Text
#define ASCII_BODY_MAX 126
#define ASCII_BODY_MIN  32

LogFileWriter::LogFileWriter() {
  this->sd = new Sd_i();
  this->re_init_sd();
  // init softwareserial
  this->serialPort = new SoftwareSerial(2, 3); // RX, TX
}

LogFileWriter::listen_for_line() {
  while(this->serialPort.available() > 0) {
    uint8_t byte = serialPort.read();
    switch (this->update_state(byte, this->parserState)){
      case initState:
      case headerReceivedState:
      case delimiterRecievedState:
        // do not write anything
        break;
      case fileIdReceivedState:
        // check that this file id is the same as before
        if(byte = this->host_file_id) {
          // nothing to do
        } else {
          // increment the file number we're writing (if needed)
          this->highest_used_id++;
          set_new_filename(this->highest_used_id);
        }
        // open the file
        this->open_line(this->highest_used_id, millis());
        break;
      case writingBodyState:
        // write the byte to the file
        this->file->write(byte);
        break;
      case protocolFailureState:
        // write a warning message to hardwareserial and the file
        if(this->file){
          this->file->print(F("PROTOCOL FAILURE"));
        }
      case endlineReceivedState:
        // write '\n' to the file
        this->file->println("");
        // close the file
        this->file->close();
        break;
    }
    state_machine(byte;);
  }
}


// State machine for the parser
LogFileWriter::update_state(uint8_t byte, ParsingState oldstate) {
  ParsingState newState = protocolFailureState;

  switch (oldstate) {
    case initState:
      // uninitialized can only transition to headerReceived state
      // requires an "0xFF" to switch state
      if (byte == HEADER_START) {
        newState = headerReceivedState;
      }  // else just skip to next byte
      break;
    case headerReceivedState:
      // at least one 0xFF has been received.  Stay in this state until
      // anything but 0xFF is received
      if (byte != HEADER_START) {
        newState = fileIdReceivedState;
      }  // else just skip to next byte
      break;
    case fileIdReceivedState:
      // expect a delimiter as the only byte we should receive in this state.
      if (byte == START_TEXT) {  // success
        newState = delimiterReceivedState;
      } else { // failure
        Serial.println(F{"Protocol failure! Delimiter not found!"});
        newState = protocolFailureState;
      }
      break;
    case delimiterReceivedState:
      // now that the delimiter was received, init row bytes written
      // and begin writting body bytes
      this->parserRowBytes=0;
      newState = writingBodyState;
      break;
    case writingBodyState:
      // if we receive a newline, go to new state
      if(byte == END_TEXT) {
        newState = endlineRecievedState;
      } else if (byte < ASCII_BODY_MIN || byte > ASCII_BODY_MAX) { // out of legal range
        Serial.println(F{"Protocol failure! Body text outside of ASCII range!"});
        newState = protocolFailureState;
      } else if(++this->parserRowBytes > MAX_BYTES_PER_ROW) {
        Serial.println(F{"Protocol failure! row too long!"});
        newState = protocolFailureState;
      } else {
        // just keep writing (bytes written is captured above)
      }
      break;
    case endlineRecievedState:  // start afresh if we failed or succeeded
    case protocolFailureState:
    default:
      // move on to the next state, either straight to header, or init
      if (byte == HEADER_START) {
        newState = headerReceivedState;
      } else {
        newState = initState;
      }
      break;
  }
  return newState;
}

void logFileWriter::set_new_filename(uint16_t file_number) {
    snprintf(current_name, MAX_FILENAME_LEN, "%s-%d.%s",
              log_file_name_base, file_number, LOGFILE_EXTENSION);
    Serial.print(F("Logfile: File name is "));
    Serial.print(current_name); Serial.println("|");
}

bool LogFileWriter::re_init_sd() {
  // don't try to re-init if cooldown has not expired
  uint16_t nowtime = this->get_millis();
  if ((nowtime >= this->cooldown_start_millis) &&  // millis hasn't wrapped
      (nowtime < this->cooldown_start_millis +
                          SD_COOLDOWN_LENGTH) &&  // waited for the cooldn
      (!this->is_first_run())                     // we've run this once
    ) {
    return this->sd_failure;
  } else {
    this->cooldown_start_millis = nowtime;  // we  ran, so reset
  }

  if(this->sd_failure) {
    this->sd->end();  // needed to de-allocate any memory todo: may need nullcheck to not crash
    // todo: consider state machine for this, since it's getting messy:
    // https://forum.arduino.cc/index.php?topic=565437.0
  }

  // Init the SD card
  this->set_pinmode(10, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.println(F("LogFile: Init SD card..."));
  this->sd_failure = !this->begin_sd();  // begin returns false on failure
  if (!sd_failure) {
    // Init the file name
    this->highest_used_id = this->get_highest_used_id() + 1;
    set_new_filename(this->highest_used_id);
  } else {
    // Failed to init SD, print error and fail out
    Serial.println(F("LogFile: ERROR - Failed to init SD card!"));
  }
  return this->sd_failure;
}

bool LogFileWriter::begin_sd() {
        return this->sd->begin(10);
}

void LogFileWriter::get_file_name(char * buffer, uint8_t max_size) {
  uint8_t maxlen = MAX_FILENAME_LEN;
  if (max_size < maxlen)
    maxlen = max_size;   // take the smallest max size

  strncpy(buffer, this->current_name, max_size);
}

char * LogFileWriter::get_file_name_ptr() {
  return this->current_name;
}

bool LogFileWriter::is_sd_failed() {
  if (sd_failure)      // check whether we're marked for failure
    if (re_init_sd())  // attempt to re-init
      return true;    // return true of re-init fails

  return false;  // otherwise, return false
}

void LogFileWriter::open_line(uint16_t id, uint16_t timestamp) {
  if (is_sd_failed())
    return;
  this->file.close();
  this->file = this->sd->open(this->current_name, FILE_WRITE);
  if (this->file) {
    // success
  } else {
    Serial.println(F("error opening log file:"));
    Serial.println(this->current_name);
    Serial.println(this->file);
    this->sd_failure = true;
  }

  this->file.print(id);
  this->file.print(F(","));
  this->file.print(timestamp);
  this->file.print(F(","));
}

uint16_t LogFileWriter::get_highest_used_id() {
  if (is_sd_failed())
    return 0;

  uint16_t highest_number_found = 0;
  File dir;
  dir = this->sd->open("/", FILE_READ);
  // check that it opened and is a directory
  if (!dir) {
    // could not open directory
    Serial.println(F("Could not open directory"));
    return 0;
  } else if (!dir.isDirectory()) {
    // not a directory
    Serial.println(F("Not a directory"));
    return 0;
  }

  // check that it is a dir
  char temp_filename[MAX_FILENAME_LEN];
  while (true) {
    File entry =  dir.openNextFile();
    if (entry == 0)
      break;  // no more files

    // if entry.name() matches our file prefix, check the highest number
    // copy the string to tokenize
    char * token;
    strncpy(temp_filename, entry.name(), MAX_FILENAME_LEN);
    // string is of the format "LOGFILE_NAME_BASE.number.csv"
    token = strtok(temp_filename, "-");  // NOLINT

    // compare the first token to the filename
    if (strncmp(token, log_file_name_base, MAX_FILENAME_LEN) != 0) {
      continue;  // different base name - ignore
    }

    token = strtok(NULL, ".");    // NOLINT
    // convert and compare the number token (if found)
    uint16_t file_number = atoi(token);
    if (file_number > highest_number_found) {
      highest_number_found = file_number;
    }
    entry.close();
  }  // while(true)

  dir.close();
  Serial.print(F("Highest file number found: "));
  Serial.println(highest_number_found);
  return highest_number_found;
}

void LogFileWriter::close_line() {
  if (is_sd_failed())
    return;
  this->file.println("");
  this->file.close();
}

void LogFileWriter::rotate_file() {
  this->override_file_number(this->highest_used_id+1);
}

void LogFileWriter::override_file_number(uint16_t new_id) {
  this->highest_used_id = new_id;

  snprintf(current_name, MAX_FILENAME_LEN, "%s-%d.%s",
           log_file_name_base, highest_used_id,
           LOGFILE_EXTENSION);

  Serial.print(F("Logfile: File name is now")); Serial.println(current_name);
}

File * LogFileWriter::get_file_ptr() {
  // todo: make this safer
  return &this->file;
}

void LogFileWriter::set_pinmode(uint8_t pin, uint8_t flags) {
        pinMode(pin, flags);  // just set pinmode
}

void LogFileWriter::replace_sd_interface(Sd_i * interface) {
        this->sd = interface;
}
