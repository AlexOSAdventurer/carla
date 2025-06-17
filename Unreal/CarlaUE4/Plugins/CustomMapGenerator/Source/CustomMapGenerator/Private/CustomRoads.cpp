// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
// Copyright (c) 2023 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#include "CustomRoads.h"
#include "CustomMapGenerator.h"
#include "Misc/FileHelper.h"

#include "ContentBrowserModule.h"
#include "GameFramework/Actor.h"

#include <string>
#include <iostream>
#include "Helpers.hpp"
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

UCustomRoads::UCustomRoads() {
  MapName = FString("");
  Origin = FVector(0.0, 0.0, 0.0);
  TileHeight = 60960.0;
  TileWidth = 91440.0;
}

UCustomRoads::~UCustomRoads()
{
}

void UCustomRoads::Init(FString MapNamePassed, UMaterialInstance* LandscapePassed, UMaterialInstance* RoadPassed, UMaterialInstance* LaneMarksWhitePassed, UMaterialInstance* LaneMarksYellowPassed, UMaterialInstance* SidewalksPassed, FVector const& origin, FString OpenDrivePath) {
  MapName = MapNamePassed;
  DefaultLandscapeMaterial = LandscapePassed;
  DefaultRoadMaterial = RoadPassed;
  DefaultLaneMarksWhiteMaterial = LaneMarksWhitePassed;
  DefaultLaneMarksYellowMaterial = LaneMarksYellowPassed;
  DefaultSidewalksMaterial = SidewalksPassed;
  Origin = FVector(origin.X, origin.Y, origin.Z);
  UE_LOG(LogCustomMapGenerator, Display, TEXT("UCustomRoads::Init is called!!!"));
  FString file_content;
  UE_LOG(LogCustomMapGenerator, Warning, TEXT("UCustomRoads::GenerateTile(): File to load %s"), *OpenDrivePath );
  FFileHelper::LoadFileToString(file_content, *OpenDrivePath);
  std::string opendrive_xml = carla::rpc::FromLongFString(file_content);
  OpenDriveMap = carla::opendrive::OpenDriveParser::Load(opendrive_xml);
}

void UCustomRoads::CreateTile(const FString TileIndex, const FVector Offset) {
  if (OpenDriveMap.has_value()) {
    UE_LOG(LogCustomMapGenerator, Display, TEXT("UCustomRoads::CreateTile TileIndex %s"), *TileIndex);
    FVector MinLocation(0.0, 0.0, 0.0);
    FVector MaxLocation(TileWidth, -TileHeight, Offset.Z);
    //MinPosition = FVector(0.0, -Tile, 0.0f);
    //  MaxPosition = FVector(TileHeight, (CurrentTilesInXY.Y + 1.0f) * -TileSize, 10000.0f);
    /*carla::road::MapData& _data = OpenDriveMap->GetMap();
    std::vector<carla::road::RoadId> ToReturn;
    std::cout << "Reading from " + std::to_string(_data.GetRoads().size() ) + " roads " << std::endl;
    float min_x, min_y = FLT_MAX;
    float max_x, max_y = FLT_MIN;
    for( auto& road : _data.GetRoads() ){
      auto &&lane_section = (*road.second.GetLaneSections().begin());
      const carla::road::Lane* lane = lane_section.GetLane(-1);
      if( lane ) {
        const double s_check = lane_section.GetDistance() + lane_section.GetLength() * 0.5;
        carla::geom::Location roadLocation = lane->ComputeTransform(s_check).location;
        std::cout << "X: " << roadLocation.x << " Y: " << roadLocation.y << " Z: " << roadLocation.z << std::endl;
        if (roadLocation.x < min_x) {
          min_x = roadLocation.x;
        }
        else if (roadLocation.x > max_x) {
          max_x = roadLocation.x;
        }
        if (roadLocation.y < min_y) {
          min_y = roadLocation.y;
        }
        else if (roadLocation.y > max_y) {
          max_y = roadLocation.y;
        }
      }
    }
    std::cout << "Min x: " << min_x << ", Max x:" << max_x << std::endl;
    std::cout << "Min y: " << min_y << ", Max y:" << max_y << std::endl;*/
    this->GenerateRoadMesh(TileIndex, MinLocation, MaxLocation);
    this->GenerateLaneMarks(TileIndex, MinLocation, MaxLocation);
    this->GenerateSpawnPoints(TileIndex, MinLocation, MaxLocation);
  }
  else {
    UE_LOG(LogCustomMapGenerator, Display, TEXT("UCustomRoads::CreateTile No Map!"));
  }
}

