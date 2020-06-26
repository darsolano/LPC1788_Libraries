/*
 * define_pins.h
 *
 *  Created on: Apr 1, 2016
 *      Author: dsolano
 *      LPC-1769 LPCXpresso1769 with EA Baseboard
 */

#ifndef INCLUDE_DEFINE_PINS_H_
#define INCLUDE_DEFINE_PINS_H_

/* define_pins.h
 *
 * Define a useful macro for giving GPIO pins nicer, semantic names.
 *
 * The only macro exported is DEFINE_PIN. Use as:
 *  DEFINE_PIN(PIN_NAME, port number, pin number)
 * this will define the following:
 *  PIN_NAME_DEASSERT(), _OFF(), _LOW() to bring the pin low
 *  PIN_NAME_ASSERT(), _ON(), _HIGH() to bring the pin high
 *  PIN_NAME_INPUT() to make the pin an input
 *  PIN_NAME_OUTPUT() to make the pin an output
 *  PIN_NAME_READ() to read the current state of the pin
 *                  (returns a 1 or 0, typed as a uint_fast8_t)
 */

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>

#define DEFINE_PIN(name, port, pin) \
inline static void name##_OFF() { \
  LPC_GPIO[port].CLR |= (1 << pin); \
} \
inline static void name##_LOW() { \
  LPC_GPIO[port].CLR |= (1 << pin); \
} \
inline static void name##_ON() { \
  LPC_GPIO[port].SET |= (1 << pin); \
} \
inline static void name##_HIGH() { \
  LPC_GPIO[port].SET |= (1 << pin); \
} \
inline static void name##_TOGGLE() { \
  LPC_GPIO[port].PIN ^= (1 << pin); \
} \
inline static void name##_INPUT() { \
  LPC_GPIO[port].DIR &= ~(1UL << pin); \
} \
inline static void name##_OUTPUT() { \
  LPC_GPIO[port].DIR |= (1UL << pin); \
} \
inline static bool name##_READ() { \
  return (( LPC_GPIO[port].PIN >> pin) & 1); \
}\
inline static void name##_DESELECTED() { \
  LPC_GPIO[port].SET |= (1 << pin); \
} \
inline static void name##_SELECTED() { \
  LPC_GPIO[port].CLR |= (1 << pin); \
}


#endif /* INCLUDE_DEFINE_PINS_H_ */
