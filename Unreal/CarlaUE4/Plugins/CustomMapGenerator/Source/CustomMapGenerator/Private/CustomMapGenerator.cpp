// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University

#include "CustomMapGenerator.h"

DEFINE_LOG_CATEGORY(LogCustomMapGenerator);

void FCustomMapGeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FCustomMapGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomMapGeneratorModule, CustomMapGenerator)

namespace carla {

  [[ noreturn ]] void throw_exception(const std::exception &e) {
    UE_LOG(LogCustomMapGenerator, Fatal, TEXT("Exception thrown: %s"), UTF8_TO_TCHAR(e.what()));
    // It should never reach this part.
    std::terminate();
  }

}