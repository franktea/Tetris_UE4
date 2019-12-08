// Fill out your copyright notice in the Description page of Project Settings.


#include "Pieces.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include <vector>

// Sets default values
APieces::APieces()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SceneComponent = CreateDefaultSubobject<USceneComponent>("Pieces Scene");
    RootComponent = SceneComponent;
}

// Called when the game starts or when spawned
void APieces::BeginPlay()
{
	Super::BeginPlay();
	
    SpawnBlocks();
}

// Called every frame
void APieces::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieces::SpawnBlocks()
{
    std::vector<std::vector<std::pair<float, float>>> Shapes =
    {
        {{-20.0, 0.0}, {-10.0, 0.0}, {0.0, 0.0}, {10.0, 0.0}},
        {{0.0, 10.0}, {0.0, 0.0}, {10.0, 0.0}, {20.0, 0.0}},
        {{-20.0, 0.0}, {-10.0, 0.0}, {0.0, 0.0}, {0.0, 10.0}},
        {{0.0, 0.0}, {10.0, 0.0}, {0.0, -10.0}, {10.0, -10.0}},
        {{-10.0, -10.0}, {0.0, -10.0}, {0.0, 0.0}, {10.0, 0.0}},
        {{-10.0, 0.0}, {0.0, 0.0}, {0.0, 10.0}, {10.0, 0.0}},
        {{-10.0, 0.0}, {0.0, 0.0}, {0.0, -10.0}, {10.0, -10.0}},
    };
    int Index = FMath::RandRange(0, Shapes.size()-1);
    UE_LOG(LogTemp, Warning, TEXT("index=%d"), Index);
    const std::vector<std::pair<float, float>>& YZs = Shapes[Index];
    for(auto&& YZ: YZs)
    {
        FRotator Rotation(0.0, 0.0, 0.0);
        ABlock* B = GetWorld()->SpawnActor<ABlock>(this->GetActorLocation(), Rotation);
        Blocks.Add(B);
        B->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
        B->SetActorRelativeLocation(FVector(0.0, YZ.first, YZ.second));
    }
}

void APieces::DrawDebugLines()
{
    for(ABlock* B: Blocks)
    {
        FVector V1 = B->GetActorLocation();
        FVector V2(V1.X, V1.Y, 5.0f);
        DrawDebugLine(GetWorld(), V1, V2, FColor::Green, false, 1, 0, 1);
    }
}

void APieces::TestRotate()
{
    auto RotateVector = [this](FVector OldVector){
        FVector ActorVector = this->GetActorLocation();
        FVector BlockVector = OldVector;
        FVector TempVector = BlockVector - ActorVector;
        TempVector = ActorVector + TempVector.RotateAngleAxis(90.0, FVector(1.0, 0.0, 0.0));
        return TempVector;
    };
    
    if(!CheckWillCollide(RotateVector))
    {
        UE_LOG(LogTemp, Warning, TEXT("now can rotate"));
        FRotator NewRotation =  this->GetActorRotation() + FRotator(0.0, 0.0, -90.0);
        this->SetActorRelativeRotation(NewRotation);
    }
}

void APieces::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(LogTemp, Warning, TEXT("Pieces Deleted"));
}

void APieces::Dismiss()
{
    Blocks.Empty();
}

void APieces::MoveLeft()
{
    auto MoveVectorLeft = [](FVector OldVector){
        OldVector.Y -= 10.0f;
        return OldVector;
    };
    
    if(!CheckWillCollide(MoveVectorLeft))
    {
        FVector NewLocation = GetActorLocation();
        NewLocation.Y -= 10;
        SetActorLocation(NewLocation);
    }
}

void APieces::MoveRight()
{
    auto MoveVectorRight = [](FVector OldVector){
        OldVector.Y += 10.0f;
        return OldVector;
    };

    if(!CheckWillCollide(MoveVectorRight))
    {
        FVector NewLocation = GetActorLocation();
        NewLocation.Y += 10;
        SetActorLocation(NewLocation);
    }
}

bool APieces::MoveDown()
{
    auto MoveVectorDown = [](FVector OldVector){
        OldVector.Z -= 10.0f;
        return OldVector;
    };
    
    if(!CheckWillCollide(MoveVectorDown))
    {
        FVector NewLocation = GetActorLocation();
        NewLocation.Z -= 10;
        SetActorLocation(NewLocation);
        return true;
    }
    else
    {
        return false;
    }
}

bool APieces::CheckWillCollision(std::function<FVector(FVector OldLocation)> ChangeBeforeCheck)
{
    FCollisionQueryParams Params;
    for(ABlock* B: Blocks)
    {
        Params.AddIgnoredActor(B);
    }
    
    for(ABlock* B: Blocks)
    {
        FVector TempVector = B->GetActorLocation();
        TempVector = ChangeBeforeCheck(TempVector);
        
        TArray<struct FOverlapResult> OutOverlaps;
        FCollisionShape CollisionShape;
        CollisionShape.SetBox(FVector(4.5f, 4.5f, 4.5f));
        FCollisionResponseParams ResponseParam;
        bool b = GetWorld()->OverlapMultiByChannel(OutOverlaps,
            TempVector, B->GetActorQuat(), ECollisionChannel::ECC_WorldDynamic,
            CollisionShape, Params, ResponseParam);
        
        if(b)
        {
            for(auto&& Result: OutOverlaps)
            {
                UE_LOG(LogTemp, Warning, TEXT("OverLapped with actor %s, component=%s"), *Result.GetActor()->GetName(),
                       *Result.GetComponent()->GetName());
            }
            return true;
        }
    }
    
    return false;
}
