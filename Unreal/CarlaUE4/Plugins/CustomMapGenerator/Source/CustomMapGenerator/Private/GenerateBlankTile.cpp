// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#include "GenerateBlankTile.h"
#include "CustomMapGenerator.h"

#include <string>
#include <iostream>
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "EditorLevelLibrary.h"
#include "Math/Vector.h"
#include "UnrealString.h"

#if WITH_EDITOR
#include "FileHelpers.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"

UGenerateBlankTile::UGenerateBlankTile()
{
  dataset_path = "/home/richarwa/CarlaIngestion/CarlaCooked";
}

UGenerateBlankTile::~UGenerateBlankTile()
{
    UE_LOG(LogCustomMapGenerator, Display, TEXT("~UGenerateBlankTile is called!!!"));
}

void UGenerateBlankTile::CreateMap()
{
  UE_LOG(LogCustomMapGenerator, Display, TEXT("CreateMap is called, map name is %s!!!"), *(map_name));
  this->map_dataset = NewObject<UMapDataset>();
  this->map_dataset->load();
  this->map_dataset->copyXODR(this->map_name);
  this->terrain_factory = NewObject<UCustomTerrain>();
  this->terrain_factory->Init(this->map_name, this->map_dataset);
  this->road_factory = NewObject<UCustomRoads>();
  this->road_factory->Init(this->map_name, this->map_dataset);

  ULevel* PersistantLevel = UEditorLevelLibrary::GetEditorWorld()->PersistentLevel;
  road_factory->CreateRoads();
  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Roads created!"));
  terrain_factory->CreateTiles();
  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Mesh terrain created!"));
}

#endif