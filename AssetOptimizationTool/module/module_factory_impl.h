#pragma once

#include "_interfaces/module_factory_interface.h"
#include "data/data.h"

namespace asset_optimization_tool {
class ProductionModuleFactory : public IModuleFactory {
  modules::IData* CreateDataInterface() override { return new modules::Data(); }
};

std::unique_ptr<AssetOptimizationTool> AssetOptimizationTool::Create() {
  std::unique_ptr<IModuleFactory> factory =
      std::make_unique<ProductionModuleFactory>();
  return std::make_unique<AssetOptimizationToolImpl>(std::move(factory));
}

}  // namespace asset_optimization_tool