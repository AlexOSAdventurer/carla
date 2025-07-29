// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#include "CustomTerrain.h"
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


UCustomTerrain::UCustomTerrain() {

}

UCustomTerrain::~UCustomTerrain()
{
}

void UCustomTerrain::Init(FString map_name_passed, UMapDataset* map_dataset_passed) {
  this->map_name = map_name_passed;
  this->map_dataset = map_dataset_passed;
  UE_LOG(LogCustomMapGenerator, Display, TEXT("UCustomTerrain::Init is called!!!"));
}

void UCustomTerrain::CreateTiles() {
  TMap<FString, FString> tile_to_asset = this->map_dataset->importTerrainMeshes(this->map_name);
  TArray<FString> keys = this->map_dataset->getTerrainTileKeys();
  TArray<float> origin = this->map_dataset->getOrigin();
  for (FString& key : keys) {
    FCustomMapTileData tile_data = this->map_dataset->getTerrainData(key);
    this->CreateTile(tile_data, tile_to_asset[tile_data.name], origin);
  }
  UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
  UEditorLevelLibrary::SaveCurrentLevel();
}

void UCustomTerrain::CreateTile(const FCustomMapTileData tile_data, const FString tile_path, const TArray<float> origin) {
  UE_LOG(LogCustomMapGenerator, Display, TEXT("CreateTile tile_index %s"), *(tile_data.name));
  UStaticMesh* static_mesh = LoadObject<UStaticMesh>(nullptr, *tile_path);

  if (!static_mesh)
  {
    UE_LOG(LogCustomMapGenerator, Error, TEXT("Failed to load mesh from %s"), *tile_path);
    return;
  }
  FVector location(tile_data.min_x - origin[0], tile_data.min_y - origin[1], tile_data.min_z - origin[2]);  // World location
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
    mesh_actor->SetActorLabel(tile_data.name);
    mesh_actor->SetFolderPath(FName("Terrain"));

    // Make sure it shows up
    mesh_component->RegisterComponent();
  }
  else {
    UE_LOG(LogCustomMapGenerator, Error, TEXT("Failed to load mesh actor for %s"), *tile_path);
  }
}

