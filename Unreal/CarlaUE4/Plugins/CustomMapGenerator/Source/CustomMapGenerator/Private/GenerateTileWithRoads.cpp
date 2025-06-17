// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#include "GenerateTileWithRoads.h"
#include "CustomMapGenerator.h"

#include <string>
#include <iostream>
#include "Helpers.hpp"
#include <float.h>
#include "Carla/MapGen/LargeMapManager.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "EditorLevelLibrary.h"
#include "Math/Vector.h"
#include "Math/Vector2D.h"
#include "UnrealString.h"

#if WITH_EDITOR
#include "FileHelpers.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"

FVector _GetOriginSurveyFeet(nlohmann::json tile) {
  return FVector(tile["y"]["min"], tile["x"]["min"], tile["z"]["min"]);
}

FVector2D _GetOriginLatLong(nlohmann::json tile) {
  FVector2D result = FVector2D(FLT_MAX, FLT_MAX);
  // Coordinate pair
  if (tile.size() == 2) {
    result.X = static_cast<float>(tile[0]);
    result.Y = static_cast<float>(tile[1]);
  }
  else {
    for (auto& item : tile.items()) {
      if (item.value().type() == nlohmann::json::value_t::array) {
        FVector2D current_result = _GetOriginLatLong(item.value());
        if (current_result.X < result.X) {
          result.X = current_result.X;
        }
        if (current_result.Y < result.Y) {
          result.Y = current_result.Y;
        }
      }
    }
  }
  return result;
}

UGenerateTileWithRoads::UGenerateTileWithRoads()
{
    OriginTile = FString("148110");
    JsonPath = "/home/richarwa/CarlaIngestion/metadata.json";
    OpenDrivePath = "/home/richarwa/CarlaIngestion/148110_full.xodr";
    UE_LOG(LogCustomMapGenerator, Display, TEXT("UGenerateTileWithRoads is called - new3!!!"));
    LoadJSONMetadata();
    UE_LOG(LogCustomMapGenerator, Display, TEXT("UGenerateTileWithRoads loaded JSON metadata!"));
}

UGenerateTileWithRoads::~UGenerateTileWithRoads()
{
    UE_LOG(LogCustomMapGenerator, Display, TEXT("~UGenerateTileWithRoads is called!!!"));
}

void UGenerateTileWithRoads::CreateMap()
{
    UE_LOG(LogCustomMapGenerator, Display, TEXT("UGenerateTileWithRoads::CreateMap is called, map name is %s!!!"), *(MapName));
    this->TerrainFactory = NewObject<UCustomTerrain>();
    this->TerrainFactory->Init(MapName, DefaultLandscapeMaterial, OriginSurveyFeet, JsonPath);
    this->RoadFactory = NewObject<UCustomRoads>();
    this->RoadFactory->Init(MapName, DefaultLandscapeMaterial, DefaultRoadMaterial, DefaultLaneMarksWhiteMaterial, DefaultLaneMarksYellowMaterial, DefaultSidewalksMaterial, FVector(OriginLatLong.X, OriginLatLong.Y, 0.0), OpenDrivePath);
    AActor* QueryActor = UGameplayStatics::GetActorOfClass(
        UEditorLevelLibrary::GetEditorWorld(),
        ALargeMapManager::StaticClass() );

    if( QueryActor != nullptr )
    {
        ALargeMapManager* LargeMapManager = Cast<ALargeMapManager>(QueryActor);
        FIntVector NumTilesInXY  = LargeMapManager->GetNumTilesInXY();
        float TileSize = LargeMapManager->GetTileSize();
        FVector Tile0Offset = LargeMapManager->GetTile0Offset();
        FIntVector CurrentTilesInXY = FIntVector(0,0,0);
        ULevel* PersistantLevel = UEditorLevelLibrary::GetEditorWorld()->PersistentLevel;
        BaseLevelName = LargeMapManager->LargeMapTilePath + "/" + LargeMapManager->LargeMapName;
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("Current Tile is %s"), *( CurrentTilesInXY.ToString() ) );
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("NumTilesInXY is %s"), *( NumTilesInXY.ToString() ) );
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("TileSize is %f"), ( TileSize ) );
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("Tile0Offset is %s"), *( Tile0Offset.ToString() ) );
        TerrainFactory->CreateTile(this->OriginTile, FVector(0.0, 0.0, 0.0));
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("Mesh terrain created!"));
        RoadFactory->CreateTile(this->OriginTile, FVector(0.0, 0.0, 0.0));
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("Roads created!"));
    }
    else {
        UE_LOG(LogCustomMapGenerator, Error, TEXT("Getting large map manager handle failed!"));
    }
    
}

void UGenerateTileWithRoads::LoadJSONMetadata() {
  this->JsonData = UCustomMapGeneratorHelpers::LoadJSONMetadata(this->JsonPath);
  std::string origin_tile_name = TCHAR_TO_UTF8(*this->OriginTile);
  if (this->JsonData.find(origin_tile_name) != this->JsonData.end()) {
    nlohmann::json origin_tile = this->JsonData[origin_tile_name];
    // X and Y are flipped from DEM to Unreal
    this->OriginSurveyFeet = _GetOriginSurveyFeet(origin_tile["DEM"]);
    this->OriginLatLong = _GetOriginLatLong(origin_tile["GEOJSON"]["coordinates"]);
    UE_LOG(LogCustomMapGenerator, Display, TEXT("Origin tile found, Origin X: %f, Origin Y: %f, Origin Z: %f"), this->OriginSurveyFeet.X, this->OriginSurveyFeet.Y, this->OriginSurveyFeet.Z);
  }
  else {
    this->OriginSurveyFeet = FVector(631381.0, 1764506.0, 479.64);
    UE_LOG(LogCustomMapGenerator, Warning, TEXT("Origin tile not found!"));
  }
}


#endif