void UCustomRoads::GenerateRoadMesh(const FString TileIndex, FVector MinLocation, FVector MaxLocation)
{
  opg_parameters.vertex_distance = 0.5f;
  opg_parameters.vertex_width_resolution = 8.0f;
  opg_parameters.simplification_percentage = 50.0f;
  double start = FPlatformTime::Seconds();

  carla::geom::Vector3D CarlaMinLocation(MinLocation.X / 100, MinLocation.Y / 100, MinLocation.Z /100);
  carla::geom::Vector3D CarlaMaxLocation(MaxLocation.X / 100, MaxLocation.Y / 100, MaxLocation.Z /100);
  const auto Meshes = OpenDriveMap->GenerateOrderedChunkedMeshInLocations(opg_parameters, CarlaMinLocation, CarlaMaxLocation);
  double end = FPlatformTime::Seconds();
  UE_LOG(LogCustomMapGenerator, Log, TEXT(" GenerateOrderedChunkedMesh code executed in %f seconds. Simplification percentage is %f"), end - start, opg_parameters.simplification_percentage);

  start = FPlatformTime::Seconds();
  static int index = 0;
  for (const auto &PairMap : Meshes)
  {
    for( auto& Mesh : PairMap.second )
    {
      if (!Mesh->GetVertices().size())
      {
        continue;
      }
      if (!Mesh->IsValid()) {
        continue;
      }

      if(PairMap.first == carla::road::Lane::LaneType::Driving)
      {
        for( auto& Vertex : Mesh->GetVertices() )
        {
          FVector VertexFVector = Vertex.ToFVector();
          float x = -Vertex.y;
          float y = Vertex.x;
          Vertex.x = x;
          Vertex.y = y;
          Vertex.z += GetHeight(Vertex.x, Vertex.y, this->DistanceToLaneBorder(VertexFVector) > 65.0f );
        }
        //carla::geom::Simplification Simplify(0.15);
        //Simplify.Simplificate(Mesh);
      }else{
        for( auto& Vertex : Mesh->GetVertices() )
        {
          float x = -Vertex.y;
          float y = Vertex.x;
          Vertex.x = x;
          Vertex.y = y;
          Vertex.z += GetHeight(Vertex.x, Vertex.y, false) + 0.15f;
          //Vertex.x *= -1.0;
          //Vertex.z *= 0.1;
        }
      }

      AStaticMeshActor* TempActor = UEditorLevelLibrary::GetEditorWorld()->SpawnActor<AStaticMeshActor>();
      UStaticMeshComponent* StaticMeshComponent = TempActor->GetStaticMeshComponent();
      TempActor->SetActorLabel(FString("SM_Lane_") + TileIndex + FString::FromInt(index));

      StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

      if(DefaultRoadMaterial && PairMap.first == carla::road::Lane::LaneType::Driving)
      {
        StaticMeshComponent->SetMaterial(0, DefaultRoadMaterial);
        StaticMeshComponent->CastShadow = false;
        TempActor->SetActorLabel(FString("SM_DrivingLane_") + TileIndex + FString::FromInt(index));
      }
      if(DefaultSidewalksMaterial && PairMap.first == carla::road::Lane::LaneType::Sidewalk)
      {
        StaticMeshComponent->SetMaterial(0, DefaultSidewalksMaterial);
        TempActor->SetActorLabel(FString("SM_Sidewalk_") + TileIndex + FString::FromInt(index));
      }
      FVector MeshCentroid = FVector(0,0,0);
      for( auto Vertex : Mesh->GetVertices() )
      {
        MeshCentroid += Vertex.ToFVector();
      }

      MeshCentroid /= Mesh->GetVertices().size();

      for( auto& Vertex : Mesh->GetVertices() )
      {
       Vertex.x -= MeshCentroid.X;
       Vertex.y -= MeshCentroid.Y;
       Vertex.z -= MeshCentroid.Z;
      }

      const FProceduralCustomMesh MeshData = *Mesh;
      TArray<FVector> Normals;
      TArray<FProcMeshTangent> Tangents;

      UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
        MeshData.Vertices,
        MeshData.Triangles,
        MeshData.UV0,
        Normals,
        Tangents
      );

      if(PairMap.first == carla::road::Lane::LaneType::Sidewalk)
      {
        UStaticMesh* MeshToSet = UMapGenFunctionLibrary::CreateMesh(MeshData,  Tangents, DefaultSidewalksMaterial, MapName, "DrivingLane", FName(TEXT("SM_SidewalkMesh" + TileIndex + FString::FromInt(index) )));
        StaticMeshComponent->SetStaticMesh(MeshToSet);
      }

      if(PairMap.first == carla::road::Lane::LaneType::Driving)
      {
        UStaticMesh* MeshToSet = UMapGenFunctionLibrary::CreateMesh(MeshData,  Tangents, DefaultRoadMaterial, MapName, "DrivingLane", FName(TEXT("SM_DrivingLaneMesh" + TileIndex + FString::FromInt(index))));
        StaticMeshComponent->SetStaticMesh(MeshToSet);
      }
      TempActor->SetActorLocation(MeshCentroid * 100);
      TempActor->Tags.Add(FName("RoadLane"));
      // ActorMeshList.Add(TempActor);
      StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
      TempActor->SetActorEnableCollision(true);
      index++;
    }
  }

  end = FPlatformTime::Seconds();
  UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
  UEditorLevelLibrary::SaveCurrentLevel();
  UE_LOG(LogCustomMapGenerator, Log, TEXT("Mesh spawnning and translation code executed in %f seconds."), end - start);
}

