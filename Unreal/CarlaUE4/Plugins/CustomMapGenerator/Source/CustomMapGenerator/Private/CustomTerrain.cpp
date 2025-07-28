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

UCustomTerrain::UCustomTerrain() {

}

UCustomTerrain::~UCustomTerrain()
{
}

void UCustomTerrain::Init(FString map_name_passed, UMapDataset* map_dataset_passed) {
  map_name = map_name_passed;
  map_dataset = map_dataset_passed;
  UE_LOG(LogCustomMapGenerator, Display, TEXT("UCustomTerrain::Init is called!!!"));
}

void UCustomTerrain::CreateTiles() {
  TArray<FString> keys = this->map_dataset->getTerrainTileKeys();
  for (FString& key : keys) {
    this->CreateTile(key);
  }
}

void UCustomTerrain::CreateTile(const FString tile_index) {
  UE_LOG(LogCustomMapGenerator, Display, TEXT("CreateTile tile_index %s"), *tile_index);
  //CreateTerrainMeshForTile(tile_index);
  //UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
  //UEditorLevelLibrary::SaveCurrentLevel();
}

/*
void UCustomTerrain::CreateTerrainMesh()
{
  UWorld* World = UEditorLevelLibrary::GetEditorWorld();
  // Creation of the procedural mesh
  AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>();
  UStaticMeshComponent* Mesh = MeshActor->GetStaticMeshComponent();

  UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
    Vertices,
    Triangles,
    UVs,
    Normals,
    Tangents
  );

  FProceduralCustomMesh MeshData;
  MeshData.Vertices = Vertices;
  MeshData.Triangles = Triangles;
  MeshData.Normals = Normals;
  MeshData.UV0 = UVs;
  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Creating Mesh"));
  UStaticMesh* MeshToSet = UMapGenFunctionLibrary::CreateMesh(MeshData, Tangents, DefaultMaterial, MapName, "Terrain"+TileIndex, FName(TEXT("SM_LandscapeMesh" + TileIndex + FString::FromInt(MeshIndex) )));
  Mesh->SetStaticMesh(MeshToSet);
  MeshActor->SetActorLabel("SM_LandscapeActor" + TileIndex + FString::FromInt(MeshIndex) );
  MeshActor->SetFolderPath(FName(*("Tile" + TileIndex + "/Terrain")));
  MeshActor->Tags.Add(FName("LandscapeToMove"));
  Mesh->CastShadow = false;
  MeshActor->SetActorLocation(FVector(tile_position_cm.X, tile_position_cm.Y, dem_height_beginning * DEMConversionFactorToUnreal));
  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Adding to Landscapes list"));
  
}*/