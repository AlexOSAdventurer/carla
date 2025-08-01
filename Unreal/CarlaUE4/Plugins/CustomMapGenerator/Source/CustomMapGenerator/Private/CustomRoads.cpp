// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#include "CustomRoads.h"
#include "CustomMapGenerator.h"
#include "Misc/FileHelper.h"

#include "ContentBrowserModule.h"
#include "GameFramework/Actor.h"

#include <string>
#include <iostream>
#include "Carla/BlueprintLibary/MapGenFunctionLibrary.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "StaticMeshAttributes.h"
#include "Engine/AssetManager.h"
#include "AssetRegistryModule.h"
#include "MeshDescription.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "EditorLevelLibrary.h"
#include "FileHelpers.h"
#include "Misc/Paths.h"

const FString UCustomRoads::asset_type = "roads";

UCustomRoads::UCustomRoads() {

}

UCustomRoads::~UCustomRoads()
{
}

void UCustomRoads::Init(FString map_name_passed, UMapDataset* map_dataset_passed) {
  this->map_name = map_name_passed;
  this->map_dataset = map_dataset_passed;
  UE_LOG(LogCustomMapGenerator, Display, TEXT("UCustomRoads::Init is called!!!"));
}

void UCustomRoads::CreateRoads() {
  TMap<FString, FString> road_to_asset = this->map_dataset->importAssetMeshes(this->map_name, UCustomRoads::asset_type);
  TArray<FString> keys = this->map_dataset->getAssetKeys(UCustomRoads::asset_type);
  for (FString& key : keys) {
    FCustomMapAssetData road_data = this->map_dataset->getAssetData(key, UCustomRoads::asset_type);
    this->CreateRoad(road_data, road_to_asset[road_data.name]);
  }
  UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
  UEditorLevelLibrary::SaveCurrentLevel();
}

void UCustomRoads::CreateRoad(const FCustomMapAssetData road_data, const FString road_path) {
  UE_LOG(LogCustomMapGenerator, Display, TEXT("CreateRoad tile_index %s"), *(road_data.name));
  UStaticMesh* static_mesh = LoadObject<UStaticMesh>(nullptr, *road_path);

  if (!static_mesh)
  {
    UE_LOG(LogCustomMapGenerator, Error, TEXT("Failed to load mesh from %s"), *road_path);
    return;
  }
  FVector location(road_data.min_x, road_data.min_y, road_data.min_z);  // World location
  FRotator rotation(0, 0, 0); // Yaw/Pitch/Roll
  FActorSpawnParameters spawn_params;

  UWorld* world = UEditorLevelLibrary::GetEditorWorld();
  AStaticMeshActor* mesh_actor = world->SpawnActor<AStaticMeshActor>(
    AStaticMeshActor::StaticClass(), 
    FTransform(rotation, location),
    spawn_params
  );
  if (mesh_actor && static_mesh)
  {
    UStaticMeshComponent* mesh_component = mesh_actor->GetStaticMeshComponent();
    mesh_component->SetStaticMesh(static_mesh);
    mesh_component->SetMobility(EComponentMobility::Static);
    mesh_component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    mesh_actor->SetActorLabel(road_data.name);
    mesh_actor->SetFolderPath(FName(UCustomRoads::asset_type));

    // Make sure it shows up
    mesh_component->RegisterComponent();
  }
  else {
    UE_LOG(LogCustomMapGenerator, Error, TEXT("Failed to load mesh actor for %s"), *road_path);
  }
}