void UCustomRoads::GenerateLaneMarks(const FString TileIndex, FVector MinLocation, FVector MaxLocation)
{
  opg_parameters.vertex_distance = 0.5f;
  opg_parameters.vertex_width_resolution = 8.0f;
  opg_parameters.simplification_percentage = 15.0f;
  std::vector<std::string> lanemarkinfo;
  carla::geom::Vector3D CarlaMinLocation(MinLocation.X / 100, MinLocation.Y / 100, MinLocation.Z /100);
  carla::geom::Vector3D CarlaMaxLocation(MaxLocation.X / 100, MaxLocation.Y / 100, MaxLocation.Z /100);
  auto MarkingMeshes = OpenDriveMap->GenerateLineMarkings(opg_parameters, CarlaMinLocation, CarlaMaxLocation, lanemarkinfo);
  TArray<AActor*> LaneMarkerActorList;
  static int meshindex = 0;
  int index = 0;
  for (const auto& Mesh : MarkingMeshes)
  {

    if ( !Mesh->GetVertices().size() )
    {
      index++;
      continue;
    }
    if ( !Mesh->IsValid() ) {
      index++;
      continue;
    }

    FVector MeshCentroid = FVector(0, 0, 0);
    for (auto& Vertex : Mesh->GetVertices())
    {
      FVector VertexFVector = Vertex.ToFVector();
      Vertex.z += GetHeight(Vertex.x, Vertex.y, DistanceToLaneBorder(VertexFVector) > 65.0f ) + 0.0001f;
      float x = -Vertex.y;
      float y = Vertex.x;
      Vertex.x = x;
      Vertex.y = y;
      MeshCentroid += Vertex.ToFVector();
     
    }

    MeshCentroid /= Mesh->GetVertices().size();

    for (auto& Vertex : Mesh->GetVertices())
    {
      Vertex.x -= MeshCentroid.X;
      Vertex.y -= MeshCentroid.Y;
      Vertex.z -= MeshCentroid.Z;
    }

    // TODO: Improve this code
    float MinDistance = FLT_MAX;
    for(auto SpawnedActor : LaneMarkerActorList)
    {
      float VectorDistance = FVector::Distance(MeshCentroid*100, SpawnedActor->GetActorLocation());
      if(VectorDistance < MinDistance)
      {
        MinDistance = VectorDistance;
      }
    }

    if(MinDistance < 250)
    {
      UE_LOG(LogCustomMapGenerator, VeryVerbose, TEXT("Skkipped is %f."), MinDistance);
      index++;
      continue;
    }

    AStaticMeshActor* TempActor = UEditorLevelLibrary::GetEditorWorld()->SpawnActor<AStaticMeshActor>();
    UStaticMeshComponent* StaticMeshComponent = TempActor->GetStaticMeshComponent();
    TempActor->SetActorLabel(FString("SM_LaneMark_") + TileIndex + FString::FromInt(meshindex));
    StaticMeshComponent->CastShadow = false;
    if (lanemarkinfo[index].find("yellow") != std::string::npos) {
      if(DefaultLaneMarksYellowMaterial)
        StaticMeshComponent->SetMaterial(0, DefaultLaneMarksYellowMaterial);
    }else{
      if(DefaultLaneMarksWhiteMaterial)
        StaticMeshComponent->SetMaterial(0, DefaultLaneMarksWhiteMaterial);

    }

    const FProceduralCustomMesh MeshData = *Mesh;
    TArray<FVector> Normals;
    TArray<FProcMeshTangent> Tangents;
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
      MeshData.Vertices,
      MeshData.Triangles,
      MeshData.UV0,
      Normals,
      Tangents
    );

    UStaticMesh* MeshToSet = UMapGenFunctionLibrary::CreateMesh(MeshData,  Tangents, DefaultLandscapeMaterial, MapName, "LaneMark", FName(TEXT("SM_LaneMarkMesh" + TileIndex + FString::FromInt(meshindex) )));
    StaticMeshComponent->SetStaticMesh(MeshToSet);
    TempActor->SetActorLocation(MeshCentroid * 100);
    TempActor->Tags.Add(*FString(lanemarkinfo[index].c_str()));
    TempActor->Tags.Add(FName("RoadLane"));
    LaneMarkerActorList.Add(TempActor);
    index++;
    meshindex++;
    TempActor->SetActorEnableCollision(false);
    StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  }
  UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
  UEditorLevelLibrary::SaveCurrentLevel();
}

