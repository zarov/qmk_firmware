/* Copyright 2016 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "process_midi.h"

bool midi_activated = false;
uint8_t midi_starting_note = 0x0C;
int midi_offset = 7;

bool process_midi(uint16_t keycode, keyrecord_t *record) {
    if (keycode == MI_ON && record->event.pressed) {
      midi_activated = true;
#ifdef AUDIO_ENABLE
      music_scale_user();
#endif
      return false;
    }

    if (keycode == MI_OFF && record->event.pressed) {
      midi_activated = false;
      midi_send_cc(&midi_device, 0, 0x7B, 0);
      return false;
    }

    if (midi_activated) {
      if (record->event.key.col == (MATRIX_COLS - 1) && record->event.key.row == (MATRIX_ROWS - 1)) {
          if (record->event.pressed) {
              midi_starting_note++; // Change key
              midi_send_cc(&midi_device, 0, 0x7B, 0);
          }
          return false;
      }
      if (record->event.key.col == (MATRIX_COLS - 2) && record->event.key.row == (MATRIX_ROWS - 1)) {
          if (record->event.pressed) {
              midi_starting_note--; // Change key
              midi_send_cc(&midi_device, 0, 0x7B, 0);
          }
          return false;
      }
      if (record->event.key.col == (MATRIX_COLS - 3) && record->event.key.row == (MATRIX_ROWS - 1) && record->event.pressed) {
          midi_offset++; // Change scale
          midi_send_cc(&midi_device, 0, 0x7B, 0);
          return false;
      }
      if (record->event.key.col == (MATRIX_COLS - 4) && record->event.key.row == (MATRIX_ROWS - 1) && record->event.pressed) {
          midi_offset--; // Change scale
          midi_send_cc(&midi_device, 0, 0x7B, 0);
          return false;
      }
      // basic
      // uint8_t note = (midi_starting_note + SCALE[record->event.key.col + midi_offset])+12*(MATRIX_ROWS - record->event.key.row);
      // advanced
      // uint8_t note = (midi_starting_note + record->event.key.col + midi_offset)+12*(MATRIX_ROWS - record->event.key.row);
      // guitar
      uint8_t note = (midi_starting_note + record->event.key.col + midi_offset)+5*(MATRIX_ROWS - record->event.key.row);
      // violin
      // uint8_t note = (midi_starting_note + record->event.key.col + midi_offset)+7*(MATRIX_ROWS - record->event.key.row);

      if (record->event.pressed) {
        // midi_send_noteon(&midi_device, record->event.key.row, midi_starting_note + SCALE[record->event.key.col], 127);
        midi_send_noteon(&midi_device, 0, note, 127);
      } else {
        // midi_send_noteoff(&midi_device, record->event.key.row, midi_starting_note + SCALE[record->event.key.col], 127);
        midi_send_noteoff(&midi_device, 0, note, 127);
      }

      if (keycode < 0xFF) // ignores all normal keycodes, but lets RAISE, LOWER, etc through
        return false;
    }
  return true;
}
