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

#include "Helpers.hpp"

UCustomTerrain::UCustomTerrain() {
  MapName = FString("");
  DefaultMaterial = NULL;
  Origin = FVector(0.0, 0.0, 0.0);
  TileHeight = 60960.0;
  TileWidth = 91440.0;
  //GridSectionSize = 30.48;
  GridSectionSize = 60.96;
  //GridSectionSize = 15.24;
  SubTilesInHeight = 20;
  SubTilesInWidth = 30;
  SubTilesInHeight = 40;
  SubTilesInWidth = 60;
  DEMCellSize = 2.0;
  DEMConversionFactorToUnreal = 30.48;
  TerrainMetadata = NULL;
}

UCustomTerrain::~UCustomTerrain()
{
}

void UCustomTerrain::Init(FString MapNamePassed, UMaterialInstance* DefaultLandscapeMaterial, FVector const& origin, FString json_path) {
  MapName = MapNamePassed;
  DefaultMaterial = DefaultLandscapeMaterial;
  Origin = FVector(origin.X, origin.Y, origin.Z);
  TerrainMetadata = UCustomMapGeneratorHelpers::LoadJSONMetadata(json_path);
  UE_LOG(LogCustomMapGenerator, Display, TEXT("UCustomTerrain::Init is called!!!"));
}

void UCustomTerrain::CreateTile(const FString TileIndex, const FVector Offset) {
  UE_LOG(LogCustomMapGenerator, Display, TEXT("CreateTile TileIndex %s"), *TileIndex);
  DEM<double> dem = UCustomMapGeneratorHelpers::ReadDEMFromTile(this->TerrainMetadata, TileIndex);
  const nlohmann::json& metadata = UCustomMapGeneratorHelpers::GetDEMMeta(this->TerrainMetadata, TileIndex);
  CreateTerrainMeshForTile(TileIndex, dem, metadata);
}

void UCustomTerrain::CreateTerrainMeshForTile(const FString TileIndex, DEM<double> dem, const nlohmann::json & metadata) {
  float x_start = (static_cast<float>(metadata["y"]["min"]) - (DEMCellSize / 2.0));
  float y_start = (static_cast<float>(metadata["x"]["min"]) - (DEMCellSize / 2.0));
  float x_height = (static_cast<float>(metadata["y"]["max"]) - static_cast<float>(metadata["y"]["min"])) + DEMCellSize;
  float y_height = (static_cast<float>(metadata["x"]["max"]) - static_cast<float>(metadata["x"]["min"])) + DEMCellSize;
  float x_delta = (x_height / static_cast<float>(SubTilesInHeight));
  float y_delta = (y_height / static_cast<float>(SubTilesInWidth));
  for (int i = 0; i < SubTilesInHeight; i++) {
    for (int j = 0; j < SubTilesInWidth; j++) {
      float x_subtile_start = x_start + (x_delta * i);
      float x_subtile_end = x_start + (x_delta * (i + 1));
      float y_subtile_start = y_start + (y_delta * j);
      float y_subtile_end = y_start + (y_delta * (j + 1));
      FVector2D tile_position_cm = FVector2D(x_delta * i * DEMConversionFactorToUnreal, y_delta * j * DEMConversionFactorToUnreal);
      FVector2D tile_start = FVector2D(x_subtile_start, y_subtile_start);
      FVector2D tile_end = FVector2D(x_subtile_end, y_subtile_end);
      UE_LOG(LogCustomMapGenerator, Display, TEXT("CreateTerrainMesh TileIndex %s, MeshIndex %d, Position: (%f, %f), Start: (%f, %f), End: (%f, %f)"), *TileIndex, (i * SubTilesInWidth) + j, tile_position_cm.X, tile_position_cm.Y, tile_start.X, tile_start.Y, tile_end.X, tile_end.Y);
      CreateTerrainMesh(dem, TileIndex, (i * SubTilesInWidth) + j, tile_position_cm, tile_start, tile_end);
    }
  }
  UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
  UEditorLevelLibrary::SaveCurrentLevel();
}

void UCustomTerrain::CreateTerrainMesh(DEM<double>& dem, const FString TileIndex, const int MeshIndex, const FVector2D tile_position_cm, const FVector2D tile_start, const FVector2D tile_end)
{
  UWorld* World = UEditorLevelLibrary::GetEditorWorld();
  // Creation of the procedural mesh
  AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>();
  UStaticMeshComponent* Mesh = MeshActor->GetStaticMeshComponent();

  TArray<FVector> Vertices;
  TArray<int32> Triangles;

  TArray<FVector> Normals;
  TArray<FLinearColor> Colors;
  TArray<FProcMeshTangent> Tangents;
  TArray<FVector2D> UVs;


  int VerticesInX = (((tile_end.X - tile_start.X) * DEMConversionFactorToUnreal) / GridSectionSize) + 1.0f;
  int VerticesInY = (((tile_end.Y - tile_start.Y) * DEMConversionFactorToUnreal) / GridSectionSize) + 1.0f;
  double dem_X_beginning = static_cast<double>(tile_start.X);
  double dem_Y_beginning = static_cast<double>(tile_start.Y);
  double dem_height_beginning = dem.interpolated_altitude(dem_Y_beginning, dem_X_beginning);
  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Vertices Loop"));
  for (int i = 0; i < VerticesInX; i++) {
    float X = (i * GridSectionSize); 
    double dem_X = static_cast<double>(tile_start.X + (X / DEMConversionFactorToUnreal));
    for (int j = 0; j < VerticesInY; j++) {
      float Y = (j * GridSectionSize);
      double dem_Y = static_cast<double>(tile_start.Y + (Y / DEMConversionFactorToUnreal));
      float HeightValue = (static_cast<float>(dem.interpolated_altitude(dem_Y, dem_X) - dem_height_beginning)) * DEMConversionFactorToUnreal;
      Vertices.Add(FVector(X, Y, HeightValue));
      UVs.Add(FVector2D(i, j));
    }
  }

  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Triangles Loop"));
  //// Triangles formation. 2 triangles per section.
  for(int i = 0; i < VerticesInX - 1; i++)
  {
    for(int j = 0; j < VerticesInY - 1; j++)
    {
      Triangles.Add(j + (i * VerticesInY));
      Triangles.Add((j + 1) + (i * VerticesInY));
      Triangles.Add(j + ((i + 1) * VerticesInY));

      Triangles.Add((j + 1) + (i * VerticesInY));
      Triangles.Add((j + 1) + ((i + 1) * VerticesInY));
      Triangles.Add(j + ((i + 1) * VerticesInY));
    }
  }

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
}