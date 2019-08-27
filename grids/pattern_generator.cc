// Copyright 2011 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// Global clock.

#include "grids/pattern_generator.h"

#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "avrlib/op.h"

#include "grids/resources.h"

namespace grids {

using namespace avrlib;

uint8_t KNOB_RANGE = 256;

/* static */
Options PatternGenerator::options_;

/* static */
uint8_t PatternGenerator::pulse_;

/* static */
uint8_t PatternGenerator::pulse_index_;

/* static */
uint8_t PatternGenerator::step_;

/* static */
bool PatternGenerator::first_beat_;

/* static */
bool PatternGenerator::beat_;

/* static */
uint8_t PatternGenerator::euclidean_step_[kNumParts];

/* static */
uint8_t PatternGenerator::state_;

/* static */
uint8_t PatternGenerator::pulse_duration_counter_;

/* static */
uint8_t PatternGenerator::part_perturbation_[kNumParts];

/* static */
PatternGeneratorSettings PatternGenerator::settings_;

/* static */
uint8_t PatternGenerator::factory_testing_;

/* extern */
PatternGenerator pattern_generator;

// static const prog_uint8_t* drum_map[5][5] = {
//   { node_10, node_8, node_0, node_9, node_11 },
//   { node_15, node_7, node_13, node_12, node_6 },
//   { node_18, node_14, node_4, node_5, node_3 },
//   { node_23, node_16, node_21, node_1, node_2 },
//   { node_24, node_19, node_17, node_20, node_22 },
// };

// static const uint8_t swing_patterns_lookup[11][12] = {
//   // Swing Pattern Generated from Rakefile - 63 == OFF
// { 0, 6, 63, 7, 63, 8, 63, 9, 63, 10, 63, 11 },
// { 0, 3, 6, 63, 7, 63, 8, 9, 63, 10, 63, 11 },
// { 0, 2, 3, 6, 63, 7, 63, 8, 9, 63, 10, 11 },
// { 0, 2, 3, 5, 6, 63, 7, 63, 8, 9, 10, 11 },
// { 0, 1, 2, 3, 4, 6, 4, 5, 8, 9, 10, 11 },
// { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
// { 0, 63, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11 },
// { 0, 63, 1, 2, 3, 63, 4, 5, 6, 8, 9, 11 },
// { 0, 63, 1, 63, 2, 3, 63, 4, 5, 6, 9, 11 },
// { 0, 63, 1, 63, 2, 63, 3, 63, 4, 5, 6, 9 },
// { 0, 63, 1, 63, 2, 63, 3, 63, 4, 63, 5, 6 }
// };
static const uint8_t swing_patterns_lookup[16][24] = {
{  0,  5,  8, 10, 11, 13, 14, 15, 16, 17, 18, 19, 99, 20, 21, 99, 22, 99, 99, 23, 99, 99, 99, 99 }, // 0 swing: 0.03
{  0,  5,  7,  9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 99, 20, 21, 99, 22, 99, 23, 99, 99, 99, 99 }, // 1 swing: 0.08
{  0,  4,  6,  8,  9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 99, 20, 21, 99, 22, 99, 23, 99, 99, 99 }, // 2 swing: 0.14
{  0,  3,  5,  7,  8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 99, 20, 21, 99, 22, 99, 23, 99, 99 }, // 3 swing: 0.19
{  0,  3,  4,  6,  7,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 99, 20, 21, 99, 22, 23, 99, 99 }, // 4 swing: 0.25
{  0,  2,  4,  5,  6,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 99, 20, 21, 22, 99, 23, 99 }, // 5 swing: 0.31
{  0,  2,  3,  4,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 99, 21, 22, 23, 99 }, // 6 swing: 0.36
{  0,  1,  2,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 99, 22, 23 }, // 7 swing: 0.42
{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 }, // 8 swing: 0.47
{  0, 99,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 }, // 9 swing: 0.53
{  0, 99,  1,  2, 99,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22 }, // 10 swing: 0.58
{  0, 99, 99,  1,  2,  3, 99,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21 }, // 11 swing: 0.64
{  0, 99, 99,  1, 99,  2,  3,  4, 99,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21 }, // 12 swing: 0.69
{  0, 99, 99, 99,  1,  2, 99,  3,  4, 99,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 16, 17, 19, 20 }, // 13 swing: 0.75
{  0, 99, 99, 99,  1, 99,  2, 99,  3,  4, 99,  5,  6,  7,  8,  9, 10, 11, 12, 13, 15, 16, 18, 20 }, // 14 swing: 0.81
{  0, 99, 99, 99, 99,  1, 99,  2, 99,  3,  4, 99,  5,  6,  7,  8,  9, 10, 11, 12, 14, 15, 17, 19 }, // 15 swing: 0.86
};

static const prog_uint8_t* flat_drum_map[16] = {
  node_0, node_1, node_2, node_3, node_4, node_5, node_6, node_7, node_8, node_9, node_10, node_11, node_12, node_13, node_14, node_15
};

bool PatternGenerator::triplets(){
  return settings_.options.drums.x >> 4 == 9; // TODO: move this into a data structure
}

/* static */
uint8_t PatternGenerator::ReadDrumMap(
    uint8_t step,
    uint8_t instrument,
    uint8_t x,
    uint8_t y) {
  //  ditch most of this and just have the X knob
  // select from one of 16 patterns and save the Y knob for something else
  uint8_t i = x >> 4;

  // const prog_uint8_t* a_map = drum_map[i][j];
  // const prog_uint8_t* b_map = drum_map[i + 1][j];
  // const prog_uint8_t* c_map = drum_map[i][j + 1];
  // const prog_uint8_t* d_map = drum_map[i + 1][j + 1];
  const prog_uint8_t* map = flat_drum_map[i];
  uint8_t offset = (instrument * stepsPerPattern()) + step;
  // uint8_t a = pgm_read_byte(a_map + offset);
  // uint8_t b = pgm_read_byte(b_map + offset);
  // uint8_t c = pgm_read_byte(c_map + offset);
  // uint8_t d = pgm_read_byte(d_map + offset);
  // return U8Mix(U8Mix(a, b, x << 2), U8Mix(c, d, x << 2), y << 2);
  return pgm_read_byte(map + offset);
}

/* static */
void PatternGenerator::EvaluateDrums() {
  // Kill part perturbation because of problems compiling if I
  // add additional logic!
  // `unable to find a register to spill in class 'POINTER_REGS'`

  // At the beginning of a pattern, decide on perturbation levels.
  // if (step_ == 0) {
  //   for (uint8_t i = 0; i < kNumParts; ++i) {
  //     uint8_t randomness = options_.swing
  //         ? 0 : settings_.options.drums.randomness >> 2;
  //     part_perturbation_[i] = U8U8MulShift8(Random::GetByte(), randomness);
  //   }
  // }

  // randomness = the chaos knob, but I'm using it to offset the snare pattern

  uint8_t instrument_mask = 1;
  uint8_t x = settings_.options.drums.x;
  uint8_t y = settings_.options.drums.y;
  uint8_t accent_bits = 0;
  uint8_t swing_pattern_index = settings_.options.drums.randomness / 16; // 
    
  for (uint8_t i = 0; i < kNumParts; ++i) {
    bool isSnarePart = i == 1;
    bool isHihatPart = i == 2;

    // use  settings_.options.drums.randomness to control "swing"
    //For 16th swing we want to group in sets of 12, stretching out the first 6 and contracting the second 6
    // |.....:.....|.....:.....|.....:.....| 50%
    // |<---->:>--<|<---->:>--<|<---->:>--<| 60%
    // |<----->:>-<|<----->:>-<|<----->:>-<| 70%
    // |<------>:><|<------>:><|<------>:><| 80%
    uint8_t swung_offset = swing_patterns_lookup[swing_pattern_index][step_ % 24];
    uint8_t stepNumber = (step_ - (step_ % 24)) + swung_offset;
    uint8_t level = swung_offset == 99 ? 0 : ReadDrumMap(stepNumber, i, x, y);

    // if (level < 255 - part_perturbation_[i]) {
    //   level += part_perturbation_[i];
    // } else {
    //   // The sequencer from Anushri uses a weird clipping rule here. Comment
    //   // this line to reproduce its behavior.
    //   level = 255;
    // }
    uint8_t threshold = ~settings_.density[i];
    if (level > threshold) {
      if (level > 192) {
        if ((!isHihatPart && !isSnarePart) || (isHihatPart && y > 100)) {
          accent_bits |= instrument_mask;
        }
      }
      state_ |= instrument_mask;
    }
    instrument_mask <<= 1;
  }
  uint8_t snareDensity = 256 - settings_.density[1]; // trigger resolution from snare density
  uint8_t power = snareDensity >> 4;                 // 256 >> 5 == 8
  // temporarily send to kick accent bit since snare bit is being weird
  if (power > 0 && step_ % (1 << (power - 1)) == 0) {
    accent_bits |= 1; // turn on kick accent bit
  }  else  {
    accent_bits &= ~1; // turn off kick accent bit
  }

  // Killing the output_clock mode because it keeps getting reset to this mode
  // every time I upload to the board
  // if (output_clock()) {
  //   state_ |= accent_bits ? OUTPUT_BIT_COMMON : 0;
  //   state_ |= step_ == 0 ? OUTPUT_BIT_RESET : 0;
  // } else {
    state_ |= accent_bits << 3;
  // }
}
// pulse index is 0..<96
// we swing on the basis of 96/4 = 24
// but our patterns are  defined on a 1/32 basis  (96/32 = 3)
void PatternGenerator::EvaluateMFDrums(uint8_t pulse_index) {
  uint8_t instrument_mask = 1;
  uint8_t x = settings_.options.drums.x;
  uint8_t y = settings_.options.drums.y;
  uint8_t accent_bits = 0;
  uint8_t swing_pattern_index = settings_.options.drums.randomness / 16; // 
  
  if(pulse_ == 0){
    uint8_t snareDensity = 256 - settings_.density[1]; // trigger resolution from snare density
    uint8_t power = snareDensity >> 4;                 // 256 >> 5 == 8
    // temporarily send to kick accent bit since snare bit is being weird
    if (power > 0 && step_ % (1 << (power - 1)) == 0) {
      accent_bits |= 1; // turn on kick accent bit
    }  else  {
      accent_bits &= ~1; // turn off kick accent bit
    }
    state_ |= accent_bits << 3;
  }

  uint8_t swung_offset = swing_patterns_lookup[swing_pattern_index][pulse_index % 24];
  uint8_t pulse_with_offset = (pulse_index - (pulse_index % 24)) + swung_offset;

  if( pulse_with_offset % 3 != 0) { // our patterns are 32 per 96 
     return;
  }

  for (uint8_t i = 0; i < kNumParts; ++i) {
    bool isSnarePart = i == 1;
    bool isHihatPart = i == 2;

    uint8_t stepNumber = pulse_with_offset / 3;
    uint8_t level = swung_offset == 99 ? 0 : ReadDrumMap(stepNumber, i, x, y);

    uint8_t threshold = ~settings_.density[i];
    if (level > threshold) {
      if (level > 192) {
        if ((isHihatPart && y > 100)) {
          // not trying to use accent for anything but open hat and regular trigger now
          state_ |= instrument_mask << 3;
        }
      }
      state_ |= instrument_mask; // if we're above the threshold, hit that drum
    }
    instrument_mask <<= 1;
  }
  
  
}

/* static */
void PatternGenerator::EvaluateEuclidean() {
  // Refresh only on sixteenth notes.
  if (step_ & 1) {
    return;
  }

  // Euclidean pattern generation
  uint8_t instrument_mask = 1;
  uint8_t reset_bits = 0;
  for (uint8_t i = 0; i < kNumParts; ++i) {
    uint8_t length = (settings_.options.euclidean_length[i] >> 3) + 1;
    uint8_t density = settings_.density[i] >> 3;
    uint16_t address = U8U8Mul(length - 1, 32) + density;
    while (euclidean_step_[i] >= length) {
      euclidean_step_[i] -= length;
    }
    uint32_t step_mask = 1L << static_cast<uint32_t>(euclidean_step_[i]);
    uint32_t pattern_bits = pgm_read_dword(lut_res_euclidean + address);
    if (pattern_bits & step_mask) {
      state_ |= instrument_mask;
    }
    if (euclidean_step_[i] == 0) {
      reset_bits |= instrument_mask;
    }
    instrument_mask <<= 1;
  }

  if (output_clock()) {
    state_ |= reset_bits ? OUTPUT_BIT_COMMON : 0;
    state_ |= (reset_bits == 0x07) ? OUTPUT_BIT_RESET : 0;
  } else {
    state_ |= reset_bits << 3;
  }
}

/* static */
void PatternGenerator::LoadSettings() {
  options_.unpack(eeprom_read_byte(NULL));
  factory_testing_ = eeprom_read_byte((uint8_t*)(1)) + 1;
}

/* static */
void PatternGenerator::SaveSettings() {
  eeprom_write_byte(NULL, options_.pack());
  ++factory_testing_;
  if (factory_testing_ >= 5) {
    factory_testing_ = 5;
  }
  eeprom_write_byte((uint8_t*)(1), factory_testing_);
}

/* static */
void PatternGenerator::Evaluate() {
  state_ = 0;
  pulse_duration_counter_ = 0;

  Random::Update();
  // Highest bits: clock and random bit.
  state_ |= 0x40;
  state_ |= Random::state() & 0x80;

  if (output_clock()) {
    state_ |= OUTPUT_BIT_CLOCK;
  }

  EvaluateMFDrums(pulse_index_);

  // Refresh only at step changes.
  if (pulse_ != 0) {
    return;
  }

  // if (options_.output_mode == OUTPUT_MODE_EUCLIDEAN) {
  //   EvaluateEuclidean();
  // } else {
  //   EvaluateMFDrums();
  // }
}

/* static */
int8_t PatternGenerator::swing_amount() {
  if (options_.swing && output_mode() == OUTPUT_MODE_DRUMS) {
    int8_t value = U8U8MulShift8(settings_.options.drums.randomness, 42 + 1);
    return (!(step_ & 2)) ? value : -value;
  } else {
    return 0;
  }
}

}  // namespace grids
