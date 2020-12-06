// Copyright 2020 Brett M. Leedy

// #define PARSING_VERBOSE
#ifdef PARSING_VERBOSE
  #define DEBUG(debug_text) Serial.println(F(debug_text));
  #define DEBUG_VAL(debug_text, debug_value) \
      Serial.print(F(debug_text)); Serial.println(debug_value);
#else
  #define DEBUG(debug_text)
  #define DEBUG_VAL(debug_text, debug_value)
#endif

#include "LogFileWriter.hh"

LogFileWriter::LogFileWriter() {
  this->sd = new Sd_i();
  this->re_init_sd();
  // init softwareserial
  pinMode(SOFTWARESERIAL_RX_PIN, INPUT);
  pinMode(SOFTWARESERIAL_TX_PIN, OUTPUT);
  this->serialPort = new SoftwareSerial(SOFTWARESERIAL_RX_PIN,
                                        SOFTWARESERIAL_TX_PIN);
  this->serialPort->begin(SOFTWARESERIAL_BAUD);
}

void LogFileWriter::listen_for_line() {
  while (this->serialPort->available() > 0) {
    uint8_t newbyte = serialPort->read();
    DEBUG_VAL("LogFileWriter: Received byte ", newbyte);
    DEBUG_VAL("LogFileWriter: in state ", this->parserState);
    this->parserState = this->update_state(newbyte, this->parserState);
    switch (this->parserState) {
      case initState:
      case headerReceivedState:
      case delimiterReceivedState:
        // do not write anything
        break;
      case fileIdReceivedState:
        // check that this file id is the same as before
        if (newbyte == this->host_file_id) {
          // nothing to do
        } else {
          // increment the file number we're writing (if needed)
          DEBUG_VAL("Found new host id:", newbyte);
          DEBUG_VAL("previous:", this->host_file_id);
          this->highest_used_id++;
          set_new_filename(this->highest_used_id);
          this->host_file_id = newbyte;
        }
        // open the file
        this->open_line(this->highest_used_id, millis());
        break;
      case writingBodyState:
        // write the byte to the file
        this->file.write(newbyte);
        Serial.write(newbyte);
        break;
      case protocolFailureState:
        // write a warning message to hardwareserial and the file
        if (this->file) {
          this->file.print(F("PROTOCOL FAILURE"));
        }
      case endlineReceivedState:
        // write '\n' and close the file
        this->close_line();
        break;
    }
  }
}


// State machine for the parser
ParsingState LogFileWriter::update_state(uint8_t byte, ParsingState oldState) {
  ParsingState newState = protocolFailureState;

  switch (oldState) {
    case initState:
      // uninitialized can only transition to headerReceived state
      // requires an "0xFF" to switch state
      if (byte == HEADER_START) {
        newState = headerReceivedState;
        DEBUG("InitState: Switching to headerReceivedState");
      } else {   // else just skip to next byte
        newState = oldState;
      }
      break;
    case headerReceivedState:
      // at least one 0xFF has been received.  Stay in this state until
      // anything but 0xFF is received
      if (byte != HEADER_START) {
        newState = fileIdReceivedState;
        DEBUG("Switching to fileIdReceivedState");
      } else {
        newState = oldState;
      }
      break;
    case fileIdReceivedState:
      // expect a delimiter as the only byte we should receive in this state.
      if (byte == START_TEXT) {  // success
        newState = delimiterReceivedState;
        DEBUG("Switching to delimiterReceivedState");
      } else {  // failure
        Serial.println(F("Protocol failure! Delimiter not found!"));
        newState = protocolFailureState;
      }
      break;
    case delimiterReceivedState:
      // now that the delimiter was received, init row bytes written
      // and begin writting body bytes
      this->parserRowBytes = 0;
      newState = writingBodyState;
      DEBUG("Switching to writingBodyState");
      break;
    case writingBodyState:
      // if we receive a newline, go to new state
      if (byte == END_TEXT) {
        newState = endlineReceivedState;
        DEBUG("Switching to endlineReceivedState");
      } else if (byte < ASCII_BODY_MIN ||
                  byte > ASCII_BODY_MAX) {  // out of legal range
        Serial.println(
          F("Protocol failure! Body text outside of ASCII range!"));
        newState = protocolFailureState;
      } else if (++this->parserRowBytes > MAX_BYTES_PER_ROW) {
        Serial.println(F("Protocol failure! row too long!"));
        newState = protocolFailureState;
      } else {
        // just keep writing (bytes written is captured above)
        DEBUG("Just keep writing body");
        newState = oldState;
      }
      break;
    case protocolFailureState:
    case endlineReceivedState:  // start afresh if we failed or succeeded
    default:
      // move on to the next state, either straight to header, or init
      if (byte == HEADER_START) {
        newState = headerReceivedState;
        DEBUG("default: Switching to headerReceivedState");
      } else {
        newState = initState;
        DEBUG("Switching to initState");
      }
      break;
  }
  return newState;
}

void LogFileWriter::set_new_filename(uint16_t file_number) {
    snprintf(this->current_name, MAX_FILENAME_LEN, "%s-%d.%s",
              log_file_name_base, file_number, LOGFILE_EXTENSION);
    Serial.print(F("Logfile: File name is "));
    Serial.print(this->current_name); Serial.println("|");
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

  if (this->sd_failure) {
    if (this->sd != NULL)
      this->sd->end();  // needed to de-allocate any memory
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

bool LogFileWriter::is_sd_failed() {
  if (sd_failure)      // check whether we're marked for failure
    if (re_init_sd())  // attempt to re-init
      return true;    // return true of re-init fails

  return false;  // otherwise, return false
}

void LogFileWriter::open_line(uint32_t id, uint32_t timestamp) {
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
  Serial.println("");
}

void LogFileWriter::rotate_file() {
  this->override_file_number(this->highest_used_id+1);
}

void LogFileWriter::override_file_number(uint16_t new_id) {
  this->highest_used_id = new_id;

  snprintf(this->current_name, MAX_FILENAME_LEN, "%s-%d.%s",
           log_file_name_base, highest_used_id,
           LOGFILE_EXTENSION);

  Serial.print(F("Logfile: File name is now"));
  Serial.println(this->current_name);
}

void LogFileWriter::set_pinmode(uint8_t pin, uint8_t flags) {
        pinMode(pin, flags);  // just set pinmode
}

void LogFileWriter::replace_sd_interface(Sd_i * interface) {
        this->sd = interface;
}
