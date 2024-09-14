#pragma once

#include "_interfaces/data_interface.h"

namespace asset_optimization_tool {
class IModuleFactory {
 public:
  virtual modules::IData* CreateDataInterface() = 0;
  virtual ~IModuleFactory() = default;
};
}  // namespace asset_optimization_tool