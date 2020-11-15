/*
  EEPROM.h - EEPROM library
  Original Copyright (c) 2006 David A. Mellis.  All right reserved.
*/

#ifndef EEPROM_h
#define EEPROM_h

#include <inttypes.h>

typedef void * EEPtr;

class EEPROMClass{
  public:
    //Basic user access methods.
    //EERef operator[]( const int idx )    { return idx; }
    uint8_t read( int idx )              { return 0;}
    void write( int idx, uint8_t val )   {}
    //void update( int idx, uint8_t val )  {}

    //STL and C++11 iteration capability.
    EEPtr begin()                        { return 0x00; }
    // EEPtr end()                          { return length(); } //Standards requires this to be the item after the last valid entry. The returned pointer is invalid.
    // uint16_t length()                    { return 0; }

    //Functionality to 'get' and 'put' objects to and from EEPROM.
    template< typename T > T &get( int idx, T &t ){
        static_assert(std::is_trivially_copyable<T>::value,"You can not use this type with EEPROM.get" ); // the code below only makes sense if you can "memcpy" T

        // EEPtr e = idx;
        // uint8_t *ptr = (uint8_t*) &t;
        // for( int count = sizeof(T) ; count ; --count, ++e )  *ptr++ = *e;
        // return t;
        return t;
    }

    template< typename T > const T &put( int idx, const T &t ){
//         static_assert(std::is_trivially_copyable<T>::value, "You can not use this type with EEPROM.get"); // the code below only makes sense if you can "memcpy" T

//         const uint8_t *ptr = (const uint8_t*) &t;
// #ifdef __arm__
//         eeprom_write_block(ptr, (void *)idx, sizeof(T));
// #else
//         EEPtr e = idx;
//         for( int count = sizeof(T) ; count ; --count, ++e )  (*e).update( *ptr++ );
// #endif
//         return t;
        return t;
    }

    template< typename T > const T &update( int idx, const T &t ){
//         static_assert(std::is_trivially_copyable<T>::value, "You can not use this type with EEPROM.get"); // the code below only makes sense if you can "memcpy" T

//         const uint8_t *ptr = (const uint8_t*) &t;
// #ifdef __arm__
//         eeprom_write_block(ptr, (void *)idx, sizeof(T));
// #else
//         EEPtr e = idx;
//         for( int count = sizeof(T) ; count ; --count, ++e )  (*e).update( *ptr++ );
// #endif
//         return t;
        return t;
    }
} EEPROM;

#endif
