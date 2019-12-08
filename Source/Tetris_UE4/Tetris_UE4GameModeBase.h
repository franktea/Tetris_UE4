// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Tetris_UE4GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TETRIS_UE4_API ATetris_UE4GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
    ATetris_UE4GameModeBase();
    
    virtual void BeginPlay() override;
};
