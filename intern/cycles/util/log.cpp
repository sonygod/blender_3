/*
 * Copyright 2011-2014 Blender Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "util/log.h"

#include "util/math.h"
#include "util/string.h"

#include <stdio.h>
#ifdef _MSC_VER
#  define snprintf _snprintf
#endif

CCL_NAMESPACE_BEGIN

#ifdef WITH_CYCLES_LOGGING
static bool is_verbosity_set()
{
  using CYCLES_GFLAGS_NAMESPACE::GetCommandLineOption;

  std::string verbosity;
  if (!GetCommandLineOption("v", &verbosity)) {
    return false;
  }
  return verbosity != "0";
}
#endif

void util_logging_init(const char *argv0)
{
#ifdef WITH_CYCLES_LOGGING
  using CYCLES_GFLAGS_NAMESPACE::SetCommandLineOption;

  google::InitGoogleLogging(argv0);
  SetCommandLineOption("logtostderr", "1");
  if (!is_verbosity_set()) {
    SetCommandLineOption("v", "0");
  }
  SetCommandLineOption("stderrthreshold", "0");
  SetCommandLineOption("minloglevel", "0");
#else
  (void)argv0;
#endif
}

void util_logging_start()
{
#ifdef WITH_CYCLES_LOGGING
  using CYCLES_GFLAGS_NAMESPACE::SetCommandLineOption;
  SetCommandLineOption("logtostderr", "1");
  if (!is_verbosity_set()) {
    SetCommandLineOption("v", "2");
  }
  SetCommandLineOption("stderrthreshold", "0");
  SetCommandLineOption("minloglevel", "0");
#endif
}

void util_logging_verbosity_set(int verbosity)
{
#ifdef WITH_CYCLES_LOGGING
  using CYCLES_GFLAGS_NAMESPACE::SetCommandLineOption;
  char val[10];
  snprintf(val, sizeof(val), "%d", verbosity);
  SetCommandLineOption("v", val);
#else
  (void)verbosity;
#endif
}

std::ostream &operator<<(std::ostream &os, const int2 &value)
{
  os << "(" << value.x << ", " << value.y << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const float3 &value)
{
  os << "(" << value.x << ", " << value.y << ", " << value.z << ")";
  return os;
}

CCL_NAMESPACE_END