void UCustomRoads::GenerateSpawnPoints(const FString TileIndex, FVector MinLocation, FVector MaxLocation)
{
  float SpawnersHeight = 300.f;
  const auto Waypoints = OpenDriveMap->GenerateWaypointsOnRoadEntries();
  TArray<AActor*> ActorsToMove;
  for (const auto &Wp : Waypoints)
  {
    const FTransform Trans = OpenDriveMap->ComputeTransform(Wp);
    if( Trans.GetLocation().X >= MinLocation.X && Trans.GetLocation().Y >= MinLocation.Y &&
        Trans.GetLocation().X <= MaxLocation.X && Trans.GetLocation().Y <= MaxLocation.Y)
    {
      AVehicleSpawnPoint *Spawner = UEditorLevelLibrary::GetEditorWorld()->SpawnActor<AVehicleSpawnPoint>();
      Spawner->SetActorRotation(Trans.GetRotation());
      Spawner->SetActorLocation(Trans.GetTranslation() + FVector(0.f, 0.f, SpawnersHeight));
      ActorsToMove.Add(Spawner);
    }
  }
  UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
  UEditorLevelLibrary::SaveCurrentLevel();
}

float UCustomRoads::DistanceToLaneBorder(FVector &location, int32_t lane_type) const
{
  carla::geom::Location cl(location);
  //wp = GetClosestWaypoint(pos). if distance wp - pos == lane_width --> estas al borde de la carretera
  auto wp = OpenDriveMap->GetClosestWaypointOnRoad(cl, lane_type);
  if(wp)
  {
    carla::geom::Transform ct = OpenDriveMap->ComputeTransform(*wp);
    double LaneWidth = OpenDriveMap->GetLaneWidth(*wp);
    return cl.Distance(ct.location) - LaneWidth;
  }
  return 100000.0f;
}

float UCustomRoads::GetHeight(float PosX, float PosY, bool bDrivingLane) {
  if( bDrivingLane ){
    return carla::geom::deformation::GetZPosInDeformation(PosX, PosY) +
      (carla::geom::deformation::GetZPosInDeformation(PosX, PosY) * -0.3f) -
      carla::geom::deformation::GetBumpDeformation(PosX,PosY);
  } else {
      return carla::geom::deformation::GetZPosInDeformation(PosX, PosY) + (carla::geom::deformation::GetZPosInDeformation(PosX, PosY) * -0.3f);
  }
}