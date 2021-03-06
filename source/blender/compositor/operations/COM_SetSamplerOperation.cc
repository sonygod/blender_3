/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2011, Blender Foundation.
 */

#include "COM_SetSamplerOperation.h"

namespace blender::compositor {

SetSamplerOperation::SetSamplerOperation()
{
  this->add_input_socket(DataType::Color);
  this->add_output_socket(DataType::Color);
}

void SetSamplerOperation::init_execution()
{
  reader_ = this->get_input_socket_reader(0);
}
void SetSamplerOperation::deinit_execution()
{
  reader_ = nullptr;
}

void SetSamplerOperation::execute_pixel_sampled(float output[4],
                                                float x,
                                                float y,
                                                PixelSampler /*sampler*/)
{
  reader_->read_sampled(output, x, y, sampler_);
}

}  // namespace blender::compositor
