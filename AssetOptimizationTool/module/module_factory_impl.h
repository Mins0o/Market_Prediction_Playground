#pragma once

#include "_interfaces/module_factory_interface.h"
#include "asset_optimization_tool_impl.h"
#include "configuration/configuration.h"
#include "data/data.h"

namespace asset_optimization_tool {
class ProductionModuleFactory : public IModuleFactory {
 public:
  modules::IData* CreateDataInterface() override { return new modules::Data(); }
  modules::IConfiguration* CreateConfigurationInterface() override {
    return new modules::Configuration();
  }
};

AssetOptimizationTool* AssetOptimizationTool::Create() {
  ProductionModuleFactory factory = ProductionModuleFactory();
  modules::IData* concrete_data_interface = factory.CreateDataInterface();
  modules::IConfiguration* concrete_config_interface =
      factory.CreateConfigurationInterface();
  AssetOptimizationToolImpl* tool_p = new AssetOptimizationToolImpl(
      concrete_data_interface, concrete_config_interface);
  return tool_p;
}

}  // namespace asset_optimization_tool