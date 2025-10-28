// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FCustomMapGeneratorModule"
DECLARE_LOG_CATEGORY_EXTERN(LogCustomMapGenerator, Log, All);

class FCustomMapGeneratorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
